#include "Mbb.h"

Mbb::Mbb() : lower_left(), upper_right() {}

Mbb::Mbb(const Point& p) : lower_left(p), upper_right(p) {}

Mbb::Mbb(const Point& a, const Point& b)
    : lower_left({std::min(a.x, b.x), std::min(a.y, b.y)}),
      upper_right({std::max(a.x, b.x), std::max(a.y, b.y)}) {}

float Mbb::area() const {
    return (upper_right.x - lower_left.x) * (upper_right.y - lower_left.y);
}

float Mbb::margin() const {
    return (upper_right.x - lower_left.x) + (upper_right.y - lower_left.y);
}

float Mbb::enlarge(const Mbb& o) const {
    Mbb u = *this;
    u.expand(o);
    return u.area() - area();
}

void Mbb::expand(const Mbb& o) {
    lower_left.x  = std::min(lower_left.x,  o.lower_left.x);
    lower_left.y  = std::min(lower_left.y,  o.lower_left.y);
    upper_right.x = std::max(upper_right.x, o.upper_right.x);
    upper_right.y = std::max(upper_right.y, o.upper_right.y);
}

bool Mbb::intersects(const Mbb& o) const {
    return !(o.lower_left.x > upper_right.x || o.upper_right.x < lower_left.x ||
             o.lower_left.y > upper_right.y || o.upper_right.y < lower_left.y);
}

Mbb Mbb::combine(const std::vector<Mbb>& v) {
    Mbb res = v.front();
    for (const auto& b : v)
        res.expand(b);
    return res;
}
