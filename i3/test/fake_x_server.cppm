/*
 * A scripted fake X server for i3's X code (MIGRATION-PLAN.md §8.1, L2): the server end of a Unix socketpair. It answers
 * the connection setup, records every request the client sends, answers the core requests that have a reply (with
 * defaults, or with a test's handler), keeps the properties ChangeProperty sets, and sends the events a test gives it.
 *
 * The client connects to the other end with xcbpp's connect_to_fd(), so i3's X runs on it unchanged: X::connection()
 * and the libxcb compatibility layer's handle (X::c_handle()) reach the server through the same connection.
 */
module;
#include <cerrno>
#include <sys/socket.h>
#include <unistd.h>
export module fake_x_server;

import std;
import xcb;

export {
    /** Builds the bytes of a packet: little-endian, as the client writes and expects them on x86. */
    class ByteWriter {
      public:
        ByteWriter &u8(std::uint8_t value) {
            bytes_.push_back(std::byte{value});
            return *this;
        }

        ByteWriter &u16(std::uint16_t value) {
            return raw(&value, sizeof value);
        }

        ByteWriter &u32(std::uint32_t value) {
            return raw(&value, sizeof value);
        }

        ByteWriter &bytes(std::span<std::byte const> value) {
            bytes_.append_range(value);
            return *this;
        }

        ByteWriter &string(std::string_view value) {
            return bytes(std::as_bytes(std::span{value}));
        }

        ByteWriter &zeros(std::size_t n) {
            bytes_.insert(bytes_.end(), n, std::byte{0});
            return *this;
        }

        /** Pads with zeros to a multiple of four bytes. */
        ByteWriter &pad() {
            return zeros(-bytes_.size() & 3U);
        }

        [[nodiscard]] std::size_t size() const noexcept {
            return bytes_.size();
        }

        [[nodiscard]] std::vector<std::byte> take() {
            return std::move(bytes_);
        }

      private:
        ByteWriter &raw(void const *value, std::size_t n) {
            auto const *first = static_cast<std::byte const *>(value);
            bytes_.insert(bytes_.end(), first, first + n);
            return *this;
        }

        std::vector<std::byte> bytes_{};
    };

    /** A request the client sent, as the server read it. */
    struct FakeRequest {
        /** The request's sequence number: a connection's requests are numbered from 1. */
        std::uint64_t sequence{};
        /** The whole request, from its major opcode to its end, padding included. */
        std::vector<std::byte> bytes{};

        [[nodiscard]] std::uint8_t major() const {
            return u8(0);
        }

        /** The second byte: an extension request's minor opcode, or a parameter of a core request. */
        [[nodiscard]] std::uint8_t minor() const {
            return u8(1);
        }

        [[nodiscard]] std::uint8_t u8(std::size_t offset) const {
            return read<std::uint8_t>(offset);
        }

        [[nodiscard]] std::uint16_t u16(std::size_t offset) const {
            return read<std::uint16_t>(offset);
        }

        [[nodiscard]] std::int16_t i16(std::size_t offset) const {
            return read<std::int16_t>(offset);
        }

        [[nodiscard]] std::uint32_t u32(std::size_t offset) const {
            return read<std::uint32_t>(offset);
        }

        /** The 32-bit values from @p offset to the end of the request. */
        [[nodiscard]] std::vector<std::uint32_t> u32s(std::size_t offset) const {
            std::vector<std::uint32_t> values{};
            for (std::size_t at = offset; at + 4 <= bytes.size(); at += 4) {
                values.push_back(u32(at));
            }
            return values;
        }

        /** @p n bytes from @p offset, as a string. */
        [[nodiscard]] std::string string(std::size_t offset, std::size_t n) const {
            check(offset, n);
            return {reinterpret_cast<char const *>(bytes.data() + offset), n};
        }

      private:
        template <typename T>
        [[nodiscard]] T read(std::size_t offset) const {
            check(offset, sizeof(T));
            T value{};
            std::memcpy(&value, bytes.data() + offset, sizeof(T));
            return value;
        }

        void check(std::size_t offset, std::size_t n) const {
            if (offset + n > bytes.size()) {
                throw std::out_of_range{std::format("request {} (opcode {}) has {} bytes, not {}", sequence, major(),
                                                    bytes.size(), offset + n)};
            }
        }
    };

    /** What answers a request: the packets to send (a reply, an error, events), in order; or none. */
    using FakeHandler = std::function<std::vector<std::byte>(FakeRequest const &)>;

    /** An extension the fake server has: QueryExtension reports it present. */
    struct FakeExtension {
        std::string name;
        std::uint8_t major_opcode{};
        std::uint8_t first_event{};
        std::uint8_t first_error{};
        /** Answers the extension's requests (FakeXServer::on()); none if empty. */
        FakeHandler handler{};
    };

    /** What the fake server's setup says about its one screen, and which extensions it has. */
    struct FakeXServerOptions {
        std::uint16_t width{1280};
        std::uint16_t height{800};
        /** Whether the screen has a 32-bit TrueColor visual besides the 24-bit root visual. i3 then uses it, and creates
         *  a colormap for it. */
        bool argb_visual{false};
        /** None by default: QueryExtension reports every other extension absent. A test that adds one answers its
         *  requests that have a reply with the extension's handler, or with FakeXServer::on(). */
        std::vector<FakeExtension> extensions{};
    };

    /**
     * The server end of a socketpair, and a thread that reads the requests the client sends.
     *
     * Every request is recorded. The core requests that have a reply are answered, by a test's handler (on()) or by
     * a default: InternAtom, GetAtomName, GetProperty (from the properties ChangeProperty set), QueryExtension (the
     * options' extensions), GetInputFocus, QueryPointer, GetGeometry, GetWindowAttributes, QueryTree, ListProperties,
     * GetSelectionOwner, GrabPointer, GrabKeyboard, TranslateCoordinates, AllocColor, GetImage, GetKeyboardMapping and
     * GetModifierMapping. Any other core request with a reply gets a BadImplementation error and is recorded in
     * unanswered(), so that a test fails instead of hanging. An extension's requests are answered only by a handler.
     *
     * Destroy the client's connection first: a client that writes to a closed server end gets SIGPIPE.
     */
    class FakeXServer {
      public:
        using Handler = FakeHandler;

        static constexpr std::uint32_t root_window = 0x3a0;
        static constexpr std::uint32_t default_colormap = 0x20;
        static constexpr std::uint32_t root_visual = 0x21;
        static constexpr std::uint32_t argb_visual = 0x22;
        static constexpr std::uint32_t white_pixel = 0xffffff;
        static constexpr std::uint32_t black_pixel = 0;

        explicit FakeXServer(FakeXServerOptions options = {});
        ~FakeXServer();

        FakeXServer(FakeXServer const &) = delete;
        FakeXServer &operator=(FakeXServer const &) = delete;

        /** Connects a client to the other end with connect_to_fd(). Call once. Throws if the setup fails. */
        [[nodiscard]] std::unique_ptr<xcb::Connection> connect();

        /** Answers the requests with major opcode @p major with @p handler instead of the default. */
        void on(std::uint8_t major, Handler handler);

        /** The requests read so far, in order. */
        [[nodiscard]] std::vector<FakeRequest> requests() const;

        /**
         * Waits until the server has read every request @p client sent so far (a GetInputFocus round trip), and returns
         * them, in order, without that GetInputFocus. They are forgotten: the next call returns the requests sent after.
         */
        [[nodiscard]] std::vector<FakeRequest> take_requests(xcb::Connection &client);

        /** The requests that needed a reply the server didn't have: each got a BadImplementation error. */
        [[nodiscard]] std::vector<FakeRequest> unanswered() const;

        /** Sends @p event, with the sequence number of the last request read. */
        void send_event(std::array<std::byte, 32> event);

        /** Sets a property, as ChangeProperty in Replace mode does. */
        void set_property(std::uint32_t window, std::uint32_t property, std::uint32_t type, std::uint8_t format,
                          std::span<std::byte const> value);

        /** A property's value, or std::nullopt if the window doesn't have it. */
        [[nodiscard]] std::optional<std::vector<std::byte>> property(std::uint32_t window, std::uint32_t property) const;

        /** The atom for @p name, interned now if it isn't yet, as InternAtom does. */
        [[nodiscard]] std::uint32_t atom(std::string_view name);

        /** A reply to @p request: the 8-byte header with @p data in its second byte, then @p body, padded to 24 bytes or
         *  more. */
        [[nodiscard]] static std::vector<std::byte> reply(FakeRequest const &request, std::uint8_t data,
                                                          std::span<std::byte const> body = {});

        /** An error with @p code answering @p request. */
        [[nodiscard]] static std::vector<std::byte> error(FakeRequest const &request, std::uint8_t code,
                                                          std::uint32_t bad_value = 0);

      private:
        struct Property {
            std::uint32_t type{};
            std::uint8_t format{};
            std::vector<std::byte> value{};
        };

        FakeXServerOptions options_;
        int server_fd_{-1};
        int client_fd_{-1};
        std::jthread reader_{};

        mutable std::mutex mutex_{};
        std::vector<FakeRequest> requests_{};
        std::vector<FakeRequest> unanswered_{};
        std::map<std::uint8_t, Handler> handlers_{};
        std::map<std::string, std::uint32_t, std::less<>> atoms_{};
        std::map<std::uint32_t, std::string> atom_names_{};
        std::map<std::pair<std::uint32_t, std::uint32_t>, Property> properties_{};
        std::uint64_t last_sequence_{0};

        /* Serializes the writes of the reader thread and of send_event(). */
        std::mutex write_mutex_{};

        [[nodiscard]] std::vector<std::byte> setup() const;
        void run();
        [[nodiscard]] bool read_exactly(std::span<std::byte> buffer) const;
        bool write(std::span<std::byte const> bytes);
        /* The default answer; mutex_ is held. */
        [[nodiscard]] std::vector<std::byte> answer(FakeRequest const &request);
        std::uint32_t intern(std::string_view name);
    };

    /**
     * A connection whose setup came from a FakeXServer, and which is then disconnected: requests on it are dropped and
     * no reply arrives, but its setup (one screen, as FakeXServer's) stays readable. For tests whose code sends
     * requests nobody checks.
     */
    [[nodiscard]] std::unique_ptr<xcb::Connection> disconnected_connection();
}

