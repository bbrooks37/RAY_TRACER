// src/Camera.cpp
#include "Camera.h"
#include <cmath> // For tan, M_PI

// Define M_PI if not already defined (common in older compilers or non-GNU C++)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor: Initializes camera properties and computes its orthonormal basis.
Camera::Camera(const Vec3f& eye, const Vec3f& look, const Vec3f& up, float fov_deg, int width, int height)
    : eyePosition(eye), lookAt(look), upVector(up), fov(fov_deg), imageWidth(width), imageHeight(height) {
    updateBasis(); // Call updateBasis to initialize u,v,w
}

// Computes the primary ray for a given pixel (i, j).
// This assumes a pinhole camera model.
Ray Camera::computePrimaryRay(int i, int j) const {
    // Convert FOV from degrees to radians
    float fov_rad = fov * M_PI / 180.0f;

    // Calculate half width and half height of the image plane at distance 1 from camera
    float aspectRatio = static_cast<float>(imageWidth) / imageHeight;
    float halfHeight = std::tan(fov_rad / 2.0f);
    float halfWidth = halfHeight * aspectRatio;

    // Calculate pixel coordinates in camera space (normalized to [-1, 1])
    // Map pixel (i,j) from [0, width-1]x[0, height-1] to [-halfWidth, halfWidth]x[-halfHeight, halfHeight]
    float x_ndc = (2.0f * (i + 0.5f) / imageWidth - 1.0f) * halfWidth;
    float y_ndc = (1.0f - 2.0f * (j + 0.5f) / imageHeight) * halfHeight; // Y-axis typically points up in camera space

    // Calculate ray direction in world space
    // The ray originates from eyePosition and points towards a point on the image plane.
    // The image plane is at distance 1 along the -w direction.
    Vec3f rayDirection = (x_ndc * u + y_ndc * v - w).normalize();

    return Ray(eyePosition, rayDirection);
}

// NEW: Function to update the camera's basis vectors (u, v, w)
void Camera::updateBasis() {
    w = (eyePosition - lookAt).normalize();
    
    // Use a fixed world up vector for calculating 'u' to avoid gimbal lock issues
    // and ensure 'u' is always horizontal.
    Vec3f worldUp = Vec3f(0.0f, 1.0f, 0.0f); // Global Y-axis is always up

    // Calculate u (right vector)
    Vec3f temp_u = worldUp.cross(w);
    // If temp_u is near zero, it means 'w' is parallel to 'worldUp' (camera looking straight up/down).
    // In this edge case, we need a fallback for 'u'.
    // A small epsilon check for squared length is more robust for floats.
    if (temp_u.lengthSquared() < 1e-6f) {
        // Camera is looking almost straight up or down.
        // We can define 'u' to be along the world X-axis or a slightly perturbed vector.
        // For an orbit camera, a fixed horizontal 'u' is often desired.
        u = Vec3f(1.0f, 0.0f, 0.0f); // Fallback: World X-axis as right vector
    } else {
        u = temp_u.normalize();
    }
    
    // Calculate v (up vector) - should always be orthogonal to w and u
    v = w.cross(u);
    // Normalize v as well to ensure perfect orthogonality and unit length,
    // especially if 'u' was a fallback or had precision issues.
    v = v.normalize();
}
