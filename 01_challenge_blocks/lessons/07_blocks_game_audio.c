/*******************************************************************************************
*
*   Challenge 01:   BLOCKS GAME
*   Lesson 07:      audio
*   Description:    Sounds and music loading and playing
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -lraylib -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99
*
*   This example has been created using raylib 1.7 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include <math.h>               // Basic math functionality: fabs()

//----------------------------------------------------------------------------------
// Useful values definitions 
//----------------------------------------------------------------------------------
#define PLAYER_LIFES             5
#define BRICKS_LINES             5
#define BRICKS_PER_LINE         20

#define BRICKS_POSITION_Y       50

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, ENDING } GameScreen;

// Player structure
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    Rectangle bounds;
    int lifes;
} Player;

// Ball structure
typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    int radius;
    bool active;
} Ball;

// Bricks structure
typedef struct Brick {
    Vector2 position;
    Vector2 size;
    Rectangle bounds;
    int resistance;
    bool active;
} Brick;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    // LESSON 05: Image struct
    InitWindow(screenWidth, screenHeight, "CHALLENGE 01: BLOCKS GAME");
    
    // NOTE: If using textures, declare Texture2D variables here (after InitWindow)
    // NOTE: If using SpriteFonts, declare SpriteFont variables here (after InitWindow)
    
    // NOTE: If using sound or music, InitAudioDevice() and load Sound variables here (after InitAudioDevice)
    
    GameScreen screen = GAMEPLAY;   // Current game screen state
    
    // TODO: Define required variables here
    
    int framesCounter;              // General pourpose frames counter
    int gameResult = -1;            // Game result: 0 - Loose, 1 - Win, -1 - Not defined
    bool gamePaused = false;        // Game paused state toggle
    
    Player player;
    Ball ball;
    Brick bricks[BRICKS_LINES][BRICKS_PER_LINE];
    
    // Initialize player
    player.position = (Vector2){ screenWidth/2, screenHeight*7/8 };
    player.speed = (Vector2){ 8.0f, 0.0f };
    player.size = (Vector2){ screenWidth/10, 20 };
    player.lifes = PLAYER_LIFES;
    
    // Initialize ball
    ball.radius = 7.0f;
    ball.active = false;
    ball.position = (Vector2){ player.position.x + player.size.x/2, player.position.y - ball.radius - 1 };
    ball.speed = (Vector2){ 4.0f, 4.0f };

    // Initialize bricks
    for (int j = 0; j < BRICKS_LINES; j++)
    {
        for (int i = 0; i < BRICKS_PER_LINE; i++)
        {
            bricks[j][i].size = (Vector2){ screenWidth/BRICKS_PER_LINE, 20 };
            bricks[j][i].position = (Vector2){ i*bricks[j][i].size.x, j*bricks[j][i].size.y + BRICKS_POSITION_Y };
            bricks[j][i].bounds = (Rectangle){ bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y };
            bricks[j][i].active = true;
        }
    }
    
    // LESSON 05: Textures loading and drawing
    
    // LESSON 06: SpriteFonts loading and text drawing
    
    // LESSSON 07: Sounds and music loading and playing
    
    
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
                
                // TODO: Logo fadeIn and fadeOut logic
                
            } break;
            case TITLE: 
            {
                // Update TITLE screen data here!
                
                // TODO: Title animation logic
                
                // TODO: "PRESS ENTER" logic
                
            } break;
            case GAMEPLAY:
            { 
                // Update GAMEPLAY screen data here!
                
                // LESSON 03: Inputs management (keyboard, mouse)
                if (IsKeyPressed('P')) gamePaused = !gamePaused;    // Pause button logic

                if (!gamePaused)
                {
                    // Player movement logic
                    if (IsKeyDown(KEY_LEFT)) player.position.x -= player.speed.x;
                    if (IsKeyDown(KEY_RIGHT)) player.position.x += player.speed.x;
                    
                    if ((player.position.x) <= 0) player.position.x = 0;
                    if ((player.position.x + player.size.x) >= screenWidth) player.position.x = screenWidth - player.size.x;
                    
                    player.bounds = (Rectangle){ player.position.x, player.position.y, player.size.x, player.size.y };

                    if (ball.active)
                    {
                        // Ball movement logic
                        ball.position.x += ball.speed.x;
                        ball.position.y += ball.speed.y;
                        
                        // Collision logic: ball vs screen-limits
                        if (((ball.position.x + ball.radius) >= screenWidth) || ((ball.position.x - ball.radius) <= 0)) ball.speed.x *= -1;
                        if ((ball.position.y - ball.radius) <= 0) ball.speed.y *= -1;
                        
                        
                        // LESSON 04: Collision detection and resolution
                        
                        // Collision logic: ball vs player
                        if (CheckCollisionCircleRec(ball.position, ball.radius, player.bounds))
                        {
                            ball.speed.y *= -1;
                            ball.speed.x = (ball.position.x - (player.position.x + player.size.x/2))/player.size.x*5.0f;
                        }
                        
                        // TODO: Collision logic: ball vs bricks
                        for (int j = 0; j < BRICKS_LINES; j++)
                        {
                            for (int i = 0; i < BRICKS_PER_LINE; i++)
                            {
                                if (bricks[j][i].active && (CheckCollisionCircleRec(ball.position, ball.radius, bricks[j][i].bounds)))
                                {
                                    bricks[j][i].active = false;
                                    
                                    // TODO: Calculate ball bounce vector after collision

                                    ball.speed.y *= -1;
                                    
                                    break;
                                }
                            }
                        }

                        // Game ending logic
                        if ((ball.position.y + ball.radius) >= screenHeight)
                        {
                            ball.position.x = player.position.x + player.size.x/2;
                            ball.position.y = player.position.y - ball.radius - 1.0f;
                            ball.speed = (Vector2){ 0, 0 };
                            ball.active = false;

                            player.lifes--;
                        }
                        
                        // TODO: Time counter logic
                    }
                    else
                    {
                        // Reset ball position
                        ball.position.x = player.position.x + player.size.x/2;
                        
                        // Activate ball logic
                        if (IsKeyPressed(KEY_SPACE))
                        {
                            ball.active = true;
                            ball.speed = (Vector2){ 0, -5.0f };
                        }
                    }
                }

            } break;
            case ENDING: 
            {
                // Update END screen data here!
                
                // TODO: Replay / Exit game logic
                if (IsKeyPressed(KEY_ENTER))
                {
                    
                }
                
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
                    
                    // TODO: Draw Logo
                    
                } break;
                case TITLE: 
                {
                    // Draw TITLE screen here!
                    
                    // TODO: Draw Title
                    
                    // TODO: Draw "PRESS ENTER" message
                    
                } break;
                case GAMEPLAY:
                { 
                    // Draw GAMEPLAY screen here!
                    
                    // LESSON 02: Draw basic shapes (circle, rectangle)
                    DrawRectangle(player.position.x, player.position.y, player.size.x, player.size.y, BLACK);   // Draw player bar
                    for (int i = 0; i < player.lifes; i++) DrawRectangle(20 + 40*i, screenHeight - 30, 35, 10, LIGHTGRAY); // Draw player lives
                    DrawCircleV(ball.position, ball.radius, MAROON);    // Draw ball
                    
                    // Draw bricks
                    for (int j = 0; j < BRICKS_LINES; j++)
                    {
                        for (int i = 0; i < BRICKS_PER_LINE; i++)
                        {
                            if (bricks[j][i].active)
                            {
                                if ((i + j)%2 == 0) DrawRectangle(bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y, GRAY);
                                else DrawRectangle(bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y, DARKGRAY);
                            }
                        }
                    }
                    
                    // TODO: Draw time counter
                    
                    // TODO: Draw pause message when required
                    if (gamePaused) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
                    
                    // DEBUG
                    DrawRectangleRec(player.bounds, Fade(RED, 0.5f));
                    
                    for (int j = 0; j < BRICKS_LINES; j++)
                    {
                        for (int i = 0; i < BRICKS_PER_LINE; i++)
                        {
                            if (bricks[j][i].active) DrawRectangleRec(bricks[j][i].bounds, Fade(BLUE, 0.5f));
                        }
                    }
                    
                } break;
                case ENDING: 
                {
                    // Draw END screen here!
                    
                    // TODO: Draw ending message (win or loose)

                    DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
                    
                } break;
                default: break;
            }
        
            DrawFPS(10, 10);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    // NOTE: Unload any resources (Texture2D, SpriteFont, Sound...) loaded
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}