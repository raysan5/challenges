/*******************************************************************************************
*
*   Challenge 02:   DUNGEON GAME
*   Lesson 01:      window creation
*   Description:    window creation and management
*
*   NOTE 1: This example requires OpenGL 3.3 or ES2 for shaders support,
*       OpenGL 1.1 does not support shaders but it can also be used.
*
*   NOTE 2: This example requires the following single-file header-only modules:
*       rlgl.h    - OpenGL abstraction layer for OpenGL 1.1 immediate-mode style coding
*       glad.h    - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h - Vector and matrix math functions
*
*   NOTE 3: This example requires GLFW library that is first compiled rglfw.o module and
*       later is linked on example compilation.
*
*   Compile rglfw module using:
*       gcc -c external/rglfw.c -Wall -std=c99 -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 \
*           -Iexternal/glfw/include -Iexternal/glfw/deps/mingw
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -Iexternal -Iexternal/glfw/include \
*           rglfw.o -lopengl32 -lgdi32 -Wall -std=c99
*
*   Copyright (c) 2017-2019 Ramon Santamaria (@raysan5)
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
//...

//----------------------------------------------------------------------------------
// Global Variables Declaration
//----------------------------------------------------------------------------------

// LESSON 01: Window and graphic device initialization and management
GLFWwindow *window;

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

// LESSON 01: Window and graphic device initialization and management
//----------------------------------------------------------------------------------
static void ErrorCallback(int error, const char* description);                              // GLFW3: Error callback function
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);   // GLFW3: Keyboard callback function

static void InitWindow(int width, int height);          // Initialize window and context
static void CloseWindow(void);                          // Close window and free resources

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
    
    // Load OpenGL 3.3 supported extensions
    rlLoadExtensions(glfwGetProcAddress);
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
        
        glfwSwapBuffers(window);            // Swap buffers: show back buffer into front
        glfwPollEvents();                   // Register input events (keyboard, mouse)
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
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