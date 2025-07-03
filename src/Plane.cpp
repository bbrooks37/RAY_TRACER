// src/Plane.cpp
#include "Plane.h"
#include <limits> // For std::numeric_limits
#include <cmath>  // For std::fabs

// The definition 'Plane::~Plane() = default;' is no longer needed here
// because it is explicitly defaulted in the header (Plane.h),
// and the compiler will generate its definition automatically.

// Implements the ray-plane intersection test.
// A ray is defined as R(t) = O + tD, where O is origin, D is direction.
// A plane is defined as (P - A) . N = 0, where P is any point on the plane, A is a known point on the plane, N is the normal.
// Substituting R(t) for P: (O + tD - A) . N = 0
// tD . N = (A - O) . N
// t = ((A - O) . N) / (D . N)
// If D . N is zero, the ray is parallel to the plane (no intersection or ray is on the plane).
// If t < 0, the intersection is behind the ray origin.
bool Plane::intersect(const Ray& ray, IntersectionInfo& info) const {
    float denom = ray.direction.dot(normal);

    // Check if the ray is parallel to the plane (or nearly parallel)
    // Use a small epsilon to account for floating-point inaccuracies
    if (std::fabs(denom) < 1e-6f) {
        return false; // Ray is parallel to the plane, no unique intersection point
    }

    // Calculate t (distance along the ray to the intersection point)
    float t = (point - ray.origin).dot(normal) / denom;

    // Check if the intersection point is in front of the ray origin
    // Use a small epsilon to avoid self-intersection issues when the ray origin is on the plane
    if (t > 1e-4f) {
        info.distance = t;
        info.point = ray.origin + ray.direction * t;
        info.normal = normal; // The normal of the plane is constant
        return true;
    }

    return false; // Intersection is behind the ray origin or too close
}
