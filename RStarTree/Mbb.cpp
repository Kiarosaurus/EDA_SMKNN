#include "Mbb.h"

Mbb::Mbb() : lower_left(), upper_right() {}

Mbb::Mbb(const Point& p) : lower_left(p), upper_right(p) {}

Mbb::Mbb(const Point& a, const Point& b) {
    if (a.dimensions() != b.dimensions()) {
        throw std::invalid_argument("Points must have same dimensions for MBB construction");
    }
    
    size_t dims = a.dimensions();
    vector<float> lower_coords(dims);
    vector<float> upper_coords(dims);
    
    for (size_t i = 0; i < dims; ++i) {
        lower_coords[i] = std::min(a[i], b[i]);
        upper_coords[i] = std::max(a[i], b[i]);
    }
    
    lower_left = Point(lower_coords);
    upper_right = Point(upper_coords);
}

float Mbb::area() const {
    if (lower_left.dimensions() == 0) return 0.0f;
    
    float area = 1.0f;
    for (size_t i = 0; i < lower_left.dimensions(); ++i) {
        area *= (upper_right[i] - lower_left[i]);
    }
    return area;
}

float Mbb::margin() const {
    if (lower_left.dimensions() == 0) return 0.0f;
    
    float margin = 0.0f;
    for (size_t i = 0; i < lower_left.dimensions(); ++i) {
        margin += (upper_right[i] - lower_left[i]);
    }
    return margin;
}

float Mbb::enlarge(const Mbb& o) const {
    Mbb u = *this;
    u.expand(o);
    return u.area() - area();
}

void Mbb::expand(const Mbb& o) {
    if (lower_left.dimensions() != o.lower_left.dimensions()) {
        throw std::invalid_argument("MBBs must have same dimensions for expansion");
    }
    
    for (size_t i = 0; i < lower_left.dimensions(); ++i) {
        lower_left[i] = std::min(lower_left[i], o.lower_left[i]);
        upper_right[i] = std::max(upper_right[i], o.upper_right[i]);
    }
}

bool Mbb::intersects(const Mbb& o) const {
    if (lower_left.dimensions() != o.lower_left.dimensions()) {
        return false;
    }
    
    for (size_t i = 0; i < lower_left.dimensions(); ++i) {
        if (o.lower_left[i] > upper_right[i] || o.upper_right[i] < lower_left[i]) {
            return false;
        }
    }
    return true;
}

Mbb Mbb::combine(const std::vector<Mbb>& v) {
    if (v.empty()) return Mbb();
    if (v.size() == 1) return v[0];
    
    // Usar el primer MBB como base y expandir con los demás
    Mbb res = v[0];
    for (size_t i = 1; i < v.size(); ++i) {
        res.expand(v[i]);
    }
    return res;
}

float Mbb::minDist(const Point& p) const {
    if (lower_left.dimensions() != p.dimensions()) {
        throw std::invalid_argument("Point and MBB must have same dimensions");
    }
    
    // Distancia cuadrática mínima de un punto a este MBB.
    // Si el punto está dentro de los límites en un eje,
    // la contribución de ese eje es 0.
    float dist2 = 0.0f;
    
    for (size_t i = 0; i < p.dimensions(); ++i) {
        float dx = 0.0f;
        if (p[i] < lower_left[i]) {
            dx = lower_left[i] - p[i];
        } else if (p[i] > upper_right[i]) {
            dx = p[i] - upper_right[i];
        }
        dist2 += dx * dx;
    }
    
    return dist2;
}

Mbb Mbb::fromFaceData(const std::vector<Point>& facePoints) {
    if (facePoints.empty()) return Mbb();
    
    size_t dims = facePoints[0].dimensions();
    vector<float> min_coords(dims, std::numeric_limits<float>::max());
    vector<float> max_coords(dims, std::numeric_limits<float>::lowest());
    
    for (const auto& point : facePoints) {
        if (point.dimensions() != dims) {
            throw std::invalid_argument("All face points must have same dimensions");
        }
        
        for (size_t i = 0; i < dims; ++i) {
            min_coords[i] = std::min(min_coords[i], point[i]);
            max_coords[i] = std::max(max_coords[i], point[i]);
        }
    }
    
    return Mbb(Point(min_coords), Point(max_coords));
}

size_t Mbb::dimensions() const {
    return lower_left.dimensions();
}

bool Mbb::operator==(const Mbb& other) const {
    if (lower_left.dimensions() != other.lower_left.dimensions()) {
        return false;
    }
    
    for (size_t i = 0; i < lower_left.dimensions(); ++i) {
        if (lower_left[i] != other.lower_left[i] || upper_right[i] != other.upper_right[i]) {
            return false;
        }
    }
    return true;
}

