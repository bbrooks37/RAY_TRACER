// src/Ray.h
#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

// Represents a ray in 3D space with an origin and a normalized direction.
class Ray {
public:
    Vec3f origin;      // Starting point of the ray
    Vec3f direction;   // Normalized direction vector of the ray

    // Default constructor
    Ray() : origin(0), direction(0) {}
    // Constructor with origin and direction
    Ray(const Vec3f& origin, const Vec3f& direction)
        : origin(origin), direction(direction.normalize()) {} // Ensure direction is normalized
};

#endif // RAY_H