namespace {
    /* The core requests that have a reply. */
    constexpr std::array<std::uint8_t, 40> requests_with_reply{
        3, 14, 15, 16, 17, 20, 21, 23, 26, 31, 38, 39, 40, 43, 44, 47, 48, 49, 50, 52,
        73, 83, 84, 85, 86, 87, 91, 92, 97, 98, 99, 101, 103, 106, 108, 110, 116, 117, 118, 119};

    /* The predefined atoms, from PRIMARY (1) to WM_TRANSIENT_FOR (68). */
    constexpr std::array<std::string_view, 68> predefined_atoms{
        "PRIMARY", "SECONDARY", "ARC", "ATOM", "BITMAP", "CARDINAL", "COLORMAP", "CURSOR", "CUT_BUFFER0",
        "CUT_BUFFER1", "CUT_BUFFER2", "CUT_BUFFER3", "CUT_BUFFER4", "CUT_BUFFER5", "CUT_BUFFER6", "CUT_BUFFER7",
        "DRAWABLE", "FONT", "INTEGER", "PIXMAP", "POINT", "RECTANGLE", "RESOURCE_MANAGER", "RGB_COLOR_MAP",
        "RGB_BEST_MAP", "RGB_BLUE_MAP", "RGB_DEFAULT_MAP", "RGB_GRAY_MAP", "RGB_GREEN_MAP", "RGB_RED_MAP", "STRING",
        "VISUALID", "WINDOW", "WM_COMMAND", "WM_HINTS", "WM_CLIENT_MACHINE", "WM_ICON_NAME", "WM_ICON_SIZE", "WM_NAME",
        "WM_NORMAL_HINTS", "WM_SIZE_HINTS", "WM_ZOOM_HINTS", "MIN_SPACE", "NORM_SPACE", "MAX_SPACE", "END_SPACE",
        "SUPERSCRIPT_X", "SUPERSCRIPT_Y", "SUBSCRIPT_X", "SUBSCRIPT_Y", "UNDERLINE_POSITION", "UNDERLINE_THICKNESS",
        "STRIKEOUT_ASCENT", "STRIKEOUT_DESCENT", "ITALIC_ANGLE", "X_HEIGHT", "QUAD_WIDTH", "WEIGHT", "POINT_SIZE",
        "RESOLUTION", "COPYRIGHT", "NOTICE", "FONT_NAME", "FAMILY_NAME", "FULL_NAME", "CAP_HEIGHT", "WM_CLASS",
        "WM_TRANSIENT_FOR"};

