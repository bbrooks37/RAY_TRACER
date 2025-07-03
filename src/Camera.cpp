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
    // Compute camera basis vectors (w, u, v)
    // w: direction from lookAt to eye (negative camera Z-axis)
    w = (eyePosition - lookAt).normalize();
    // u: right vector (camera X-axis)
    u = upVector.cross(w).normalize();
    // v: up vector (camera Y-axis)
    v = w.cross(u); // Already normalized if u and w are orthonormal
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
