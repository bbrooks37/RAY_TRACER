// src/main.cpp
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <GL/glew.h>  // For OpenGL functions (GLEW is commonly used to manage OpenGL extensions)
#include <GLFW/glfw3.h> // For window creation and input handling
#include <imgui.h>      // Dear ImGui main header
#include <imgui_impl_glfw.h> // Dear ImGui GLFW backend
#include <imgui_impl_opengl3.h> // Dear ImGui OpenGL3 backend

// Include custom header files for the ray tracer components
#include "Vec3.h"
#include "Ray.h"
#include "Object.h"
#include "Sphere.h"
#include "Light.h"
#include "Camera.h"
#include "Scene.h"
#include "Utils.h"
#include "Plane.h" // Include Plane header

// Global variables for scene elements that will be modified by the GUI
Camera* g_camera = nullptr;
Scene* g_scene = nullptr;
Object* g_selectedObject = nullptr; // Pointer to the currently selected object
IntersectionInfo g_selectedHitInfo; // Stores the intersection info for the selected object
Plane* g_groundPlane = nullptr;     // Pointer to the ground plane for exclusion

const int IMAGE_WIDTH = 640;
const int IMAGE_HEIGHT = 480;
std::vector<Vec3f> g_framebuffer(IMAGE_WIDTH * IMAGE_HEIGHT);

// OpenGL texture ID to display the ray-traced framebuffer
GLuint g_framebufferTextureID = 0;
// Shader program ID for rendering the quad
GLuint g_shaderProgram = 0;
// Vertex Array Object (VAO) and Vertex Buffer Object (VBO) for the fullscreen quad
GLuint g_quadVAO = 0;
GLuint g_quadVBO = 0;

// --- GLOBAL VARIABLES FOR MOUSE CAMERA CONTROL ---
float g_lastMouseX = IMAGE_WIDTH / 2.0f; // Initial mouse X position (center of screen)
float g_lastMouseY = IMAGE_HEIGHT / 2.0f; // Initial mouse Y position (center of screen)
bool g_firstMouse = true; // Flag to indicate if it's the first mouse movement
bool g_isRotating = false; // Flag to indicate if the camera is currently being rotated by mouse drag

float g_cameraYaw = -90.0f;  // Initial yaw angle (looking along -Z axis)
float g_cameraPitch = 0.0f; // Initial pitch angle
float g_cameraRadius = 6.0f; // Distance from lookAt point (orbital radius)
const float CAMERA_SENSITIVITY = 0.1f; // How fast the camera rotates with mouse movement
// --- END GLOBAL VARIABLES ---

// Vertex Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoord);
}
)";

// Function to compile a shader
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

// Function to link shaders into a program
GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    return program;
}