    constexpr std::uint8_t bad_implementation = 17;
}  // namespace

FakeXServer::FakeXServer(FakeXServerOptions options) : options_(std::move(options)) {
    std::array<int, 2> fds{};
    if (::socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, fds.data()) != 0) {
        throw std::system_error{errno, std::system_category(), "socketpair"};
    }
    server_fd_ = fds[0];
    client_fd_ = fds[1];
    for (auto const &extension : options_.extensions) {
        if (extension.handler) {
            handlers_.emplace(extension.major_opcode, extension.handler);
        }
    }
    for (auto const &[index, name] : std::views::enumerate(predefined_atoms)) {
        auto const atom = static_cast<std::uint32_t>(index + 1);
        atoms_.emplace(name, atom);
        atom_names_.emplace(atom, name);
    }
}

FakeXServer::~FakeXServer() {
    /* Ends the reader thread's read. */
    ::shutdown(server_fd_, SHUT_RDWR);
    if (reader_.joinable()) {
        reader_.join();
    }
    ::close(server_fd_);
    if (client_fd_ != -1) {
        ::close(client_fd_);
    }
}

std::vector<std::byte> FakeXServer::setup() const {
    constexpr std::string_view vendor{"i3 fake X server"};
    struct Depth {
        std::uint8_t depth;
        std::optional<std::uint32_t> visual;
    };
    std::vector<Depth> depths{{24, root_visual}, {1, std::nullopt}};
    if (options_.argb_visual) {
        depths.push_back({32, argb_visual});
    }

    ByteWriter screen{};
    screen.u32(root_window).u32(default_colormap).u32(white_pixel).u32(black_pixel).u32(0);
    screen.u16(options_.width).u16(options_.height).u16(options_.width / 4).u16(options_.height / 4);
    screen.u16(1).u16(1).u32(root_visual).u8(0).u8(0).u8(24).u8(static_cast<std::uint8_t>(depths.size()));
    for (auto const &depth : depths) {
        screen.u8(depth.depth).u8(0).u16(depth.visual ? 1 : 0).u32(0);
        if (depth.visual) {
            /* TrueColor, 8 bits per channel. */
            screen.u32(*depth.visual).u8(4).u8(8).u16(256).u32(0xff0000).u32(0x00ff00).u32(0x0000ff).u32(0);
        }
    }

    ByteWriter formats{};
    formats.u8(1).u8(1).u8(32).zeros(5);
    formats.u8(24).u8(32).u8(32).zeros(5);
    if (options_.argb_visual) {
        formats.u8(32).u8(32).u8(32).zeros(5);
    }

    ByteWriter body{};
    body.u32(0).u32(0x00400000).u32(0x001fffff).u32(0);
    body.u16(static_cast<std::uint16_t>(vendor.size())).u16(0xffff).u8(1);
    body.u8(static_cast<std::uint8_t>(formats.size() / 8)).u8(0).u8(0).u8(32).u8(32).u8(8).u8(255).u32(0);
    body.string(vendor).pad().bytes(formats.take()).bytes(screen.take());

    ByteWriter packet{};
    packet.u8(1).u8(0).u16(11).u16(0).u16(static_cast<std::uint16_t>(body.size() / 4)).bytes(body.take());
    return packet.take();
}

