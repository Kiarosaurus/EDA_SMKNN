#include "Point.h"

Point::Point() {coordinates_.fill(0.0f); }
Point::Point(const array<float, DIM>& coordinates) : coordinates_(coordinates) {}


Point Point::operator+(const Point& other) const {
    Point result;
    for (size_t i = 0; i < DIM; ++i) {
        result.coordinates_[i] = coordinates_[i] + other.coordinates_[i];
    }
    return result;
}
Point& Point::operator+=(const Point& other) {
    for (size_t i = 0; i < DIM; ++i) {
        coordinates_[i] += other.coordinates_[i];
    }
    return *this;
}


Point Point::operator-(const Point& other) const {
    Point result;
    for (size_t i = 0; i < DIM; ++i) {
        result.coordinates_[i] = coordinates_[i] - other.coordinates_[i];
    }
    return result;
}
Point& Point::operator-=(const Point& other) {
    for (size_t i = 0; i < DIM; ++i) {
        coordinates_[i] -= other.coordinates_[i];
    }
    return *this;
}


Point Point::operator*(float scalar) const {
    Point result;
    for (size_t i = 0; i < DIM; ++i) {
        result.coordinates_[i] = coordinates_[i] * scalar;
    }
    return result;
}
Point& Point::operator*=(float scalar) {
    for (size_t i = 0; i < DIM; ++i) {
        coordinates_[i] *= scalar;
    }
    return *this;
}


Point Point::operator/(float scalar) const {
    if (abs(scalar) < EPSILON) {
        throw invalid_argument("Division by zero");
    }
    Point result;
    for (size_t i = 0; i < DIM; ++i) {
        result.coordinates_[i] = coordinates_[i] / scalar;
    }
    return result;
}
Point& Point::operator/=(float scalar) {
    if (abs(scalar) < EPSILON) {
        throw invalid_argument("Division by zero");
    }
    for (size_t i = 0; i < DIM; ++i) {
        coordinates_[i] /= scalar;
    }
    return *this;
}


float Point::norm() const {
    float sum = 0.0f;
    for (size_t i = 0; i < DIM; ++i) {
        sum += coordinates_[i] * coordinates_[i];
    }
    return sqrt(sum);
}



float Point::operator[](size_t index) const {
    if (index >= DIM) {
        throw out_of_range("Index out of range");
    }
    return coordinates_[index];
}
float& Point::operator[](size_t index) {
    if (index >= DIM) {
        throw out_of_range("Index out of range");
    }
    return coordinates_[index];
}


static mt19937& global_engine() {
    static random_device rd;
    static mt19937 eng(rd());
    return eng;
}
Point Point::random(float min, float max) {
    uniform_real_distribution<float> dis(min, max);
    array<float, DIM> coords;
    for (auto& c : coords) c = dis(global_engine());
    return Point(coords);
}


float Point::distance(const Point& p1, const Point& p2) {
    float sum = 0.0f;
    for (size_t i = 0; i < DIM; ++i) {
        float diff = p1.coordinates_[i] - p2.coordinates_[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

ostream& operator<<(ostream& os, const Point& point) {
    os << "(";
    for (size_t i = 0; i < DIM; ++i) {
        os << point.coordinates_[i];
        if (i < DIM - 1) os << ", ";
    }
    os << ")";
    return os;
}