// Function to initialize OpenGL and create a texture for the framebuffer
void setupOpenGLTexture() {
    glGenTextures(1, &g_framebufferTextureID); // Generate one texture ID
    glBindTexture(GL_TEXTURE_2D, g_framebufferTextureID); // Bind it as a 2D texture

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear filtering for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear filtering for magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Clamp to edge for S coordinate
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Clamp to edge for T coordinate

    // Allocate texture memory (initially empty or with dummy data)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}

// Function to update the OpenGL texture with the current framebuffer data
void updateOpenGLTexture() {
    glBindTexture(GL_TEXTURE_2D, g_framebufferTextureID);
    // Upload the pixel data from the framebuffer to the texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_FLOAT, g_framebuffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Function to setup the fullscreen quad for rendering
void setupFullscreenQuad() {
    // Vertex data for a fullscreen quad (2 triangles)
    // Positions (x, y) and Texture Coordinates (s, t)
    // OpenGL (0,0) is bottom-left, (1,1) is top-right for textures.
    // Our framebuffer is top-left (0,0) to bottom-right (width, height).
    // So we need to flip the Y texture coordinate.
    float quadVertices[] = {
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
        -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

        -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
         1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f,  1.0f, 1.0f  // Top-right
    };

    glGenVertexArrays(1, &g_quadVAO);
    glGenBuffers(1, &g_quadVBO);
    glBindVertexArray(g_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
}


// Function to perform the ray tracing and fill the framebuffer
void renderScene() {
    // Iterate over each pixel in the image, from top-left to bottom-right.
    for (int j = 0; j < IMAGE_HEIGHT; ++j) { // Loop through rows (y-coordinate)
        for (int i = 0; i < IMAGE_WIDTH; ++i) { // Loop through columns (x-coordinate)
            // Compute the primary ray that originates from the camera's eye position
            // and passes through the center of the current pixel on the image plane.
            Ray primRay = g_camera->computePrimaryRay(i, j);

            IntersectionInfo hitInfo;    // Struct to store details about the closest intersection found
            Object* hitObject = nullptr; // Pointer to the object that was hit (nullptr if no hit)

            // Trace the primary ray into the scene.
            // If an intersection is found, hitInfo and hitObject will be populated.
            if (g_scene->trace(primRay, hitInfo, hitObject)) {
                // If an object was hit, calculate its color based on lighting.
                Vec3f finalColor = Vec3f(0.0f); // Start with black (no light contribution yet)

                // Iterate through each light source in the scene to calculate its contribution.
                for (const auto& light : g_scene->lights) {
                    // Check if the intersection point is in shadow relative to the current light.
                    if (!g_scene->isInShadow(hitInfo.point, light)) {
                        // If not in shadow, calculate the light direction vector
                        // from the hit point to the light source.
                        Vec3f lightDir = (light.position - hitInfo.point).normalize();

                        // Calculate the diffuse component using Lambert's cosine law:
                        // max(0, N . L), where N is the surface normal and L is the light direction.
                        // This ensures that light only affects surfaces facing it.
                        float diffuseFactor = std::max(0.0f, hitInfo.normal.dot(lightDir));

                        // Add the contribution of this light to the final color.
                        // The object's color is multiplied by the light's color (intensity)
                        // and the diffuse factor.
                        finalColor += hitObject->color * light.color * diffuseFactor;
                    }
                }
                // Assign the calculated final color to the current pixel in the framebuffer.
                g_framebuffer[j * IMAGE_WIDTH + i] = finalColor;
            } else {
                // If the primary ray did not hit any object, assign the scene's background color.
                g_framebuffer[j * IMAGE_WIDTH + i] = g_scene->backgroundColor;
            }
        }
    }
}

// --- Custom GLFW Callbacks (now explicitly defined and passed to ImGui's handlers) ---

// Custom mouse button callback
void customMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Always pass the event to ImGui's handler first
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    // Only process our custom logic if ImGui is NOT capturing the mouse
    if (!ImGui::GetIO().WantCaptureMouse) {
        // Handle right mouse button for camera rotation
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                g_isRotating = true;
                g_firstMouse = true; // Reset firstMouse flag when rotation starts
            } else if (action == GLFW_RELEASE) {
                g_isRotating = false;
            }
        }
        // Handle left mouse button for object picking
        else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            float ndcX = (2.0f * static_cast<float>(xpos) / IMAGE_WIDTH) - 1.0f;
            float ndcY = 1.0f - (2.0f * static_cast<float>(ypos) / IMAGE_HEIGHT);

            float fov_rad = g_camera->fov * M_PI / 180.0f;
            float aspectRatio = static_cast<float>(IMAGE_WIDTH) / IMAGE_HEIGHT;
            float halfHeight = std::tan(fov_rad / 2.0f);
            float halfWidth = halfHeight * aspectRatio;

            Vec3f rayDirection = (ndcX * halfWidth * g_camera->u + ndcY * halfHeight * g_camera->v - g_camera->w).normalize();
            Ray pickingRay(g_camera->eyePosition, rayDirection);

            float closestHitDistance = std::numeric_limits<float>::max();
            Object* potentialHitObject = nullptr;
            IntersectionInfo currentHitInfo;
            IntersectionInfo tempSelectedHitInfo;

            for (Object* obj : g_scene->objects) {
                if (obj == g_groundPlane) {
                    continue;
                }

                if (obj->intersect(pickingRay, currentHitInfo)) {
                    if (currentHitInfo.distance < closestHitDistance) {
                        closestHitDistance = currentHitInfo.distance;
                        potentialHitObject = obj;
                        tempSelectedHitInfo = currentHitInfo;
                    }
                }
            }

            g_selectedObject = potentialHitObject;
            if (g_selectedObject) {
                g_selectedHitInfo = tempSelectedHitInfo;
                std::cout << "Selected object at: (" << g_selectedHitInfo.point.x << ", " << g_selectedHitInfo.point.y << ", " << g_selectedHitInfo.point.z << ")" << std::endl;
            } else {
                std::cout << "No object selected (or ground plane hit)." << std::endl;
            }
        }
    }
}