std::unique_ptr<xcb::Connection> FakeXServer::connect() {
    /* The reader thread answers the setup request. It must not be in the socket before: the client reads what
     * arrives while it writes, and would take an answer that early for a packet. */
    reader_ = std::jthread{[this] { run(); }};
    auto connection = xcb::connect_to_fd(std::exchange(client_fd_, -1), nullptr);
    if (!connection) {
        throw std::runtime_error{std::format("connect_to_fd() failed with {}", static_cast<int>(connection.error()))};
    }
    return std::move(*connection);
}

void FakeXServer::on(std::uint8_t major, Handler handler) {
    std::scoped_lock lock{mutex_};
    handlers_.insert_or_assign(major, std::move(handler));
}

std::vector<FakeRequest> FakeXServer::requests() const {
    std::scoped_lock lock{mutex_};
    return requests_;
}

std::vector<FakeRequest> FakeXServer::take_requests(xcb::Connection &client) {
    if (!xcb::xproto::get_input_focus(&client).get()) {
        throw std::runtime_error{"the GetInputFocus round trip to the fake X server failed"};
    }
    std::scoped_lock lock{mutex_};
    auto taken = std::exchange(requests_, {});
    /* The last request read is the round trip's GetInputFocus: its reply has arrived, and the client was waiting. */
    taken.pop_back();
    return taken;
}

