// src/Utils.cpp
#include "Utils.h"     // Include the header for Utils namespace and function declarations
#include <fstream>     // Required for std::ofstream (file output operations)
#include <algorithm>   // Required for std::min and std::max
#include <cstdio>      // Required for fprintf (standard C I/O functions for error reporting)

// Definition of the savePPMImage function, which belongs to the Utils namespace.
// This function writes the pixel data from the framebuffer to a PPM (Portable PixMap) file.
// PPM is a simple image format, easy to generate and view.
void Utils::savePPMImage(const std::string& filename, int width, int height, const std::vector<Vec3f>& pixels) {
    // Open the output file stream.
    // std::ios::out flag indicates output operation (writing to file).
    // std::ios::binary flag opens the file in binary mode for raw byte output, which is generally
    // more efficient and safer for image data, though PPM P3 format is ASCII.
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);

    // Check if the file was successfully opened.
    if (!ofs.is_open()) {
        // If not, print an error message to the standard error stream (stderr).
        fprintf(stderr, "Error: Could not open file %s for writing.\n", filename.c_str());
        return; // Exit the function as we cannot proceed without a valid file handle.
    }

    // Write the PPM header.
    // "P3" indicates an ASCII color image.
    ofs << "P3\n";
    // Write the image width and height, separated by a space.
    ofs << width << " " << height << "\n";
    // Write the maximum color value (255 for 8-bit color components).
    ofs << "255\n";

    // Loop through each pixel of the image, row by row, then column by column.
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            // Get the color of the current pixel from the framebuffer.
            // The framebuffer is a 1D vector, so we calculate the index: row * width + column.
            const Vec3f& pixelColor = pixels[j * width + i];

            // Clamp each color component (red, green, blue) to the valid range [0, 1].
            // Then, scale it to the [0, 255] integer range for PPM output.
            // Using 255.99f ensures proper rounding when casting to int.
            int r = static_cast<int>(255.99f * std::min(1.0f, std::max(0.0f, pixelColor.x)));
            int g = static_cast<int>(255.99f * std::min(1.0f, std::max(0.0f, pixelColor.y)));
            int b = static_cast<int>(255.99f * std::min(1.0f, std::max(0.0f, pixelColor.z)));

            // Write the RGB components for the current pixel to the file,
            // separated by spaces, followed by a newline.
            ofs << r << " " << g << " " << b << "\n";
        }
    }

    // Close the file stream to ensure all data is written and resources are released.
    ofs.close();

    // Print a success message to the standard output stream (stdout).
    fprintf(stdout, "Image saved to %s\n", filename.c_str());
}
