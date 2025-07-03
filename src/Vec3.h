// src/Vec3.h
#ifndef VEC3_H
#define VEC3_H

#include <cmath> // For sqrt, fabs

// A simple 3D vector class for points, directions, and colors.
// Uses float for components for simplicity in ray tracing.
class Vec3f {
public:
    float x, y, z;

    // Default constructor
    Vec3f() : x(0), y(0), z(0) {}
    // Constructor with three float components
    Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
    // Constructor with a single float component (for uniform values)
    Vec3f(float v) : x(v), y(v), z(v) {}

    // Operator overloads for vector arithmetic
    Vec3f operator+(const Vec3f& v) const { return Vec3f(x + v.x, y + v.y, z + v.z); }
    Vec3f operator-(const Vec3f& v) const { return Vec3f(x - v.x, y - v.y, z - v.z); }
    Vec3f operator*(float s) const { return Vec3f(x * s, y * s, z * s); }
    Vec3f operator/(float s) const { return Vec3f(x / s, y / s, z / s); }
    Vec3f operator*(const Vec3f& v) const { return Vec3f(x * v.x, y * v.y, z * v.z); } // Component-wise multiplication (for colors)

    // Compound assignment operators
    Vec3f& operator+=(const Vec3f& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3f& operator-=(const Vec3f& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3f& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vec3f& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

    // Dot product
    float dot(const Vec3f& v) const { return x * v.x + y * v.y + z * v.z; }

    // Cross product
    Vec3f cross(const Vec3f& v) const {
        return Vec3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    // Length (magnitude) of the vector
    float length() const { return std::sqrt(x * x + y * y + z * z); }

    // Squared length (useful for comparisons to avoid sqrt)
    float lengthSquared() const { return x * x + y * y + z * z; }

    // Normalize the vector (make its length 1)
    Vec3f normalize() const {
        float len = length();
        if (len > 0) { // Avoid division by zero
            return *this / len;
        }
        return Vec3f(0); // Return zero vector if length is zero
    }

    // Unary minus operator
    Vec3f operator-() const { return Vec3f(-x, -y, -z); }

    // Equality check with a small epsilon for floating point comparisons
    bool operator==(const Vec3f& v) const {
        const float EPSILON = 1e-6f;
        return std::fabs(x - v.x) < EPSILON && std::fabs(y - v.y) < EPSILON && std::fabs(z - v.z) < EPSILON;
    }
};

// Global operator for scalar * Vec3f
inline Vec3f operator*(float s, const Vec3f& v) {
    return v * s;
}

#endif // VEC3_H