std::vector<FakeRequest> FakeXServer::unanswered() const {
    std::scoped_lock lock{mutex_};
    return unanswered_;
}

void FakeXServer::send_event(std::array<std::byte, 32> event) {
    std::uint16_t sequence{};
    {
        std::scoped_lock lock{mutex_};
        sequence = static_cast<std::uint16_t>(last_sequence_);
    }
    std::memcpy(event.data() + 2, &sequence, sizeof sequence);
    write(event);
}

void FakeXServer::set_property(std::uint32_t window, std::uint32_t property, std::uint32_t type, std::uint8_t format,
                               std::span<std::byte const> value) {
    std::scoped_lock lock{mutex_};
    properties_.insert_or_assign(std::pair{window, property}, Property{type, format, {value.begin(), value.end()}});
}

std::optional<std::vector<std::byte>> FakeXServer::property(std::uint32_t window, std::uint32_t property) const {
    std::scoped_lock lock{mutex_};
    if (auto const it = properties_.find({window, property}); it != properties_.end()) {
        return it->second.value;
    }
    return std::nullopt;
}

std::uint32_t FakeXServer::atom(std::string_view name) {
    std::scoped_lock lock{mutex_};
    return intern(name);
}

std::uint32_t FakeXServer::intern(std::string_view name) {
    if (auto const it = atoms_.find(name); it != atoms_.end()) {
        return it->second;
    }
    auto const atom = static_cast<std::uint32_t>(atoms_.size() + 1);
    atoms_.emplace(name, atom);
    atom_names_.emplace(atom, name);
    return atom;
}

std::vector<std::byte> FakeXServer::reply(FakeRequest const &request, std::uint8_t data, std::span<std::byte const> body) {
    ByteWriter extra{};
    extra.bytes(body).pad();
    if (extra.size() < 24) {
        extra.zeros(24 - extra.size());
    }
    ByteWriter packet{};
    packet.u8(1).u8(data).u16(static_cast<std::uint16_t>(request.sequence));
    packet.u32(static_cast<std::uint32_t>((extra.size() - 24) / 4)).bytes(extra.take());
    return packet.take();
}

std::vector<std::byte> FakeXServer::error(FakeRequest const &request, std::uint8_t code, std::uint32_t bad_value) {
    ByteWriter packet{};
    packet.u8(0).u8(code).u16(static_cast<std::uint16_t>(request.sequence)).u32(bad_value);
    packet.u16(request.major() >= 128 ? request.minor() : 0).u8(request.major()).zeros(21);
    return packet.take();
}

bool FakeXServer::read_exactly(std::span<std::byte> buffer) const {
    while (!buffer.empty()) {
        ssize_t const n = ::read(server_fd_, buffer.data(), buffer.size());
        if (n == -1 && errno == EINTR) {
            continue;
        }
        if (n <= 0) {
            return false;
        }
        buffer = buffer.subspan(static_cast<std::size_t>(n));
    }
    return true;
}

bool FakeXServer::write(std::span<std::byte const> bytes) {
    std::scoped_lock lock{write_mutex_};
    while (!bytes.empty()) {
        /* MSG_NOSIGNAL: a client that has gone gives EPIPE, not SIGPIPE. */
        ssize_t const n = ::send(server_fd_, bytes.data(), bytes.size(), MSG_NOSIGNAL);
        if (n == -1 && errno == EINTR) {
            continue;
        }
        if (n <= 0) {
            return false;
        }
        bytes = bytes.subspan(static_cast<std::size_t>(n));
    }
    return true;
}

