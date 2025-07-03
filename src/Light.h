// src/Light.h
#ifndef LIGHT_H
#define LIGHT_H

#include "Vec3.h"

// Represents a point light source in the scene.
class Light {
public:
    Vec3f position;  // Position of the light source
    Vec3f color;     // Color/intensity of the light

    // Constructor
    Light(const Vec3f& pos = Vec3f(0), const Vec3f& col = Vec3f(1.0f))
        : position(pos), color(col) {}
};

#endif // LIGHT_H
