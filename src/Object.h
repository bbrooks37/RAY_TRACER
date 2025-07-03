// src/Object.h
#ifndef OBJECT_H
#define OBJECT_H

#include "Vec3.h" // Required for Vec3f
#include "Ray.h"  // Required for Ray class definition

// Structure to hold intersection information
struct IntersectionInfo {
    Vec3f point;    // Point of intersection
    Vec3f normal;   // Normal at the intersection point
    float distance; // Distance from ray origin to intersection point
};

// Abstract base class for all geometric objects in the scene.
// Defines common properties like color and an interface for intersection testing.
class Object {
public:
    Vec3f color; // Color of the object

    // Constructor
    Object(const Vec3f& col = Vec3f(0.5f)) : color(col) {}

    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~Object() = default;

    // Pure virtual function for ray-object intersection.
    // Derived classes must implement this.
    // Returns true if an intersection occurs, and fills the IntersectionInfo struct.
    virtual bool intersect(const Ray& ray, IntersectionInfo& info) const = 0;
};

#endif // OBJECT_H
