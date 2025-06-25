#ifndef POINT_H
#define POINT_H

#include <array>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <random>
#include <iostream>
using namespace std;

constexpr float EPSILON = 1e-8f;

class Point {
private:
    int id = -1; //Necesario para rastrear el índice
    vector<float> coordinates_;

public:
    Point();
    explicit Point(const vector<float>& coordinates);
    Point(float x, float y); // Constructor para R^2 (mantiene compatibilidad)
    Point(float x, float y, float z); // Constructor para R^3
    explicit Point(size_t dimensions); // Constructor para n dimensiones
    
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

    size_t dimensions() const { return coordinates_.size(); }
    
    static Point random(size_t dims, float min = 0.0f, float max = 1.0f);
    static float distance(const Point& p1, const Point& p2);
    static float distance2(const Point& p1, const Point& p2);
    
    bool operator==(const Point& other) const {
        if (dimensions() != other.dimensions()) return false;
        for (size_t i = 0; i < dimensions(); ++i) {
            if (fabs(coordinates_[i] - other.coordinates_[i]) > EPSILON)
                return false;
        }
        return true;
    }

    friend ostream& operator<<(ostream& os, const Point& point);

    int getId() const { return id; }
    void setId(int newId) { id = newId; }
    
    // Métodos para trabajar con datos de rostros
    static Point fromFaceData(const vector<float>& faceData, int subjectId = -1);
    vector<float> toFaceData() const;
};

#endif // POINT_H