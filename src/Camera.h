// src/Camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "Vec3.h"
#include "Ray.h"

// Represents the camera in the scene, responsible for generating primary rays.
class Camera {
public:
    Vec3f eyePosition;    // Position of the camera (eye)
    Vec3f lookAt;         // Point the camera is looking at
    Vec3f upVector;       // Up direction for the camera
    float fov;            // Field of View in degrees

    // Image dimensions
    int imageWidth;
    int imageHeight;

    // Camera basis vectors (u, v, w)
    Vec3f u, v, w;

    // Constructor
    Camera(const Vec3f& eye, const Vec3f& look, const Vec3f& up, float fov_deg, int width, int height);

    // Computes the primary ray for a given pixel (i, j)
    Ray computePrimaryRay(int i, int j) const;
};

#endif // CAMERA_H
