// src/Sphere.cpp
#include "Sphere.h" // Include the header for Sphere class, which also includes Ray.h and Object.h (for IntersectionInfo)
#include <cmath>    // For std::sqrt

// Implements the ray-sphere intersection test.
// This uses the quadratic formula to find intersection points.
// A ray is defined as P(t) = O + tD, where O is origin, D is direction, t is distance.
// A sphere is defined as ||P - C||^2 = R^2, where C is center, R is radius.
// Substituting P(t) into the sphere equation gives a quadratic equation in t:
// (D . D)t^2 + 2(D . (O - C))t + ((O - C) . (O - C) - R^2) = 0
// Let A = D . D (which is 1 since D is normalized)
// Let B = 2 * (D . (O - C))
// Let C = (O - C) . (O - C) - R^2
// The discriminant is delta = B^2 - 4AC
// If delta < 0, no real roots, no intersection.
// If delta = 0, one real root, ray touches sphere.
// If delta > 0, two real roots, ray intersects sphere at two points.
bool Sphere::intersect(const Ray& ray, IntersectionInfo& info) const {
    Vec3f oc = ray.origin - center; // Vector from ray origin to sphere center
    float a = ray.direction.dot(ray.direction); // Should be 1 if ray.direction is normalized
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false; // No real intersection
    } else {
        float t0 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        float t1 = (-b + std::sqrt(discriminant)) / (2.0f * a);

        // We need the closest intersection point that is in front of the ray origin (t > 0)
        float t = -1.0f; // Initialize with an invalid distance

        // Check the smaller root first
        if (t0 > 1e-4f) { // Use a small epsilon to avoid self-intersection issues
            t = t0;
        }
        // If t0 is not valid, check t1
        else if (t1 > 1e-4f) {
            t = t1;
        }

        if (t > 0) { // Valid intersection found
            info.distance = t;
            info.point = ray.origin + ray.direction * t;
            info.normal = (info.point - center).normalize(); // Normal points outwards from sphere center
            return true;
        }
    }
    return false; // No valid intersection in front of the ray
}
