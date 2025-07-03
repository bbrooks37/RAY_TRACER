// src/Plane.h
#ifndef PLANE_H
#define PLANE_H

#include "Object.h" // Inherits from Object
#include "Vec3.h"   // For Vec3f
#include "Ray.h"    // For Ray

// Concrete implementation of an Object representing an infinite plane.
class Plane : public Object {
public:
    Vec3f point;  // A point on the plane
    Vec3f normal; // The normal vector of the plane (must be normalized)

    // Constructor
    Plane(const Vec3f& pointOnPlane, const Vec3f& normalVector, const Vec3f& color)
        : Object(color), point(pointOnPlane), normal(normalVector.normalize()) {} // Ensure normal is normalized

    // Implements the ray-plane intersection test.
    // Returns true if an intersection occurs, and fills the IntersectionInfo struct.
    bool intersect(const Ray& ray, IntersectionInfo& info) const override;
};

#endif // PLANE_H
