Simple Ray Tracer This is a basic ray tracing project implemented in
C++. It demonstrates the fundamental principles of ray tracing,
including ray-object intersection, basic shading (diffuse), and shadow
casting.

Project Structure src/: Contains all the C++ source code files.

build/: Directory for build artifacts (created by CMake).

bin/: Directory for the compiled executable.

CMakeLists.txt: CMake build configuration file.

README.md: This file.

Key Files Explained src/main.cpp: This is the main entry point of the
application. It sets up the scene, defines the camera, adds objects and
lights, and then initiates the ray tracing loop. For each pixel in the
output image, it computes a primary ray, traces it into the scene to
find the closest intersection, calculates the color based on lighting
and shadows, and stores it in a framebuffer. Finally, it saves the
framebuffer content to an output.ppm image file.

CMakeLists.txt: This file is crucial for building the project across
different platforms. It tells CMake:

The minimum required CMake version.

The project name (RayTracer).

To use C++11 standard.

To include the src directory for header files.

Which source files (.cpp) to compile into the ray_tracer executable.

Where to place the compiled executable (bin/) and libraries (lib/).

Optional compiler flags for optimization and warnings.

Features Ray-Sphere Intersection: Calculates where rays intersect with
spherical objects.

Diffuse Shading: Applies basic Lambertian diffuse lighting model.

Shadows: Casts shadow rays to determine if a point is illuminated or in
shadow.

PPM Image Output: Renders the scene to a .ppm (Portable PixMap) image
file, which can be viewed with many image viewers or converted to other
formats.

Building the Project This project uses CMake for cross-platform
compilation.

Prerequisites A C++ compiler (e.g., GCC, Clang, MSVC)

CMake (version 3.10 or higher)

Steps Navigate to the project root directory:

cd ray_tracer

Create a build directory and navigate into it:

mkdir build cd build

Run CMake to configure the project:

cmake ..

(On Windows with Visual Studio, you might specify a generator, e.g.,
cmake .. -G \"Visual Studio 16 2019\")

Build the project:

cmake \--build .

This will compile the source code and create the ray_tracer executable
in the bin/ directory.

Running the Application After building, you can run the ray tracer from
the build directory:

./bin/ray_tracer

(On Windows: .\\bin\\ray_tracer.exe)

This will generate an output.ppm file in the build directory.

Viewing the Output The output.ppm file is a plain text image format. You
can open it with:

Image viewers like IrfanView (Windows), GIMP, or Photoshop.

Command-line tools like feh or ImageMagick (display output.ppm).

Many online PPM viewers.

Extending the Project Here are some ideas for extending this basic ray
tracer:

Add more geometric shapes: Implement Object derived classes for planes,
triangles, cylinders, boxes, etc.

Implement more shading models:

Specular reflection: For shiny surfaces.

Refraction: For transparent objects (e.g., glass).

Reflection: For mirror-like surfaces.

Texture mapping: Apply images to object surfaces.

Anti-aliasing: Reduce jagged edges by sampling multiple rays per pixel.

Acceleration structures: Use BVH (Bounding Volume Hierarchy) or KD-trees
for faster ray-scene intersection tests.

Different light types: Area lights, directional lights.

Gamma correction: Adjust colors for proper display on monitors.

Multithreading: Speed up rendering by utilizing multiple CPU cores.