// Custom mouse cursor position callback
void customCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Always pass the event to ImGui's handler first
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

    // Only rotate if right mouse button is pressed AND ImGui is not capturing the mouse
    if (g_isRotating && !ImGui::GetIO().WantCaptureMouse) {
        if (g_firstMouse) {
            g_lastMouseX = static_cast<float>(xpos);
            g_lastMouseY = static_cast<float>(ypos);
            g_firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos) - g_lastMouseX;
        float yoffset = g_lastMouseY - static_cast<float>(ypos); // Reversed since y-coordinates go from bottom to top

        g_lastMouseX = static_cast<float>(xpos);
        g_lastMouseY = static_cast<float>(ypos);

        xoffset *= CAMERA_SENSITIVITY;
        yoffset *= CAMERA_SENSITIVITY;

        g_cameraYaw += xoffset;
        g_cameraPitch += yoffset;

        // Clamp pitch to avoid flipping the camera upside down
        if (g_cameraPitch > 89.0f) {
            g_cameraPitch = 89.0f;
        }
        if (g_cameraPitch < -89.0f) {
            g_cameraPitch = -89.0f;
        }

        // Calculate new camera position based on yaw, pitch, and radius
        // Convert degrees to radians for trigonometric functions
        float yaw_rad = g_cameraYaw * M_PI / 180.0f;
        float pitch_rad = g_cameraPitch * M_PI / 180.0f;

        // Spherical coordinates to Cartesian
        g_camera->eyePosition.x = g_cameraRadius * std::cos(yaw_rad) * std::cos(pitch_rad);
        g_camera->eyePosition.y = g_cameraRadius * std::sin(pitch_rad);
        g_camera->eyePosition.z = g_cameraRadius * std::sin(yaw_rad) * std::cos(pitch_rad);

        // Adjust for lookAt point if it's not at the origin
        g_camera->eyePosition += g_camera->lookAt;

        g_camera->updateBasis(); // Call updateBasis here
    }
}

// Custom scroll callback
void customScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    // Add custom scroll logic here if needed, checking ImGui::GetIO().WantCaptureMouse
    if (!ImGui::GetIO().WantCaptureMouse) {
        // Example: Adjust camera radius with scroll wheel
        g_cameraRadius -= static_cast<float>(yoffset);
        if (g_cameraRadius < 1.0f) g_cameraRadius = 1.0f; // Minimum radius
        if (g_cameraRadius > 20.0f) g_cameraRadius = 20.0f; // Maximum radius

        // Recalculate camera position after radius change
        float yaw_rad = g_cameraYaw * M_PI / 180.0f;
        float pitch_rad = g_cameraPitch * M_PI / 180.0f;
        g_camera->eyePosition.x = g_cameraRadius * std::cos(yaw_rad) * std::cos(pitch_rad);
        g_camera->eyePosition.y = g_cameraRadius * std::sin(pitch_rad);
        g_camera->eyePosition.z = g_cameraRadius * std::sin(yaw_rad) * std::cos(pitch_rad);
        g_camera->eyePosition += g_camera->lookAt;
        g_camera->updateBasis();
    }
}

// Custom keyboard callback
void customKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    // Add custom key logic here if needed, checking ImGui::GetIO().WantCaptureKeyboard
}

// Custom character input callback
void customCharCallback(GLFWwindow* window, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(window, c);
    // Add custom char logic here if needed, checking ImGui::GetIO().WantCaptureKeyboard
}

// --- END Custom GLFW Callbacks ---


