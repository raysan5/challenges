/*******************************************************************************************
*
*   Challenge 02: DUNGEON GAME - window creation and management
*
*   NOTE: This example requires OpenGL 3.3 or ES2 versions for shaders support,
*         OpenGL 1.1 does not support shaders but it can also be used.
*
*   Compile rlgl module using:
*   gcc -c rlgl.c -Wall -std=c99 -DRLGL_STANDALONE -DRAYMATH_IMPLEMENTATION -DGRAPHICS_API_OPENGL_33
*
*   NOTE: rlgl module requires the following header-only files:
*       external/glad.h - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h - Vector and matrix math functions
*
*   Compile example using:
*   gcc -o rlgl_standalone.exe rlgl_standalone.c rlgl.o -lglfw3 -lopengl32 -lgdi32 -std=c99
*
*   This example has been created using raylib 1.7 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <GLFW/glfw3.h>         // Windows/Context and inputs management

#define RLGL_STANDALONE
#include "rlgl.h"               // rlgl library: OpenGL 1.1 immediate-mode style coding

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

// GLFW3: Error callback function to be registered
static void ErrorCallback(int error, const char* description);

static void InitWindow(int screenWidth, int screenHeight);
static void CloseWindow(void);

// Drawing functions (uses rlgl functionality)
// NOTE: Vector2 and Color structures are defined in rlgl.h
static void DrawRectangleV(Vector2 position, Vector2 size, Color color);

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight);      // Initialize Window using GLFW3
    
    rlglLoadExtensions(glfwGetProcAddress);     // Load OpenGL 3.3 supported extensions
    
    rlglInit(screenWidth, screenHeight);        // Initialize OpenGL context (states and resources)
    
    //InitGraphicDevice();

    // Initialize viewport and internal projection/modelview matrices
    rlViewport(0, 0, screenWidth, screenHeight);
    rlMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
    rlLoadIdentity();                                   // Reset current matrix (PROJECTION)
    rlOrtho(0, screenWidth, screenHeight, 0, 0.0f, 1.0f); // Orthographic projection with top-left corner at (0,0)
    rlMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
    rlLoadIdentity();                                   // Reset current matrix (MODELVIEW)

    rlClearColor(245, 245, 245, 255);                   // Define clear color
    rlEnableDepthTest();                                // Enable DEPTH_TEST for 3D
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
        rlClearScreenBuffers();             // Clear current framebuffer
        
            //DrawRectangleV((Vector2){ 10.0f, 10.0f }, (Vector2){ 780.0f, 20.0f }, DARKGRAY);

            rlglDraw();     // NOTE: Internal buffers drawing (2D data)
            
        glfwSwapBuffers(window);
        glfwPollEvents();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    rlglClose();                    // Unload rlgl internal buffers and default shader/texture
    
    glfwDestroyWindow(window);      // Close window
    glfwTerminate();                // Free GLFW3 resources
    //--------------------------------------------------------------------------------------
    
    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definitions
//----------------------------------------------------------------------------------

// GLFW3: Error callback
static void ErrorCallback(int error, const char* description)
{
    TraceLog(ERROR, description);
}

static void InitWindow(int screenWidth, int screenHeight)
{
    // GLFW3 Initialization + OpenGL 3.3 Context + Extensions
    //--------------------------------------------------------
    glfwSetErrorCallback(ErrorCallback);
    
    if (!glfwInit())
    {
        TraceLog(WARNING, "GLFW3: Can not initialize GLFW");
        return 1;
    }
    else TraceLog(INFO, "GLFW3: GLFW initialized successfully");
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
   
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "rlgl standalone", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return 2;
    }
    else TraceLog(INFO, "GLFW3: Window created successfully");
    
    glfwSetWindowPos(window, 200, 200);
    
    glfwSetKeyCallback(window, KeyCallback);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    //--------------------------------------------------------
}

// Draw rectangle using rlgl OpenGL 1.1 style coding (translated to OpenGL 3.3 internally)
static void DrawRectangleV(Vector2 position, Vector2 size, Color color)
{
    rlBegin(RL_TRIANGLES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2i(position.x, position.y);
        rlVertex2i(position.x, position.y + size.y);
        rlVertex2i(position.x + size.x, position.y + size.y);

        rlVertex2i(position.x, position.y);
        rlVertex2i(position.x + size.x, position.y + size.y);
        rlVertex2i(position.x + size.x, position.y);
    rlEnd();
}