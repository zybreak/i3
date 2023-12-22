module;
#include <cstdint>
module i3;

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