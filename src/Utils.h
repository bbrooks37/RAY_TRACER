// src/Utils.h
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "Vec3.h"

// Utility functions for the ray tracer.
namespace Utils {
    // Writes the framebuffer data to a PPM (Portable PixMap) image file.
    // PPM is a simple image format that can be easily viewed.
    void savePPMImage(const std::string& filename, int width, int height, const std::vector<Vec3f>& pixels);
}

#endif // UTILS_H
