#ifndef COORDINATE_H
#define COORDINATE_H

#include <cmath>
#include <iosfwd>   // lightweight forward-declaration of std::ostream
// FIX: original included <iostream> in the header — only needed in the .cpp.

class Coordinate {
private:
    float x, y, z;   // 3-D position

public:
    explicit Coordinate(float xVal = 0.0f, float yVal = 0.0f, float zVal = 0.0f);

    void  resetPosition();
    void  moveTo(float newX, float newY, float newZ);
    void  translate(float dx, float dy, float dz);
    void  display() const;
    float distanceTo(const Coordinate& other) const;

    // Accessors — needed by SimulationEngine and others
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

    // Operator Overloads
    float      operator-(const Coordinate& other) const;  // distance
    Coordinate operator+(const Coordinate& other) const;  // vector addition
    bool       operator==(const Coordinate& other) const; // equality
};

#endif // COORDINATE_H
