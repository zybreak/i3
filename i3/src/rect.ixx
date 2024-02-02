module;
#include <cstdint>
#include <compare>
export module rect;

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
        auto operator<=>(const Rect &r) const = default;
        /*friend bool operator==(const Rect &a, const Rect &b) {
            return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
        }*/
        Rect operator+(const Rect &r) const;
        Rect &operator+=(const Rect &r);
        Rect operator-(const Rect &r) const;
        Rect &operator-=(const Rect &r);
    };
}

bool Rect::rect_contains(uint32_t _x, uint32_t _y) {
    Rect *rect = this;

    return (_x >= rect->x &&
            _x <= (rect->x + rect->width) &&
            _y >= rect->y &&
            _y <= (rect->y + rect->height));
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
    Rect *rect = this;
    rect->width = (int32_t)rect->width <= 0 ? 1 : rect->width;
    rect->height = (int32_t)rect->height <= 0 ? 1 : rect->height;
}
