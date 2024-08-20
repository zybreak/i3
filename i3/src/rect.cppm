module;
#import <nlohmann/json.hpp>
export module rect;

import std;

using std::uint32_t;
using std::int32_t;

export {
    /**
     * Stores a rectangle, for example the size of a window, the child window etc.
     *
     * Note that x and y can contain signed values in some cases (for example when
     * used for the coordinates of a window, which can be set outside of the
     * visible area, but not when specifying the position of a workspace for the
     * _NET_WM_WORKAREA hint). Not declaring x/y as int32_t saves us a lot of
     * typecasts.
     *
     */
    class Rect {
       public:
        uint32_t x{};
        uint32_t y{};
        uint32_t width{};
        uint32_t height{};
        bool rect_contains(uint32_t _x, uint32_t _y);
        void rect_sanitize_dimensions();
        auto operator<=>(Rect const &r) const = default;
        /*friend bool operator==(const Rect &a, const Rect &b) {
            return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
        }*/
        Rect operator+(const Rect &r) const;
        Rect &operator+=(const Rect &r);
        Rect operator-(const Rect &r) const;
        Rect &operator-=(const Rect &r);
    };

    void to_json(nlohmann::json &j, Rect const &r) {
        j = {
                { "x", r.x },
                { "y", r.y },
                { "width", r.width },
                { "height", r.height },
        };
    }

    void from_json(nlohmann::json const &j, Rect &r) {
        j.at("x").get_to(r.x);
        j.at("y").get_to(r.y);
        j.at("width").get_to(r.width);
        j.at("height").get_to(r.height);
    }

}

bool Rect::rect_contains(uint32_t _x, uint32_t _y) {
    return (_x >= this->x &&
            _x <= (this->x + this->width) &&
            _y >= this->y &&
            _y <= (this->y + this->height));
}

Rect Rect::operator+(const Rect &r) const {
    return {
        this->x + r.x,
        this->y + r.y,
        this->width + r.width,
        this->height + r.height
    };
}

Rect& Rect::operator+=(const Rect &r) {
    this->x += r.x;
    this->y += r.y;
    this->width += r.width;
    this->height += r.height;

    return *this;
}

Rect Rect::operator-(const Rect &r) const {
    return {
        this->x - r.x,
        this->y - r.y,
        this->width - r.width,
        this->height - r.height
    };
}

Rect& Rect::operator-=(const Rect &r) {
    this->x -= r.x;
    this->y -= r.y;
    this->width -= r.width;
    this->height -= r.height;

    return *this;
}

void Rect::rect_sanitize_dimensions() {
    this->width = static_cast<int32_t>(this->width) <= 0 ? 1 : this->width;
    this->height = static_cast<int32_t>(this->height) <= 0 ? 1 : this->height;
}
