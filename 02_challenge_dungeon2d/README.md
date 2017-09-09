
# Challenge 02: 2D Dungeon game

*by Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*

![dungeon game](dungeon_game.gif "Dungeon Game")

## Introduction
In this challenge we will implement a 2D Dungeons game similar to the dungeons of [The Legend of Zelda](https://en.wikipedia.org/wiki/The_Legend_of_Zelda) classic game (Nintendo, 1986). Along this process we will learn how to manage game tiles and construct levels with them, how to manage the window and player inputs from a lower level perspective (in comparison to previous challenge), how to load image data from files and convert it to textures in GPU, how to draw basic shapes and textures and, finally, some basic 2D graphics animations (spritesheet based).

This game is developed using [rlgl](), a [raylib](http://www.raylib.com/) auxiliar module intended to simplify low level GPU access and teach basic principles of graphics programming like vertex buffers usage, textures binding, shaders usage...

Before starting with this challenge, it's recommended to complete the previous challenge: 
 - [Challenge 01: Blocks game](../01_challenge_blocks) - A blocks game where player has to break a wall of blocks controlling a ball with a paddle.
 
It's assumed that all concepts explained in that challenge have already been learnt by student.

**Previous knowledge required:**
 - Videogame life cycle (Init -> Update -> Draw -> DeInit)
 - Basic screens management with screens transition
 - Collision detection and resolution
 - Sounds and music loading and playing

**Learning Outcomes:**
 - rlgl functionality and possibilities
 - Window creation, configuration and management (GLFW3)
 - Context creation (OpenGL 3.3) and extensions loading (GLAD)
 - Inputs management (keyboard, mouse) (GLFW3)
 - Basic shaped drawing defining vertex data (immediate-mode)
 - Image loading (RAM), texture creation (VRAM) and drawing
 - Tile map data loading from text file
 - Sprites based animation
 
**NOTE:** All code provided is in C language for simplicity and clearness but it's up to the student to use more complex C++ code structures (OOP) if desired.
 
## Lessons Summary

Lesson | Learning outcome | Source file | Related functions
:-----:|------------------|:------------|:-----------------:
[01](#lesson-01-window-creation-and-management) | window creation and management | [01_dungeon_game_window.c](lessons/01_dungeon_game_window.c) | InitWindow(), <br>CloseWindow()
[02](#lesson-02-graphics-device-initialization) | context initialization, <br>extensions loading | [02_dungeon_game_graphics.c](lessons/02_dungeon_game_graphics.c) | InitGraphicsDevice()
[03](#lesson-03-inputs-management) | inputs management (keyboard) | [03_dungeon_game_inputs.c](lessons/03_dungeon_game_inputs.c) | IsKeyDown(), IsKeyPressed()
[04](#lesson-04-basic-shapes-definition-and-drawing) | basic shapes definition | [04_dungeon_game_shapes.c](lessons/04_dungeon_game_shapes.c) | DrawLine(), DrawTriangle(), DrawRectangle()
[05](#lesson-05-image-loading-and-texture-creation) | image data loading, <br>texture creation and drawing | [05_dungeon_game_textures.c](lessosn/05_dungeon_game_textures.c) | LoadImage(), UnloadImage(), <br>LoadTexture(), UnloadTexture(), LoadBMP()
[06](#lesson-06-tilemap-data-loading) | tilemap data loading | [06_dungeon_game_tilemap.c](lessons/06_dungeon_game_tilemap.c) | LoadTilemap(), UnloadTileMap()
[07](#lesson-07-collision-detection) | tilemap collision detection | [07_dungeon_game_collision.c](lessons/07_dungeon_game_collision.c) | CheckCollisionTilemap()

**NOTE:** Most of the documentation for the challenge is directly included in the source code files as code comments, in the form of *TODO* points for every task to be completed. Read carefully those comments to understand every task and how implement the proposed solutions.

### Lesson 01: Window creation and management

*Lesson code file to review: [01_dungeon_game_intro.c](lessons/01_dungeon_game_window.c)*

In this first lesson we will setup a window and see how to manage it using auxiliar library GLFW3:

  - [GLFW3](http://www.glfw.org/docs/latest/) ([source](https://github.com/glfw/glfw)) - Multiplatform library to manage window, graphic context and inputs.

**Window creation**

To place our graphic device (understand it as a drawing canvas), we need a window (understand it as the frame for the canvas); but that window can change from system to system. In Windows OS, that window is managed by the underlying system libraries (usually [GDI](https://en.wikipedia.org/wiki/Graphics_Device_Interface)) while in Linux is managed by the underlying [X11](https://en.wikipedia.org/wiki/X_Window_System) system; additionally, that window frame should match the graphic device (drawing canvas) attached to it. To make sure we create the correct frame with the correct canvas (for the current OS) we will use GLFW3 library that simplyfies that task.

Functions to be implemented:
```c
void InitWindow(int screenWidth, int screenHeight);         // Initialize window using GLFW3
void CloseWindow(void);                                     // Close window
```

### Lesson 02: Graphics Device initialization

*Lesson code file to review: [02_dungeon_game_window.c](lessons/02_dungeon_game_graphics.c)*

In this lesson we will learn how to initialize the graphic device context to be able to access and control the GPU. We will use two great auxiliar libraries: 

  - [rlgl](https://github.com/raysan5/raylib/blob/develop/src/rlgl.h) - Simple library to manage graphic device and the underlying OpenGL layer.
  - [glad](https://github.com/Dav1dde/glad) -  Library to manage OpenGL extensions loading.
  
rlgl is a very thin layer (wrapper) over OpenGL that simplyfies its usage to a immediate-mode programming style, it means, just defining vertex in a very direct mode to draw elements on the screen. OpenGL 1.1 just worked that way and it was very intuitive to the user but since OpenGL 2.1 that working mode became deprecated and replaced by a more complex (and efficient) way of working, using shaders. rlgl allows programming in an immediate mode style over any OpenGL version, it just takes care internally of vertex buffers filling and setting things up simplifying graphics programming to the user without losing the power of newer OpenGL versions.

More details on the utility of rlgl intermediate layer can be found [here](https://github.com/raysan5/raylib/wiki/OpenGL-Graphic-API).

**Graphic Device initialization**

Once the window is created with the correct configuration for the desired graphic device context (in our case, OpenGL 3.3 Core profile), we need to initialize any required OpenGL extensión and initialize some context configuration parameters.

Functions to be implemented:
```c
void InitGraphicDevice(int screenWidth, int screenHeight);  // Initialize graphic device using rlgl
```

### Lesson 03: Inputs management

*Lesson code file to review: [03_dungeon_game_inputs.c](lessons/03_dungeon_game_inputs.c)*

We will need to read user inputs from keyboard, to do that we will also use GLFW3 library, to abstract our code from multiple platforms. In GLFW3 inputs come as events polled at a regular basis (usually every frame) and can be read in callback functions. Basically, we can detect a input state (key) at a specific moment and we will use that information to implement a series of useful functions.

Functions to be implemented:
```c
bool IsKeyPressed(int key);                             // Detect if a key has been pressed once
bool IsKeyDown(int key);                                // Detect if a key is being pressed
```

### Lesson 04: Basic shapes definition and drawing

*Lesson code file to review: [04_dungeon_game_shapes.c](lessons/04_dungeon_game_shapes.c)*

To draw basic shapes using rlgl, we can just define them as a series of vertices attributes (position, texture coordinates, colors...). As explained in Lesson 01, we will use a immediate mode (original from OpenGL 1.1) to do that.

Functions to be implemented:
```c
void DrawLine(Vector2 startPos, Vector2 endPos, Color color);               // Draw a line between two points
void DrawCircle(Vector2 center, float radius, Color color);                 // Draw a filled circle
void DrawRectangle(int posX, int posY, int width, int height, Color color); // Draw a filled rectangle
```

### Lesson 05: Image loading and texture creation

*Lesson code file to review: [05_dungeon_game_textures.c](lessosn/05_dungeon_game_textures.c)*

To draw textures on our canvas, first we need to understand load some image data from an image file (probably decompressing and decodyfing read data) to obtain an array of pixels; after that, image data that is placed in RAM memory should be uploaded to VRAM memory (also referred as GPU memory) and configured with some additional display parameters, this is called **a texture**. Once image is loaded and converted to texture, it's ready to be drawn.

Some important concepts to remember:
 1. Image data is loaded from an image file and is stored in RAM memory. That data is usually compressed and/or codyfied in the image file and should be expanded to a simple array of pixels.
 2. Following the above declaration, note that any image file (.bmp, .jpg, .tga, .png...) will presumably have the same size once loaded into RAM, independently of the disk size of that compressed and/or codyfied data.
 3. To convert that image data into a texture, we upload pixels data to VRAM... and we set a series of display configuration parameters for that texture.
 4. Once image data is converted to a texture, we usually don't need that data in RAM memory any more.

Functions to be implemented:
```c
Image LoadImage(const char *fileName);                   // Load image data from file (RAM)
void UnloadImage(Image image);                           // Unload image data from RAM
Texture2D LoadTextureFromImage(Image image);             // Load texture from image data (VRAM)
void UnloadTexture(Texture2D texture);                   // Unload texture from VRAM

void DrawTexture(Texture2D texture, Vector2 position, Color tint); // Draw texture in screen position coordinates
```

### Lesson 06: Tilemap data loading

*Lesson code file to review: [06_dungeon_game_tiles.c](lessons/06_dungeon_game_tiles.c)*

*TODO: Explain tiles theory?*

In this lesson we will learn how to load tilemap data from a simple text file and use a tileset to draw our level based on that tilemap data. We will complete the lesson adding extra information for every tile (collision information) and multiple tile-based layers to our level.

Functions to be implemented:
```c
Tilemap LoadTilemap(const char *fileName);         // Load tilemap data from file (RAM)
void UnloadTilemap(Tilemap map);                   // Unload tilemap data from RAM

void DrawTilemap(Tilemap map, Texture2D tileset, Vector2 position, float tileSize);  // Draw tilemap using tileset texture
```

### Lesson 07: Collision detection

*Lesson code file to review: [07_dungeon_game_collision.c](lessons/07_dungeon_game_collision.c)*

We will check tilemap collisions, to avoid player moving through blocked tiles.

Functions to be implemented:
```c
bool CheckCollisionTilemap(Tilemap map, Vector2 playerPos);    // Check tilemap collision
```

## Getting help 
We strongly encourage you using the online [raylib forum](forum.raylib.com) to discuss challenges with other students. However, we recommend not to look at any source code written by other students or share your source code with others **while working on the challenge**.

## License
This lecture is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.

Challenge code is licensed under an unmodified zlib/libpng license.

Check [LICENSE](../LICENSE) for further details.

*Copyright (c) 2017 Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*
