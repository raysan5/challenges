/*******************************************************************************************
*
*   Challenge 01:   BLOCKS GAME
*   Lesson 01:      raylib intro
*   Description:    Introduction to raylib and the basic videogames life cycle
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -lraylib -lopengl32 -lgdi32 -Wall -std=c99
*
*   This example has been created using raylib 2.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017-2018 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// LESSON 01: Window initialization and screens management
typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, ENDING } GameScreen;

// TODO: Define required structs

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    // LESSON 01: Window initialization and screens management
    InitWindow(screenWidth, screenHeight, "CHALLENGE 01: BLOCKS GAME");
    
    // NOTE: Load resources (textures, fonts, audio) after Window initialization

    // Game required variables
    GameScreen screen = LOGO;       // Current game screen state
    
    int framesCounter = 0;          // General pourpose frames counter
    int gameResult = -1;            // Game result: 0 - Loose, 1 - Win, -1 - Not defined
    bool gamePaused = false;        // Game paused state toggle
    
    // TODO: Define and Initialize game variables
        
    SetTargetFPS(60);               // Set desired framerate (frames per second)
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch(screen) 
        {
            case LOGO: 
            {
                // Update LOGO screen data here!
                
                framesCounter++;
                
                if (framesCounter > 180) 
                {
                    screen = TITLE;    // Change to TITLE screen after 3 seconds
                    framesCounter = 0;
                }
                
            } break;
            case TITLE: 
            {
                // Update TITLE screen data here!
                
                framesCounter++;
                
                // LESSON 03: Inputs management (keyboard, mouse)
                if (IsKeyPressed(KEY_ENTER)) screen = GAMEPLAY;
                
            } break;
            case GAMEPLAY:
            { 
                // Update GAMEPLAY screen data here!

                if (!gamePaused)
                {
                    // TODO: Gameplay logic
                }

            } break;
            case ENDING: 
            {
                // Update END screen data here!
                
                framesCounter++;
                
                // LESSON 03: Inputs management (keyboard, mouse)
                if (IsKeyPressed(KEY_ENTER)) screen = TITLE;

            } break;
            default: break;
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            switch(screen) 
            {
                case LOGO: 
                {
                    // Draw LOGO screen here!
                    
                    DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);

                } break;
                case TITLE: 
                {
                    // Draw TITLE screen here!
                    
                    DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);

                } break;
                case GAMEPLAY:
                { 
                    // Draw GAMEPLAY screen here!
                    
                    DrawText("GAMEPLAY SCREEN", 20, 20, 40, MAROON);
                    
                } break;
                case ENDING: 
                {
                    // Draw END screen here!
                    
                    DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                    
                } break;
                default: break;
            }
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    // NOTE: Unload any loaded resources (texture, fonts, audio)

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}