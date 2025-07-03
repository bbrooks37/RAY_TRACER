# CMakeLists.txt
# This file defines how your C++ ray tracer project should be built using CMake.

# Specify the minimum required version of CMake.
# Version 3.10 is a common and stable choice that supports modern CMake features.
cmake_minimum_required(VERSION 3.10)

# Define the project name. This name will be used for the executable and other build artifacts.
project(RayTracer CXX) # CXX specifies that this is a C++ project.

# Set the C++ standard to C++11 or higher.
# C++11 is used here as it provides necessary features like range-based for loops and smart pointers (though not used extensively in this basic example, it's a good standard).
set(CMAKE_CXX_STANDARD 11)
# Ensure that the specified C++ standard is strictly enforced.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Add the 'src' directory to the include paths.
# This allows you to use include directives like `#include "Vec3.h"` directly in your source files
# without needing to specify the full path (e.g., `#include "src/Vec3.h"`).
include_directories(src)

# --- Find and Link External Libraries ---

# Find GLFW
# Find_package will try to locate GLFW on your system.
# COMPONENTS specifies required modules (e.g., glfw for the main library).
find_package(glfw3 CONFIG REQUIRED) # Use CONFIG mode for modern GLFW installations

# Find GLEW
# GLEW is often found via its header and library.
find_package(GLEW REQUIRED)

# Find Dear ImGui
# ImGui is typically added as source files or a precompiled library.
# For simplicity and cross-platform compatibility, we'll add ImGui's source files directly.
# This avoids needing to pre-build ImGui as a separate library.
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui") # Assuming imgui is a subdirectory in your project root

# Add ImGui source files to be compiled with your project.
# These are the core ImGui files and the GLFW/OpenGL backends.
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add ImGui's directory to include paths so its headers can be found.
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)

# Define the executable target
add_executable(ray_tracer
    src/main.cpp
    src/Camera.cpp
    src/Light.cpp
    src/Object.cpp
    src/Ray.cpp
    src/Scene.cpp
    src/Sphere.cpp
    src/Utils.cpp
    src/Vec3.cpp
    src/Plane.cpp
    ${IMGUI_SOURCES} # Add ImGui source files to the executable
)

# Link the executable with the found libraries.
# ${glfw3_LIBRARIES} and ${GLEW_LIBRARIES} are variables set by find_package.
# OpenGL is a system library that needs to be linked.
target_link_libraries(ray_tracer
    PRIVATE
    glfw
    GLEW::GLEW # Modern CMake target for GLEW
    GL           # Linking OpenGL directly as 'GL'
)

# Set output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Optional: Add compiler flags for optimization and warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # -Wall: Enable all common warnings.
    # -Wextra: Enable extra warnings not covered by -Wall.
    # -pedantic: Issue all warnings demanded by strict ISO C++ compliance.
    target_compile_options(ray_tracer PRIVATE -Wall -Wextra -pedantic)

    # Add debugging symbols (-g) for easier debugging with tools like GDB.
    # This is typically added when CMAKE_BUILD_TYPE is 'Debug'.
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(ray_tracer PRIVATE -g)
    else()
        # For Release or other builds, keep optimizations
        target_compile_options(ray_tracer PRIVATE -O3)
    endif()
endif()

if (MSVC)
    target_compile_options(ray_tracer PRIVATE /W4)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(ray_tracer PRIVATE /Zi) # Debug information for MSVC
    else()
        target_compile_options(ray_tracer PRIVATE /Ox)
    endif()
endif()
