
# Challenge 02: 2D Dungeon game

*by Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*

## Introduction
In this challenge we will implement a 2D Dungeons game similar to the dungeons of [The Legend of Zelda](https://en.wikipedia.org/wiki/The_Legend_of_Zelda) classic game (Nintendo, 1986). Along this process we will learn how to manage game tiles and construct levels with them, how to manage the window and player inputs from a lower level perspective (in comparison to previous challenge), how to load image data from files and convert it to textures in GPU, how to draw basic shapes and textures and, finally, some basic 2D graphics animations (spritesheet based).

This game is developed using [rlgl](), a [raylib](http://www.raylib.com/) auxiliar module intended to simplify low level GPU access and teach basic principles of graphics programming like vertex buffers usage, textures binding, shaders usage...

Before starting with this challenge, it's recommended to have completed the previous challenge: 
 - [Challenge 01: Blocks game](../01_challenge_blocks) - A blocks game where player has to break a wall of blocks controlling a ball with a paddle.
 
It's assumed that all concepts explained in that challenge are already learn by student.

**Previous knowledge required:**
 - Videogame life cycle (Init -> Update -> Draw -> DeInit)
 - Basic screens management with screens transition
 - Collision detection and resolution
 - Sounds and music loading and playing

**Learning Outcomes:**
 - rlgl functionality and possibilities
 - Window creation, configuration and management (GLFW3)
 - Keyboard and mouse inputs management (GLFW3)
 - Basic shaped drawing defining vertex data (immediate-mode)
 - Image files loading (RAM and VRAM) and drawing
 - Tile map data loading from a text file
 - Sprites based animation
 
**NOTE:** All code provided is in C language for simplicity and clearness but it's up to the student to use more complex C++ code structures (OOP) if desired.
 
## Lessons Summary

Lesson | Learning outcome | Source file | Related functions
:-----:|------------------|:------------|:-----------------:
[01](#lesson-01-introduction-to-rlgl-and-immediate-mode) | rlgl functionality | [01_dungeon_game_intro.c](lessons/01_dungeon_game_intro.c) | -
[02](#lesson-02-windows-creation-and-management) | window creation and management | [02_dungeon_game_window.c](lessons/02_dungeon_game_window.c) | InitWindow(), CloseWindow(), <br>InitGraphicDevice()
[03](#lesson-03-inputs-management-keyboard-mouse) | input management | [03_dungeon_game_inputs.c](lessons/03_dungeon_game_inputs.c) | IsKeyDown(), IsKeyPressed()
[04](#lesson-04-basic-shapes-drawing-with-vertex-data) | basic shapes definition | [04_dungeon_game_shapes.c](lessons/04_dungeon_game_shapes.c) | DrawLine(), DrawTriangle(), DrawRectangle()
[05](#lesson-05-image-files-loading-and-texture-creation) | image data loading, texture creation and drawing | [05_dungeon_game_textures.c](lessosn/05_dungeon_game_textures.c) | LoadImage(), UnloadImage(), <br>LoadTexture(), UnloadTexture(), LoadBMP()
[06](#lesson-06-tilemap-data-loading-and-usage) | tilemap data loading | [06_dungeon_game_tiles.c](lessons/06_dungeon_game_tiles.c) | LoadTilemap(), UnloadTileMap()
[07](#lesson-07-sprites-based-animation) | sprites animation | [07_dungeon_game_sprites.c](lessons/07_dungeon_game_sprites.c) | LoadSprite(), UnloadSprite(), UpdateSprite(), DrawSprite()

**NOTE:** Most of the documentation for the challenge is directly included in the source code files as code comments, in the form of *TODO* points for every task to be completed. Read carefully those comments to understand every task and how implement the proposed solutions.

### Lesson 01: Introduction to rlgl and immediate mode graphics

*Lesson code file to review: [01_dungeon_game_intro.c](lessons/01_dungeon_game_intro.c)*

In this first lesson we will introduce rlgl module library and the underlying layer OpenGL. We will setup a window and graphic device to be able to transfer data and control the GPU.

https://github.com/raysan5/raylib/wiki/OpenGL-Graphic-API

### Lesson 02: Windows creation and management using GLFW3

*Lesson code file to review: [02_dungeon_game_window.c](lessons/02_dungeon_game_window.c)*

Initialize window and graphic device context. Using the auxiliar libraries GLFW3 (window, input management) and GLAD (extensions loading).

Functions to be implemented:
```c
void InitWindow(int screenWidth, int screenHeight);         // Initialize window using GLFW3
void CloseWindow(void);                                     // Close window
void InitGraphicsDevice(int screenWidth, int screenHeight); // Initialize graphics device context
```

### Lesson 03: Inputs management (keyboard-mouse-gamepad) with GLFW3

*Lesson code file to review: [03_dungeon_game_inputs.c](lessons/03_dungeon_game_inputs.c)*

Read user inputs (keyboard-mouse) using GLFW3 library.

Functions to be implemented:
```c
bool IsKeyPressed(int key);                             // Detect if a key has been pressed once
bool IsKeyDown(int key);                                // Detect if a key is being pressed
int GetKeyPressed(void);                                // Get latest key pressed
bool IsMouseButtonPressed(int button);                  // Detect if a mouse button has been pressed once
bool IsMouseButtonDown(int button);                     // Detect if a mouse button is being pressed
Vector2 GetMousePosition(void);                         // Returns mouse position XY
```

### Lesson 04: Basic shapes creation and drawing

*Lesson code file to review: [04_dungeon_game_shapes.c](lessons/04_dungeon_game_shapes.c)*

Define basic shapes by vertex using immediate mode.

Functions to be implemented:
```c
void DrawLineV(Vector2 startPos, Vector2 endPos, Color color);
void DrawCircleV(Vector2 center, float radius, Color color);
void DrawRectangle(int posX, int posY, int width, int height, Color color);
```

### Lesson 05: Image files loading and texture creation

*Lesson code file to review: [05_dungeon_game_textures.c](lessosn/05_dungeon_game_textures.c)*

Load image data from a BMP file, decodifying information if required, convert that image data to a texture (GPU uploading) and leran to draw that texture on the canvas.

Functions to be implemented:
```c
Image LoadImage(const char *fileName);
void UnloadImage(Image image);
Texture2D LoadTextureFromImage(Image image);
void UnloadTexture(Texture2D texture);
void DrawTexture(Texture2D texture, int posX, int posY, Color tint);
```

### Lesson 06: Tilemap data loading and usage

*Lesson code file to review: [06_dungeon_game_tiles.c](lessons/06_dungeon_game_tiles.c)*

Load tilemap data from different file sources, from a simple text file to an image pixel data.

Functions to be implemented:
```c
TileMap LoadTileMap(const char *fileName);
void UnloadTileMap(TileMap map);
void DrawTileMap(TileMap);
```

### Lesson 07: Spritesheet based animations

*Lesson code file to review: [07_dungeon_game_sprites.c](lessons/07_dungeon_game_sprites.c)*

Functions to be implemented:
```c
Sprite LoadSprite(const char *fileName);
void UnloadSprite(Sprite sprite);
void UpdateSprite(Sprite *sprite);
void DrawSprite(Sprite sprite);
```

## Getting help 
We strongly encourage you using the online [raylib forum](forum.raylib.com) to discuss challenges with other students. However, we recommend not to look at any source code written by other students or share your source code with others **while working on the challenge**.

## License
All code provided for this challenge is licensed under MIT License. Check [LICENSE](../LICENSE) for further details.

*Copyright (c) 2017 Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*