void FakeXServer::run() {
    /* The setup request: byte order, protocol version and the lengths of the authorization's name and data. */
    std::array<std::byte, 12> setup_request{};
    if (!read_exactly(setup_request)) {
        return;
    }
    std::uint16_t name_length{};
    std::uint16_t data_length{};
    std::memcpy(&name_length, setup_request.data() + 6, 2);
    std::memcpy(&data_length, setup_request.data() + 8, 2);
    std::vector<std::byte> authorization(((name_length + 3U) & ~3U) + ((data_length + 3U) & ~3U));
    if (!read_exactly(authorization) || !write(setup())) {
        return;
    }

    for (std::uint64_t sequence = 1;; ++sequence) {
        FakeRequest request{.sequence = sequence, .bytes = std::vector<std::byte>(4)};
        if (!read_exactly(request.bytes)) {
            return;
        }
        std::size_t length = request.u16(2) * std::size_t{4};
        std::size_t header = 4;
        if (length == 0) {
            /* BIG-REQUESTS' form; the server doesn't offer the extension, but reads it anyway. */
            request.bytes.resize(8);
            if (!read_exactly(std::span{request.bytes}.subspan(4))) {
                return;
            }
            length = request.u32(4) * std::size_t{4};
            header = 8;
        }
        if (length < header) {
            return;
        }
        request.bytes.resize(length);
        if (!read_exactly(std::span{request.bytes}.subspan(header))) {
            return;
        }

        std::vector<std::byte> answer_bytes{};
        Handler handler{};
        {
            std::scoped_lock lock{mutex_};
            requests_.push_back(request);
            last_sequence_ = sequence;
            if (auto const it = handlers_.find(request.major()); it != handlers_.end()) {
                handler = it->second;
            } else {
                answer_bytes = answer(request);
            }
        }
        if (handler) {
            answer_bytes = handler(request);
        }
        if (!answer_bytes.empty() && !write(answer_bytes)) {
            return;
        }
    }
}

