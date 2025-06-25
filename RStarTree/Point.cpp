#include "Point.h"

Point::Point() : coordinates_() {}

Point::Point(const vector<float>& coordinates) : coordinates_(coordinates) {}

Point::Point(float x, float y) : coordinates_({x, y}) {}

Point::Point(float x, float y, float z) : coordinates_({x, y, z}) {}

Point::Point(size_t dimensions) : coordinates_(dimensions, 0.0f) {}

Point Point::operator+(const Point& other) const {
    if (dimensions() != other.dimensions()) {
        throw invalid_argument("Points must have same dimensions for addition");
    }
    Point result(dimensions());
    for (size_t i = 0; i < dimensions(); ++i) {
        result.coordinates_[i] = coordinates_[i] + other.coordinates_[i];
    }
    return result;
}

Point& Point::operator+=(const Point& other) {
    if (dimensions() != other.dimensions()) {
        throw invalid_argument("Points must have same dimensions for addition");
    }
    for (size_t i = 0; i < dimensions(); ++i) {
        coordinates_[i] += other.coordinates_[i];
    }
    return *this;
}

Point Point::operator-(const Point& other) const {
    if (dimensions() != other.dimensions()) {
        throw invalid_argument("Points must have same dimensions for subtraction");
    }
    Point result(dimensions());
    for (size_t i = 0; i < dimensions(); ++i) {
        result.coordinates_[i] = coordinates_[i] - other.coordinates_[i];
    }
    return result;
}

Point& Point::operator-=(const Point& other) {
    if (dimensions() != other.dimensions()) {
        throw invalid_argument("Points must have same dimensions for subtraction");
    }
    for (size_t i = 0; i < dimensions(); ++i) {
        coordinates_[i] -= other.coordinates_[i];
    }
    return *this;
}

Point Point::operator*(float scalar) const {
    Point result(dimensions());
    for (size_t i = 0; i < dimensions(); ++i) {
        result.coordinates_[i] = coordinates_[i] * scalar;
    }
    return result;
}

Point& Point::operator*=(float scalar) {
    for (size_t i = 0; i < dimensions(); ++i) {
        coordinates_[i] *= scalar;
    }
    return *this;
}

Point Point::operator/(float scalar) const {
    if (abs(scalar) < EPSILON) {
        throw invalid_argument("Division by zero");
    }
    Point result(dimensions());
    for (size_t i = 0; i < dimensions(); ++i) {
        result.coordinates_[i] = coordinates_[i] / scalar;
    }
    return result;
}

Point& Point::operator/=(float scalar) {
    if (abs(scalar) < EPSILON) {
        throw invalid_argument("Division by zero");
    }
    for (size_t i = 0; i < dimensions(); ++i) {
        coordinates_[i] /= scalar;
    }
    return *this;
}

float Point::norm() const {
    float sum = 0.0f;
    for (size_t i = 0; i < dimensions(); ++i) {
        sum += coordinates_[i] * coordinates_[i];
    }
    return sqrt(sum);
}

float Point::operator[](size_t index) const {
    if (index >= dimensions()) {
        throw out_of_range("Index out of range");
    }
    return coordinates_[index];
}

float& Point::operator[](size_t index) {
    if (index >= dimensions()) {
        throw out_of_range("Index out of range");
    }
    return coordinates_[index];
}

static mt19937& global_engine() {
    static random_device rd;
    static mt19937 eng(rd());
    return eng;
}

Point Point::random(size_t dims, float min, float max) {
    uniform_real_distribution<float> dis(min, max);
    vector<float> coords(dims);
    for (auto& c : coords) c = dis(global_engine());
    return Point(coords);
}

float Point::distance(const Point& p1, const Point& p2) {
    if (p1.dimensions() != p2.dimensions()) {
        throw invalid_argument("Points must have same dimensions for distance calculation");
    }
    float sum = 0.0f;
    for (size_t i = 0; i < p1.dimensions(); ++i) {
        float diff = p1.coordinates_[i] - p2.coordinates_[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

float Point::distance2(const Point& p1, const Point& p2) {
    if (p1.dimensions() != p2.dimensions()) {
        throw invalid_argument("Points must have same dimensions for distance calculation");
    }
    float sum = 0.0f;
    for (size_t i = 0; i < p1.dimensions(); ++i) {
        float diff = p1[i] - p2[i];
        sum += diff * diff;
    }
    return sum;  // ¡SIN sqrt!
}

ostream& operator<<(ostream& os, const Point& point) {
    if (point.coordinates_.empty()) {
        os << "Point()";
        return os;
    }
    os << "Point " << point.getId() << ": (";
    for (size_t i = 0; i < point.dimensions(); ++i) {
        os << point.coordinates_[i];
        if (i < point.dimensions() - 1) os << ", ";
    }
    os << ")";
    return os;
}

Point Point::fromFaceData(const vector<float>& faceData, int subjectId) {
    Point p(faceData);
    p.setId(subjectId);
    return p;
}

vector<float> Point::toFaceData() const {
    return coordinates_;
}