// Main function for the ray tracing application.
int main(void) {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version hints for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS
#endif

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(IMAGE_WIDTH, IMAGE_HEIGHT, "Interactive Ray Tracer", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW (or GLAD if preferred)
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    // IMPORTANT: Set install_callbacks to FALSE here, we will install them manually
    ImGui_ImplGlfw_InitForOpenGL(window, false); // Changed to false
    ImGui_ImplOpenGL3_Init("#version 330"); // Use GLSL version 330 for core profile

    // --- Manually set GLFW callbacks after ImGui_ImplGlfw_InitForOpenGL ---
    glfwSetMouseButtonCallback(window, customMouseButtonCallback);
    glfwSetCursorPosCallback(window, customCursorPosCallback);
    glfwSetScrollCallback(window, customScrollCallback); // Added scroll callback for camera zoom
    glfwSetKeyCallback(window, customKeyCallback);       // Added key callback
    glfwSetCharCallback(window, customCharCallback);     // Added char callback
    // --- END Manual Callback Setup ---

    // Setup OpenGL texture for framebuffer display
    setupOpenGLTexture();
    // Setup shaders and fullscreen quad
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    g_shaderProgram = createShaderProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader); // Delete shaders after linking
    glDeleteShader(fragmentShader);
    setupFullscreenQuad();

    // 2. Camera Setup
    // Initial camera setup for orbit. The actual eyePosition will be calculated
    // based on g_cameraYaw, g_cameraPitch, and g_cameraRadius.
    // The initial lookAt point is (0,0,0), and upVector is (0,1,0).
    g_camera = new Camera(
        Vec3f(0.0f, 0.0f, 0.0f),  // Placeholder eyePosition, will be updated by orbit logic
        Vec3f(0.0f, 0.0f, 0.0f),  // lookAt
        Vec3f(0.0f, 1.0f, 0.0f),  // upVector
        75.0f,                    // fov (wider view)
        IMAGE_WIDTH, IMAGE_HEIGHT
    );
    // Initialize camera's actual eye position based on initial yaw, pitch, radius
    float initial_yaw_rad = g_cameraYaw * M_PI / 180.0f;
    float initial_pitch_rad = g_cameraPitch * M_PI / 180.0f;
    g_camera->eyePosition.x = g_cameraRadius * std::cos(initial_yaw_rad) * std::cos(initial_pitch_rad);
    g_camera->eyePosition.y = g_cameraRadius * std::sin(initial_pitch_rad);
    g_camera->eyePosition.z = g_cameraRadius * std::sin(initial_yaw_rad) * std::cos(initial_pitch_rad);
    g_camera->eyePosition += g_camera->lookAt;
    g_camera->updateBasis(); // Call updateBasis here


    // 3. Scene Setup
    g_scene = new Scene(Vec3f(0.1f, 0.1f, 0.2f)); // Slightly bluish background

    // Add objects to the scene
    g_scene->addObject(new Sphere(Vec3f(0.0f, 0.5f, 0.0f), 1.0f, Vec3f(1.0f, 0.0f, 0.0f))); // Red sphere (sits on plane)
    g_scene->addObject(new Sphere(Vec3f(1.8f, 0.0f, -1.5f), 0.6f, Vec3f(0.0f, 1.0f, 0.0f))); // Green sphere (raised to be above plane)
    g_scene->addObject(new Sphere(Vec3f(-1.5f, 1.0f, 0.8f), 0.7f, Vec3f(0.0f, 0.0f, 1.0f))); // Blue sphere (already above plane)
    g_groundPlane = new Plane(Vec3f(0.0f, -1.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.8f, 0.8f, 0.8f)); // Ground Plane
    g_scene->addObject(g_groundPlane); // Add ground plane and store its pointer
    g_scene->addObject(new Sphere(Vec3f(-2.0f, 0.0f, -0.5f), 0.4f, Vec3f(1.0f, 1.0f, 0.0f))); // Yellow sphere (raised to be above plane)

    // Add light sources to the scene
    g_scene->addLight(Light(Vec3f(6.0f, 6.0f, 6.0f), Vec3f(1.0f, 1.0f, 1.0f)));
    g_scene->addLight(Light(Vec3f(-6.0f, 4.0f, 3.0f), Vec3f(0.5f, 0.8f, 1.0f)));

    // Main application loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and process events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ---------------------------------------------------------------------
        // 4. GUI Definition
        // ---------------------------------------------------------------------
        ImGui::Begin("Ray Tracer Controls"); // Create a GUI window

        // Camera Controls
        ImGui::Text("Camera Properties");
        // Display camera position, but it's now controlled by mouse orbit
        ImGui::Text("Eye Position: (%.2f, %.2f, %.2f)", g_camera->eyePosition.x, g_camera->eyePosition.y, g_camera->eyePosition.z);
        // Sliders for LookAt and FOV still allow direct input/fine-tuning
        if (ImGui::SliderFloat3("LookAt Point", &g_camera->lookAt.x, -5.0f, 5.0f)) {
            // If lookAt changes, recalculate camera position based on current yaw/pitch/radius
            // and then update camera basis vectors
            float yaw_rad = g_cameraYaw * M_PI / 180.0f;
            float pitch_rad = g_cameraPitch * M_PI / 180.0f;
            g_camera->eyePosition.x = g_cameraRadius * std::cos(yaw_rad) * std::cos(pitch_rad);
            g_camera->eyePosition.y = g_cameraRadius * std::sin(pitch_rad);
            g_camera->eyePosition.z = g_cameraRadius * std::sin(yaw_rad) * std::cos(pitch_rad);
            g_camera->eyePosition += g_camera->lookAt; // Adjust for new lookAt
            g_camera->updateBasis(); // Call updateBasis here
        }
        if (ImGui::SliderFloat("FOV", &g_camera->fov, 10.0f, 120.0f)) {
            // No direct camera basis recalculation needed for FOV, but it affects ray generation.
        }
        // New slider for camera orbital radius
        if (ImGui::SliderFloat("Orbit Radius", &g_cameraRadius, 1.0f, 20.0f)) {
            // When radius changes, recalculate camera position
            float yaw_rad = g_cameraYaw * M_PI / 180.0f;
            float pitch_rad = g_cameraPitch * M_PI / 180.0f;
            g_camera->eyePosition.x = g_cameraRadius * std::cos(yaw_rad) * std::cos(pitch_rad);
            g_camera->eyePosition.y = g_cameraRadius * std::sin(pitch_rad);
            g_camera->eyePosition.z = g_cameraRadius * std::sin(yaw_rad) * std::cos(pitch_rad);
            g_camera->eyePosition += g_camera->lookAt; // Adjust for lookAt
            g_camera->updateBasis(); // Call updateBasis here
        }
        ImGui::Separator();

        // Object Controls for Selected Object
        ImGui::Text("Selected Object Properties");
        if (g_selectedObject) {
            ImGui::Text("Type: Sphere (for now)");
            ImGui::Text("Address: %p", (void*)g_selectedObject);
            if (ImGui::ColorEdit3("Color", &g_selectedObject->color.x)) {
                // Color change will be reflected in next renderScene call
            }
        } else {
            ImGui::Text("No object selected. Click on a sphere to select it.");
        }
        ImGui::Separator();

        ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End(); // End the GUI window
        // ---------------------------------------------------------------------

        // 5. Ray Trace the Scene
        renderScene();
        updateOpenGLTexture(); // Update the OpenGL texture with the new framebuffer data

        // 6. OpenGL Rendering (Display the ray-traced image using shaders)
        glViewport(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT); // Set viewport to match image dimensions
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear with black background
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(g_shaderProgram); // Use our custom shader program
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glBindTexture(GL_TEXTURE_2D, g_framebufferTextureID); // Bind our ray-traced texture
        glUniform1i(glGetUniformLocation(g_shaderProgram, "screenTexture"), 0); // Set the sampler to texture unit 0

        glBindVertexArray(g_quadVAO); // Bind the VAO for the fullscreen quad
        glDrawArrays(GL_TRIANGLES, 0, 6); // Draw the 2 triangles (6 vertices)
        glBindVertexArray(0); // Unbind VAO

        glUseProgram(0); // Deactivate shader program
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture

        // 7. Render Dear ImGui on top of the 3D scene
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    // 8. Cleanup
    delete g_camera;
    delete g_scene;

    // Cleanup OpenGL resources
    glDeleteProgram(g_shaderProgram);
    glDeleteVertexArrays(1, &g_quadVAO);
    glDeleteBuffers(1, &g_quadVBO);
    glDeleteTextures(1, &g_framebufferTextureID);

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
