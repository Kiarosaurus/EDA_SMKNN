#ifndef POINT_H
#define POINT_H

#include <array>
#include <cmath>
#include <stdexcept>
#include <random>
using namespace std;

constexpr size_t DIM = 2; //dimensiones R^2
constexpr float EPSILON = 1e-8f;

class Point {
private:
    array<float, DIM> coordinates_;
public:
    Point();
    explicit Point(const array<float, DIM>& coordinates);
    Point(float x, float y) : coordinates_{x, y} {} // Constructor para R^2
    
    Point  operator+ (const Point& other) const;
    Point& operator+=(const Point& other);
    Point  operator- (const Point& other) const;
    Point& operator-=(const Point& other);
    Point  operator* (float scalar) const;
    Point& operator*=(float scalar);
    Point  operator/ (float scalar) const;
    Point& operator/=(float scalar);
    float norm() const;

    float  operator[](size_t index) const; 
    float& operator[](size_t index);

    static Point random(float min = 0.0f, float max = 1.0f);
    static float distance(const Point& p1, const Point& p2);


};


#endif // POINT_H