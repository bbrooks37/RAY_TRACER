// src/Scene.h
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <limits> // For std::numeric_limits

#include "Object.h"
#include "Light.h"
#include "Ray.h"
#include "Vec3.h"

// Represents the 3D scene, containing objects and lights.
// Manages finding intersections and basic shading.
class Scene {
public:
    std::vector<Object*> objects; // Dynamic array of pointers to objects
    std::vector<Light> lights;    // Dynamic array of lights
    Vec3f backgroundColor;        // Color for rays that hit nothing

    // Constructor
    Scene(const Vec3f& bgColor = Vec3f(0.2f, 0.2f, 0.2f)) : backgroundColor(bgColor) {}

    // Destructor: Cleans up dynamically allocated objects
    ~Scene();

    // Adds an object to the scene (takes ownership of the pointer)
    void addObject(Object* obj);

    // Adds a light to the scene
    void addLight(const Light& light);

    // Traces a ray into the scene to find the closest intersection.
    // Returns true if an intersection is found, and fills the info struct.
    bool trace(const Ray& ray, IntersectionInfo& info, Object*& hitObject) const;

    // Checks if a point is in shadow from a specific light source.
    // Returns true if the point is in shadow.
    bool isInShadow(const Vec3f& point, const Light& light) const;
};

#endif // SCENE_H
