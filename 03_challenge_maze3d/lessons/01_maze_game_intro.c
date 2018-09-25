/*******************************************************************************************
*
*   Challenge 03:   MAZE GAME
*   Lesson 01:      OpenGL intro
*   Description:    Introduction to OpenGL 3.3 Core profile
*
*   Compile rglfw module using:
*       gcc -c external/rglfw.c -Wall -std=c99 -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
*
*   NOTE: This example also requires the following single-file header-only modules:
*       glad.h    - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h - Vector and matrix math functions
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -Iexternal -Iexternal/glfw/include \
*           rglfw.o -lopengl32 -lgdi32 -Wall -std=c99
*
*   Copyright (c) 2017-2018 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#define GLAD_IMPLEMENTATION
#include "glad.h"               // GLAD extensions loading library
                                // NOTE: Includes required OpenGL headers
                                
#include <GLFW/glfw3.h>         // Windows/Context and inputs management

#define RAYMATH_STANDALONE
#define RAYMATH_IMPLEMENTATION
#include "raymath.h"            // Vector3 and Matrix math functions

#include <stdarg.h>             // Required for TraceLog()

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// LESSON 01: Basic data types
#ifndef __cplusplus
// Boolean type
typedef enum { false, true } bool;
#endif

// Color type, RGBA (32bit)
typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

// Rectangle type
typedef struct Rectangle {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

// Camera type, defines a camera position/orientation in 3d space
typedef struct Camera {
    Vector3 position;       // Camera position
    Vector3 target;         // Camera target it looks-at
    Vector3 up;             // Camera up vector (rotation over its axis)
    float fovy;             // Camera field-of-view apperture in Y (degrees)
} Camera;

typedef enum { LOG_INFO = 0, LOG_ERROR, LOG_WARNING, LOG_DEBUG, LOG_OTHER } TraceLogType;

//----------------------------------------------------------------------------------
// Global Variables Declaration
//----------------------------------------------------------------------------------
GLFWwindow *window;

static Matrix matProjection;                // Projection matrix to draw our world
static Matrix matModelview;                 // Modelview matrix to draw our world

static double currentTime, previousTime;    // Used to track timmings
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

// GLFW3 callback functions to be registered: Error, Key, MouseButton, MouseCursor
static void ErrorCallback(int error, const char* description);
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
static void MouseCursorPosCallback(GLFWwindow *window, double x, double y);

void TraceLog(int msgType, const char *text, ...);      // Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)

// LESSON 01: Window and context creation, extensions loading
//----------------------------------------------------------------------------------
static void InitWindow(int width, int height);          // Initialize window and context
static void InitGraphicsDevice(int width, int height);  // Initialize graphic device
static void CloseWindow(void);                          // Close window and free resources
static void SetTargetFPS(int fps);                      // Set target FPS (maximum)
static void SyncFrame(void);                            // Synchronize to desired framerate

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    // LESSON 02: Window and graphic device initialization and management
    InitWindow(screenWidth, screenHeight);          // Initialize Window using GLFW3
    
    InitGraphicsDevice(screenWidth, screenHeight);  // Initialize graphic device (OpenGL)
    
    // Define our camera
    Camera camera;
    camera.position = Vector3One();
    camera.target = Vector3Zero();
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    
    // Calculate projection matrix (from perspective) and view matrix from camera look at
    matProjection = MatrixPerspective(camera.fovy*DEG2RAD, (double)screenWidth/(double)screenHeight, 0.01, 1000.0);
    matModelview = MatrixLookAt(camera.position, camera.target, camera.up);
    
    // 2D projection
    // matProjection = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
    // matModelview = MatrixIdentity();
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------    

    // Main game loop     
    while (!glfwWindowShouldClose(window))
    {
        // Update
        //----------------------------------------------------------------------------------
        // ...
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear used buffers: Color and Depth (Depth is used for 3D)
        
        // TODO: Draw on the screen
        
        glfwSwapBuffers(window);            // Swap buffers: show back buffer into front
        glfwPollEvents();                   // Register input events (keyboard, mouse)
        SyncFrame();                        // Wait required time to target framerate
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();
    //--------------------------------------------------------------------------------------
    
    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// GLFW3: Error callback function
static void ErrorCallback(int error, const char* description)
{
    TraceLog(LOG_ERROR, description);
}

// GLFW3: Keyboard callback function
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

// GLFW3: Mouse buttons callback function
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    // ...
}

// GLFW3: Mouse cursor callback function
static void MouseCursorPosCallback(GLFWwindow *window, double x, double y)
{
    // ...
}

// Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
void TraceLog(int msgType, const char *text, ...)
{
    va_list args;
    va_start(args, text);

    switch (msgType)
    {
        case LOG_INFO: fprintf(stdout, "INFO: "); break;
        case LOG_ERROR: fprintf(stdout, "ERROR: "); break;
        case LOG_WARNING: fprintf(stdout, "WARNING: "); break;
        case LOG_DEBUG: fprintf(stdout, "DEBUG: "); break;
        default: break;
    }

    vfprintf(stdout, text, args);
    fprintf(stdout, "\n");

    va_end(args);

    if (msgType == LOG_ERROR) exit(1);
}

// LESSON 01: Window and context creation, extensions loading
//----------------------------------------------------------------------------------
// Initialize window and context (OpenGL 3.3)
static void InitWindow(int width, int height)
{
    // GLFW3 Initialization + OpenGL 3.3 Context + Extensions
    glfwSetErrorCallback(ErrorCallback);
    
    if (!glfwInit()) TraceLog(LOG_WARNING, "GLFW3: Can not initialize GLFW");
    else TraceLog(LOG_INFO, "GLFW3: GLFW initialized successfully");
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
   
    window = glfwCreateWindow(width, height, "CHALLENGE 03: 3D MAZE GAME", NULL, NULL);
    
    if (!window) glfwTerminate();
    else TraceLog(LOG_INFO, "GLFW3: Window created successfully");
    
    glfwSetWindowPos(window, 200, 200);
    
    glfwSetKeyCallback(window, KeyCallback);                    // Track keyboard events
    glfwSetMouseButtonCallback(window, MouseButtonCallback);    // Track mouse button events
    glfwSetCursorPosCallback(window, MouseCursorPosCallback);   // Track mouse position changes
    
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// Disable cursor for first person camera
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
}

// Initialize graphic device (OpenGL 3.3)
static void InitGraphicsDevice(int width, int height)
{
    // Load OpenGL 3.3 supported extensions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) TraceLog(LOG_WARNING, "GLAD: Cannot load OpenGL extensions");
    else TraceLog(LOG_INFO, "GLAD: OpenGL extensions loaded successfully");
    
    // Print current OpenGL and GLSL version
    TraceLog(LOG_INFO, "GPU: Vendor:   %s", glGetString(GL_VENDOR));
    TraceLog(LOG_INFO, "GPU: Renderer: %s", glGetString(GL_RENDERER));
    TraceLog(LOG_INFO, "GPU: Version:  %s", glGetString(GL_VERSION));
    TraceLog(LOG_INFO, "GPU: GLSL:     %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize OpenGL context (states and resources)
    //----------------------------------------------------------
    
    // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glEnable(GL_DEPTH_TEST);                                // Enable depth testing for 3D

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
    glCullFace(GL_BACK);                                    // Cull the back face (default)
    glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
    glEnable(GL_CULL_FACE);                                 // Enable backface culling

    // Init state: Color/Depth buffers clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
    glClearDepth(1.0f);                                     // Set clear depth value (default)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)

    TraceLog(LOG_INFO, "OpenGL default states initialized successfully");

    // Initialize viewport
    glViewport(0, 0, width, height);
    
    // Init internal matProjection and matModelview matrices
    matProjection = MatrixIdentity();
    matModelview = MatrixIdentity();
}

// Close window and free resources
static void CloseWindow(void)
{
    glfwDestroyWindow(window);      // Close window
    glfwTerminate();                // Free GLFW3 resources
}

// Set target FPS (maximum)
static void SetTargetFPS(int fps)
{
    if (fps < 1) targetTime = 0.0;
    else targetTime = 1.0/(double)fps;
}

// Synchronize to desired framerate
static void SyncFrame(void)
{
    // Frame time control system
    currentTime = glfwGetTime();
    frameTime = currentTime - previousTime;
    previousTime = currentTime;

    // Wait for some milliseconds...
    if (frameTime < targetTime)
    {
        double prevTime = glfwGetTime();
        double nextTime = 0.0;

        // Busy wait loop
        while ((nextTime - prevTime) < (targetTime - frameTime)) nextTime = glfwGetTime();

        currentTime = glfwGetTime();
        double extraTime = currentTime - previousTime;
        previousTime = currentTime;

        frameTime += extraTime;
    }
}
