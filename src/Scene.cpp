// src/Scene.cpp
#include "Scene.h"    // Include the header for the Scene class
#include <cmath>      // Required for std::sqrt (though not directly used in Scene.cpp, it's good practice for math ops)
#include <limits>     // Required for std::numeric_limits

// Destructor: Iterates through the objects vector and deletes each dynamically allocated object.
// This is crucial to prevent memory leaks since objects are added as raw pointers.
Scene::~Scene() {
    for (Object* obj : objects) {
        delete obj; // Deallocate memory for each object
    }
}

// Adds an object to the scene (takes ownership of the pointer).
void Scene::addObject(Object* obj) {
    objects.push_back(obj);
}

// Adds a light to the scene.
void Scene::addLight(const Light& light) {
    lights.push_back(light);
}

// Traces a ray into the scene to find the closest intersection.
// Iterates through all objects and finds the one closest to the ray origin.
bool Scene::trace(const Ray& ray, IntersectionInfo& info, Object*& hitObject) const {
    // Initialize minDist to the maximum possible float value.
    float minDist = std::numeric_limits<float>::max();
    hitObject = nullptr; // No object hit initially

    IntersectionInfo currentInfo; // Temporary struct to store intersection info for the current object

    // Loop over all objects in the scene to check for intersections.
    for (Object* obj : objects) {
        if (obj->intersect(ray, currentInfo)) { // If the ray intersects the current object...
            // Check if this intersection is closer than any previously found intersection.
            if (currentInfo.distance < minDist) {
                minDist = currentInfo.distance; // Update the minimum distance
                info = currentInfo;             // Store the detailed information of this closest intersection
                hitObject = obj;                // Store the pointer to the object that was hit
            }
        }
    }

    // Return true if an object was hit (i.e., hitObject is no longer nullptr).
    return hitObject != nullptr;
}

// Checks if a point is in shadow from a specific light source.
// This is done by casting a shadow ray from the intersection point towards the light.
bool Scene::isInShadow(const Vec3f& point, const Light& light) const {
    // Calculate the direction from the intersection point to the light source.
    Vec3f lightDir = (light.position - point).normalize();

    // Create a shadow ray. Add a small epsilon (1e-4f) to the origin of the shadow ray
    // to prevent "self-intersection" where the ray immediately hits the object it originated from
    // due to floating-point precision issues.
    Ray shadowRay(point + lightDir * 1e-4f, lightDir);

    // Calculate the actual distance from the intersection point to the light source.
    float distanceToLight = (light.position - point).length();

    // Iterate through all objects in the scene to check if any object blocks the shadow ray.
    IntersectionInfo shadowInfo;
    for (Object* obj : objects) {
        if (obj->intersect(shadowRay, shadowInfo)) {
            // If the shadow ray intersects an object AND that object is closer than the light source,
            // then the point is in shadow.
            if (shadowInfo.distance < distanceToLight) {
                return true; // Point is in shadow
            }
        }
    }
    return false; // Point is not in shadow
}
