#include "Source\Coordinate.h"
#include <iostream>

Coordinate::Coordinate(float xVal, float yVal, float zVal)
    : x(xVal), y(yVal), z(zVal) {}

void Coordinate::resetPosition() {
    x = y = z = 0.0f;
}

void Coordinate::moveTo(float newX, float newY, float newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

void Coordinate::translate(float dx, float dy, float dz) {
    x += dx;
    y += dy;
    z += dz;
}

void Coordinate::display() const {
    std::cout << "(" << x << ", " << y << ", " << z << ")\n";
}

float Coordinate::distanceTo(const Coordinate& other) const {
    return std::sqrt(
        std::pow(x - other.x, 2.0f) +
        std::pow(y - other.y, 2.0f) +
        std::pow(z - other.z, 2.0f)
    );
}

// ── Operator Overloads ────────────────────────────────────────────────────────

float Coordinate::operator-(const Coordinate& other) const {
    return distanceTo(other);
}

Coordinate Coordinate::operator+(const Coordinate& other) const {
    return Coordinate(x + other.x, y + other.y, z + other.z);
}

bool Coordinate::operator==(const Coordinate& other) const {
    // FIX: Use epsilon comparison instead of exact float equality.
    const float epsilon = 1e-5f;
    return (std::fabs(x - other.x) < epsilon) &&
           (std::fabs(y - other.y) < epsilon) &&
           (std::fabs(z - other.z) < epsilon);
}
