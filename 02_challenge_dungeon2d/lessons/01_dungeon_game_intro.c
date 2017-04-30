/*******************************************************************************************
*
*   Challenge 02: DUNGEON GAME - rlgl module intro
*
*   NOTE: This example requires OpenGL 3.3 or ES2 for shaders support,
*         OpenGL 1.1 does not support shaders but it can also be used.
*
*   Compile rlgl module using:
*       gcc -c rlgl.c -Wall -std=c99 -DRLGL_STANDALONE -DRAYMATH_IMPLEMENTATION -DGRAPHICS_API_OPENGL_33
*
*   NOTE: rlgl module requires the following header-only files:
*       glad.h    - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h - Vector and matrix math functions
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) rlgl.o -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99
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