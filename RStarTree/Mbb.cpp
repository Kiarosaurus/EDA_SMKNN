#include "Mbb.h"

Mbb::Mbb() : lower_left(), upper_right() {}

Mbb::Mbb(const Point& p) : lower_left(p), upper_right(p) {}

Mbb::Mbb(const Point& a, const Point& b)
    : lower_left(std::min(a[0], b[0]), std::min(a[1], b[1])),
      upper_right(std::max(a[0], b[0]), std::max(a[1], b[1])) {}

float Mbb::area() const {
    return (upper_right[0] - lower_left[0]) * (upper_right[1] - lower_left[1]);
}

float Mbb::margin() const {
    return (upper_right[0] - lower_left[0]) + (upper_right[1] - lower_left[1]);
}

float Mbb::enlarge(const Mbb& o) const {
    Mbb u = *this;
    u.expand(o);
    return u.area() - area();
}

void Mbb::expand(const Mbb& o) {
    lower_left[0] = std::min(lower_left[0],  o.lower_left[0]);
    lower_left[1] = std::min(lower_left[1],  o.lower_left[1]);
    upper_right[0] = std::max(upper_right[0], o.upper_right[0]);
    upper_right[1] = std::max(upper_right[1], o.upper_right[1]);
}

bool Mbb::intersects(const Mbb& o) const {
    return !(o.lower_left[0] > upper_right[0] || o.upper_right[0] < lower_left[0] ||
             o.lower_left[1] > upper_right[1] || o.upper_right[1] < lower_left[1]);
}

Mbb Mbb::combine(const std::vector<Mbb>& v) {
    if (v.empty()) return Mbb();
    Mbb res = v.front();
    for (const auto& b : v)
        res.expand(b);
    return res;
}

float Mbb::minDist(const Point& p) const {
    // Distancia cuadrática mínima de un punto a este MBB.
    // Si el punto está dentro de los límites en un eje,
    // la contribución de ese eje es 0.
    float dx = 0.0f;
    if (p[0] < lower_left[0])             // a la izquierda del box
        dx = lower_left[0] - p[0];
    else if (p[0] > upper_right[0])       // a la derecha
        dx = p[0] - upper_right[0];

    float dy = 0.0f;
    if (p[1] < lower_left[1])             // debajo del box
        dy = lower_left[1] - p[1];
    else if (p[1] > upper_right[1])       // encima del box
        dy = p[1] - upper_right[1];

    // Distancia euclidiana al cuadrado
    return dx * dx + dy * dy;
}

