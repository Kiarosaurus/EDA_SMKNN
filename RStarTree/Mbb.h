#ifndef MBB_H
#define MBB_H

#include <algorithm>
#include <vector>
#include "./Point.h" // Asegúrate de que la ruta sea correcta

// struct Point {
//     float x, y;
//     Point(float _x = 0, float _y = 0) : x(_x), y(_y) {}
//     bool operator==(const Point& other) const {
//         return x == other.x && y == other.y;
//     }
// };

struct Mbb {
    Point lower_left; // esquina inferior izquierda
    Point upper_right; // esquina superior derecha

    Mbb();
    explicit Mbb(const Point& p);
    Mbb(const Point& a, const Point& b);

    float area() const;
    float margin() const; // (ancho + alto)
    float enlarge(const Mbb& other) const; // cuánto crece el área al unirlos ?
    void expand(const Mbb& other); // extiende este box (lo modifica)
    bool intersects(const Mbb& other) const; // si se cruzan
    static Mbb combine(const std::vector<Mbb>& boxes);
    float minDist(const Point& p) const; // distancia mínima al punto p
};

#endif //MBB_H