std::vector<std::byte> FakeXServer::answer(FakeRequest const &request) {
    ByteWriter body{};
    switch (request.major()) {
        case 3: /* GetWindowAttributes: an unmapped InputOutput window that selects nothing. */
            body.u32(root_visual).u16(1).u8(0).u8(1).u32(0).u32(0).u8(0).u8(1).u8(0).u8(0);
            body.u32(default_colormap).u32(0).u32(0).u16(0).u16(0);
            return reply(request, 0, body.take());
        case 14: { /* GetGeometry: the root window's for every drawable. */
            body.u32(root_window).u16(0).u16(0).u16(options_.width).u16(options_.height).u16(0);
            return reply(request, 24, body.take());
        }
        case 15: /* QueryTree: no children. */
            body.u32(root_window).u32(request.u32(4) == root_window ? 0 : root_window).u16(0).zeros(14);
            return reply(request, 0, body.take());
        case 16: { /* InternAtom */
            std::string const name = request.string(8, request.u16(4));
            std::uint32_t atom = 0;
            if (auto const it = atoms_.find(name); it != atoms_.end()) {
                atom = it->second;
            } else if (request.minor() == 0) {
                atom = intern(name);
            }
            body.u32(atom);
            return reply(request, 0, body.take());
        }
        case 17: { /* GetAtomName */
            auto const it = atom_names_.find(request.u32(4));
            if (it == atom_names_.end()) {
                return error(request, 5, request.u32(4)); /* Atom */
            }
            body.u16(static_cast<std::uint16_t>(it->second.size())).zeros(22).string(it->second);
            return reply(request, 0, body.take());
        }
        case 18: { /* ChangeProperty: kept for GetProperty. */
            auto const mode = request.minor();
            auto const format = request.u8(16);
            std::size_t const n = request.u32(20) * std::size_t{format / 8U};
            auto const *first = request.bytes.data() + 24;
            auto &property = properties_[{request.u32(4), request.u32(8)}];
            if (mode == 0 || property.value.empty()) {
                property.value.assign(first, first + n);
            } else if (mode == 1) {
                property.value.insert(property.value.begin(), first, first + n);
            } else {
                property.value.insert(property.value.end(), first, first + n);
            }
            property.type = request.u32(12);
            property.format = format;
            return {};
        }
        case 19: /* DeleteProperty */
            properties_.erase({request.u32(4), request.u32(8)});
            return {};
        case 20: { /* GetProperty */
            auto const it = properties_.find({request.u32(4), request.u32(8)});
            if (it == properties_.end()) {
                body.u32(0).u32(0).u32(0);
                return reply(request, 0, body.take());
            }
            Property const &property = it->second;
            std::uint32_t const type = request.u32(12);
            if (type != 0 && type != property.type) {
                body.u32(property.type).u32(static_cast<std::uint32_t>(property.value.size())).u32(0);
                return reply(request, property.format, body.take());
            }
            std::size_t const offset = std::min<std::size_t>(request.u32(16) * std::size_t{4}, property.value.size());
            std::size_t const n = std::min<std::size_t>(request.u32(20) * std::size_t{4}, property.value.size() - offset);
            std::size_t const after = property.value.size() - offset - n;
            body.u32(property.type).u32(static_cast<std::uint32_t>(after)).u32(static_cast<std::uint32_t>(n / (property.format / 8U)));
            body.zeros(12).bytes(std::span{property.value}.subspan(offset, n));
            auto packet = reply(request, property.format, body.take());
            if (request.minor() != 0 && after == 0) {
                properties_.erase(it);
            }
            return packet;
        }
        case 21: /* ListProperties: none. */
            return reply(request, 0, body.u16(0).take());
        case 23: /* GetSelectionOwner: none. */
            return reply(request, 0, body.u32(0).take());
        case 26: /* GrabPointer: Success. */
        case 31: /* GrabKeyboard: Success. */
            return reply(request, 0);
        case 38: /* QueryPointer: at (0, 0) on the root window. */
            body.u32(root_window).u32(0).u16(0).u16(0).u16(0).u16(0).u16(0);
            return reply(request, 1, body.take());
        case 40: /* TranslateCoordinates: the same coordinates. */
            body.u32(0).u16(request.u16(12)).u16(request.u16(14));
            return reply(request, 1, body.take());
        case 43: /* GetInputFocus: PointerRoot. */
            return reply(request, 1, body.u32(1).take());
        case 73: { /* GetImage: black, at depth 24, 32 bits per pixel. */
            std::size_t const n = std::size_t{request.u16(12)} * request.u16(14) * 4;
            body.u32(root_visual).zeros(20).zeros(n);
            return reply(request, 24, body.take());
        }
        case 84: /* AllocColor: the exact color, its pixel the 8-bit channels. */
            body.u16(request.u16(8)).u16(request.u16(10)).u16(request.u16(12)).u16(0);
            body.u32((std::uint32_t{request.u16(8)} >> 8U) << 16U | (std::uint32_t{request.u16(10)} >> 8U) << 8U | std::uint32_t{request.u16(12)} >> 8U);
            return reply(request, 0, body.take());
        case 98: { /* QueryExtension */
            std::string const name = request.string(8, request.u16(4));
            auto const it = std::ranges::find(options_.extensions, name, &FakeExtension::name);
            if (it == options_.extensions.end()) {
                body.u8(0).u8(0).u8(0).u8(0);
            } else {
                body.u8(1).u8(it->major_opcode).u8(it->first_event).u8(it->first_error);
            }
            return reply(request, 0, body.take());
        }
        case 101: { /* GetKeyboardMapping: one NoSymbol per keycode. */
            body.zeros(24).zeros(request.u8(5) * std::size_t{4});
            return reply(request, 1, body.take());
        }
        case 119: /* GetModifierMapping: no modifier keys. */
            return reply(request, 0);
        default:
            if (std::ranges::contains(requests_with_reply, request.major())) {
                unanswered_.push_back(request);
                return error(request, bad_implementation);
            }
            return {};
    }
}

std::unique_ptr<xcb::Connection> disconnected_connection() {
    FakeXServer server{};
    auto connection = server.connect();
    connection->disconnect();
    return connection;
}
