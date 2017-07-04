/*******************************************************************************************
*
*   Challenge 03:   MAZE GAME
*   Lesson 01:      OpenGL intro
*   Description:    Introduction to OpenGL 3.3 Core profile
*
*   NOTE: This example requires the following header-only files:
*       glad.h      - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h   - Vector and matrix math functions
*       stb_image.h - Multiple formats image loading (BMP, PNG, TGA, JPG...)
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99
*
*   This example has been created using raylib 1.7 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <GLFW/glfw3.h>         // Windows/Context and inputs management

#define GLAD_IMPLEMENTATION
#include "glad.h"               // GLAD extensions loading library
                                // NOTE: Includes required OpenGL headers

#define RAYMATH_IMPLEMENTATION
#include "raymath.h"            // Required for: Vector3 and Matrix functions

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    bool windowShouldClose = true;
    
    // TODO: GLFW3 Initialization + OpenGL 3.3 Context + Extensions

    // Main game loop    
    while (!windowShouldClose) 
    {
        // Update
        //----------------------------------------------------------------------------------
        // ...
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        // Draw everything
        // Swap screen buffers
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // TODO: Unload rlgl internal buffers and default shader/texture
    // TODO: Close window
    //--------------------------------------------------------------------------------------
    
    return 0;
}