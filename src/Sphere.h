// src/Sphere.h
#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h" // Required for Object base class and IntersectionInfo struct definition
#include "Vec3.h"   // Required for Vec3f
#include "Ray.h"    // Required for Ray class

// Concrete implementation of an Object representing a sphere.
class Sphere : public Object {
public:
    Vec3f center; // Center of the sphere
    float radius; // Radius of the sphere

    // Constructor: Initializes the sphere's center, radius, and color (inherited from Object).
    Sphere(const Vec3f& center, float radius, const Vec3f& color)
        : Object(color), center(center), radius(radius) {}

    // Implements the pure virtual intersect function from the Object base class.
    // This method calculates if a given ray intersects with this sphere.
    // Returns true if an intersection occurs, and fills the IntersectionInfo struct
    // with details about the closest intersection point, its normal, and distance.
    bool intersect(const Ray& ray, IntersectionInfo& info) const override;
};

#endif // SPHERE_H
