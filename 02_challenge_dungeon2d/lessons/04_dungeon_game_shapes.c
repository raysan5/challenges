/*******************************************************************************************
*
*   Challenge 02:   DUNGEON GAME
*   Lesson 04:      shapes
*   Description:    Basic shapes definition and drawing (immediate mode style)
*
*   NOTE: This example requires OpenGL 3.3 or ES2 for shaders support,
*       OpenGL 1.1 does not support shaders but it can also be used.
*
*   Compile rglfw module using:
*       gcc -c external/rglfw.c -Wall -std=c99 -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
*
*   NOTE: This example also requires the following single-file header-only modules:
*       rlgl.h    - OpenGL abstraction layer for OpenGL 1.1 immediate-mode style coding
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

#define RLGL_STANDALONE
#define RLGL_IMPLEMENTATION
#include "rlgl.h"               // rlgl library: OpenGL 1.1 immediate-mode style coding

#include <GLFW/glfw3.h>         // Windows/Context and inputs management

#include <stdio.h>              // Standard input-output C library
#include <stdlib.h>             // Memory management functions: malloc(), free()
#include <string.h>             // String manipulation functions: strrchr(), strcmp()

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#define WHITE   (Color){ 255, 255, 255, 255 }       // White color definition

//----------------------------------------------------------------------------------
// Global Variables Declaration
//----------------------------------------------------------------------------------

// LESSON 01: Window and graphic device initialization and management
GLFWwindow *window;

// Timming required variables
static double currentTime, previousTime;    // Used to track timmings
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied

// LESSON 03: Keyboard input management
// Register keyboard states (current and previous)
static char previousKeyState[512] = { 0 };  // Registers previous frame key state
static char currentKeyState[512] = { 0 };   // Registers current frame key state

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

// LESSON 02: Window and graphic device initialization and management
//----------------------------------------------------------------------------------
static void ErrorCallback(int error, const char* description);                              // GLFW3: Error callback function
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);   // GLFW3: Keyboard callback function

static void InitWindow(int width, int height);          // Initialize window and context
static void InitGraphicsDevice(int width, int height);  // Initialize graphic device
static void CloseWindow(void);                          // Close window and free resources
static void SetTargetFPS(int fps);                      // Set target FPS (maximum)
static void SyncFrame(void);                            // Synchronize to desired framerate

// LESSON 03: Inputs management (keyboard and mouse)
//----------------------------------------------------------------------------------
static bool IsKeyDown(int key);                     // Detect if a key is being pressed (key held down)
static bool IsKeyPressed(int key);                  // Detect if a key has been pressed once
static void PollInputEvents(void);                  // Poll (store) all input events

// LESSON 04: Basic shapes drawing
//----------------------------------------------------------------------------------
static void DrawLine(Vector2 startPos, Vector2 endPos, Color color);                // Draw a line
static void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);          // Draw a triangle
static void DrawRectangle(int posX, int posY, int width, int height, Color color);  // Draw a rectangle
static void DrawRectangleRec(Rectangle rec, Color color);                           // Draw a rectangle

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    // LESSON 01: Window and graphic device initialization and management
    InitWindow(screenWidth, screenHeight);          // Initialize Window using GLFW3
    
    InitGraphicsDevice(screenWidth, screenHeight);  // Initialize graphic device (OpenGL)
    
    // Init player position
    Rectangle player = { 100, 100, 32, 32 };
    Rectangle oldPlayer = player;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------    

    // Main game loop    
    while (!glfwWindowShouldClose(window))
    {
        // Update
        //----------------------------------------------------------------------------------
        // Player movement logic
        oldPlayer = player;
        
        if (IsKeyDown(GLFW_KEY_DOWN)) player.y += 2;
        else if (IsKeyDown(GLFW_KEY_UP)) player.y -= 2;
        
        if (IsKeyDown(GLFW_KEY_RIGHT)) player.x += 2;
        else if (IsKeyDown(GLFW_KEY_LEFT)) player.x -= 2;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        rlClearScreenBuffers();             // Clear current framebuffer

        DrawRectangle(200, 100, 400, 250, (Color){ 0, 255, 0, 255 });
        DrawRectangleRec(player, (Color){ 255, 0, 0, 255 });
        
        rlglDraw();                         // Internal buffers drawing (2D data)

        glfwSwapBuffers(window);            // Swap buffers: show back buffer into front
        PollInputEvents();                  // Register input events (keyboard, mouse)
        SyncFrame();                        // Wait required time to target framerate
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    rlglClose();                    // Unload rlgl internal buffers and default shader/texture
    
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definitions
//----------------------------------------------------------------------------------

// GLFW3: Error callback
static void ErrorCallback(int error, const char* description)
{
    TraceLog(LOG_ERROR, description);
}

// GLFW3: Keyboard callback
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else currentKeyState[key] = action;
}

// LESSON 01: Window creation and management
//----------------------------------------------------------------------------------
// Initialize window and context (OpenGL 3.3)
static void InitWindow(int screenWidth, int screenHeight)
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
   
    window = glfwCreateWindow(screenWidth, screenHeight, "CHALLENGE 02: 2D DUNGEON GAME", NULL, NULL);
    
    if (!window) glfwTerminate();
    else TraceLog(LOG_INFO, "GLFW3: Window created successfully");
    
    glfwSetWindowPos(window, 200, 200);
    
    glfwSetKeyCallback(window, KeyCallback);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
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

// LESSON 02: Graphic device initialization and management
//----------------------------------------------------------------------------------
// Initialize graphic device (OpenGL 3.3)
static void InitGraphicsDevice(int width, int height)
{
    // Load OpenGL 3.3 supported extensions
    rlLoadExtensions(glfwGetProcAddress);

    // Initialize OpenGL context (states and resources)
    rlglInit(width, height);

    // Initialize viewport and internal projection/modelview matrices
    rlViewport(0, 0, width, height);
    rlMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
    rlLoadIdentity();                                   // Reset current matrix (PROJECTION)
    rlOrtho(0, width, height, 0, 0.0f, 1.0f);           // Orthographic projection with top-left corner at (0,0)
    rlMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
    rlLoadIdentity();                                   // Reset current matrix (MODELVIEW)

    rlClearColor(0, 0, 0, 255);                         // Define clear color (BLACK)
    rlEnableDepthTest();                                // Enable DEPTH_TEST for 3D
}

// LESSON 03: Inputs management (keyboard and mouse)
//----------------------------------------------------------------------------------
// Detect if a key is being pressed (key held down)
static bool IsKeyDown(int key)
{
    return glfwGetKey(window, key);
}

// Detect if a key has been pressed once
static bool IsKeyPressed(int key)
{
    if ((currentKeyState[key] != previousKeyState[key]) && (currentKeyState[key] == 1)) return true;
    else return false;
}

// Poll (store) all input events
static void PollInputEvents(void)
{
    // Register previous keys states (required to check variations)
    for (int i = 0; i < 512; i++) previousKeyState[i] = currentKeyState[i];

    // Input events polling (managed by GLFW3 through callback)
    glfwPollEvents();
}

// LESSON 04: Basic shapes drawing
//----------------------------------------------------------------------------------
// Draw a line
static void DrawLine(Vector2 startPos, Vector2 endPos, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(startPos.x, startPos.y);
        rlVertex2f(endPos.x, endPos.y);
    rlEnd();
}

// Draw a triangle
static void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    rlEnableTexture(GetTextureDefault().id); // Default white texture

    rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(v1.x, v1.y);
        rlVertex2f(v2.x, v2.y);
        rlVertex2f(v2.x, v2.y);
        rlVertex2f(v3.x, v3.y);
    rlEnd();
    
    rlDisableTexture();
}

// Draw color-filled rectangle
static void DrawRectangle(int posX, int posY, int width, int height, Color color)
{
#define TRIS_RECTANGLE
#if defined(TRIS_RECTANGLE)
    // NOTE: We use rlgl OpenGL 1.1 style vertex definition
    rlBegin(RL_TRIANGLES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2i(posX, posY);
        rlVertex2i(posX, posY + height);
        rlVertex2i(posX + width, posY + height);

        rlVertex2i(posX, posY);
        rlVertex2i(posX + width, posY + height);
        rlVertex2i(posX + width, posY);
    rlEnd();
#else   // QUADS_RECTANGLE
    // NOTE: Alternative implementation using RL_QUADS
    rlEnableTexture(GetDefaultTexture().id);    // Default white texture
    rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlNormal3f(0.0f, 0.0f, 1.0f);

        rlTexCoord2f(0.0f, 0.0f);
        rlVertex2f(position.x, position.y);

        rlTexCoord2f(0.0f, 1.0f);
        rlVertex2f(position.x, position.y + size.y);

        rlTexCoord2f(1.0f, 1.0f);
        rlVertex2f(position.x + size.x, position.y + size.y);

        rlTexCoord2f(1.0f, 0.0f);
        rlVertex2f(position.x + size.x, position.y);
    rlEnd();
    rlDisableTexture();
#endif
}

// Draw a color-filled rectangle
static void DrawRectangleRec(Rectangle rec, Color color)
{
    DrawRectangle(rec.x, rec.y, rec.width, rec.height, color);
}
