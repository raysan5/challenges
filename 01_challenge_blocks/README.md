
# Challenge 01: Blocks game

*by Ramon Santamaria - [@raysan5](https://twitter.com/raysan5)*

## Introduction
In this challenge we will implement a Blocks game similar to the well-known game [Arkanoid](https://en.wikipedia.org/wiki/Arkanoid) (Taito, 1986). Along this process we will learn how the videogames life cycle works, how to manage the window and player inputs, and how to draw some graphics on screen.

**Learning Outcomes:**
 - raylib functionality and possibilities
 - Videogame life cycle (Init -> Update -> Draw -> DeInit)
 - Basic screens management with screens transition
 - Basic shapes drawing (circle, rectangle)
 - Inputs management (keyboard, mouse)
 - Collision detection and resolution
 - Textures loading and drawing
 - Fonts loading and text drawing
 - Sounds loading and playing
 
This game is developed using [raylib](http://www.raylib.com/), a simple and easy-to-use library to learn videogames programming.

## Lessons Summary

Lesson | Learning outcome | Source file | Related functions
:-----:|------------------|:------------|:-----------------:
[01](#lesson-01-introduction-to-raylib-and-videogames-programming) | raylib functionality, <br>videogame lyfe cycle, <br>basic screens management | [01_blocks_game_intro.c](lessons/01_blocks_game_intro.c) | InitWindow(), CloseWindow(), <br>BeginDrawing(), EndDrawing()
[02](#lesson-02-draw-basic-shapes-circle-rectangle) | draw basic shapes (circle, rectangle) | [02_blocks_game_drawing.c](lessons/02_blocks_game_drawing.c) | DrawRectangle(), DrawCircle()
[03](#lesson-03-inputs-management-keyboard-mouse) | inputs management | [03_blocks_game_inputs.c](lessons/03_blocks_game_inputs.c) | IsKeyPressed(), IsKeyDown(), <br>IsMouseButtonPressed(), IsMouseButtonDown(), <br>GetMouseX(), GetMouseY()
[04](#lesson-04-collision-detection-and-resolution) | collision detection and resolution | [04_blocks_game_collisions.c](lessons/04_blocks_game_collisions.c) | CheckCollisionCircleRec(), <br>CheckCollisionRecs(), <br>CheckCollisionCircles()
[05](#lesson-05-textures-loading-and-drawing) | textures loading and drawing | [05_blocks_game_textures.c](lessons/05_blocks_game_textures.c) | LoadTexture(), UnloadTexture(), <br>DrawTexture()
[06](#lesson-06-spritefonts-loading-and-text-drawing) | fonts loading and text drawing | [06_blocks_game_text.c](lessons/06_blocks_game_text.c) | LoadSpriteFont(), UnloadSpriteFont(), <br>DrawText(), DrawTextEx()
[07](#lesson-07-sounds-loading-and-playing) | sounds loading and playing | [07_blocks_game_sounds.c](lessons/07_blocks_game_sounds.c) | InitAudioDevice(), CloseAudioDevice(), LoadSound(), UnloadSound(), PlaySound()

**NOTE:** Most of the documentation for the challenge is directly included in the source code files as code comments, in the form of *TODO* points for every task to be completed. Read carefully those comments to understand every task and how implement the proposed solutions.

### Lesson 01: Introduction to raylib and videogames programming

In this first lesson we will introduce raylib library and videogames programming principles. We will setup raylib and take a look to its functionality; we will see how videogame life cycle works and we will implement a basic screens management system for our game.

**NOTE:** All code provided is in C language for simplicity and clearness but it's up to the student to use more complex C++ code structures (OOP) if desired.

**Introduction to raylib**

raylib is simple and easy-to-use library to learn videogmaes programming. raylib abstracts the user from the more low level libraries like OpenGL, OpenAL or GLFW3. Following challenges focus on those deeper levels.

Here there are some interesting links to know more about raylib:

 - [raylib Features](http://www.raylib.com)
 - [raylib Architecture](https://github.com/raysan5/raylib/wiki/raylib-arquitecture)
 - [raylib Functions Cheatsheet](http://www.raylib.com/cheatsheet/cheatsheet.html)
 - [raylib Data Structures](https://github.com/raysan5/raylib/wiki/raylib-data-structures)
 - [raylib Examples](http://www.raylib.com/examples.html)
 - [raylib Wiki](https://github.com/raysan5/raylib/wiki)

raylib follows the KISS principle, providing simple functions with clear naming conventions. Main documentation for the library functionality is condensed in a single [cheatsheet](http://www.raylib.com/cheatsheet/cheatsheet.html).

raylib is structured in several C files, most of them decoupled from each other and functionality is mostly configurable on library compilation

**Environment setup**

Just [download and install raylib](http://www.raylib.com/). raylib is distributed on Windows as a standalone installer containing all required tools to develop videogames in C/C++. Tools included with installer are preconfigured versions of:

 - MinGW 5.3.0 - GCC C compiler for Windows
 - Notepad++ 7.1 - Windows simple code editor
 
Those are the recommended tools to develop this challenge. In case some students desire to use other tools or works in another environment different than Windows, is up to them to setup the required environment. Some information is provided in raylib Wiki to configure raylib on [OSX](https://github.com/raysan5/raylib/wiki/Compile-for-OSX) and [GNU Linux](https://github.com/raysan5/raylib/wiki/Compile-for-GNU-Linux) platforms.
 
To compile the code, you will be using the Notepad++ script `raylib_compile_execute`.

### Lesson 02: Draw basic shapes (circle, rectangle) 

To draw basic shapes, raylib provides the following functions:
```c
void DrawPixel(int posX, int posY, Color color);
void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);
void DrawCircle(int centerX, int centerY, float radius, Color color);
void DrawCircleLines(int centerX, int centerY, float radius, Color color);
void DrawRectangle(int posX, int posY, int width, int height, Color color);
void DrawRectangleLines(int posX, int posY, int width, int height, Color color);
```
Most of those functions are self explanatory, they must be called in the draw part of the loop, between `BeginDrawing()` and `EndDrawing()`. User needs to provide the drawing position (x, y), size and color. Just note that in case of rectangle-shapes drawing origin is upper-left corner while drawing circle-shapes origin is set in the center of the circle.

Recommended [raylib examples](http://www.raylib.com/examples.html) to check:
 - [shapes_basic_shapes](http://www.raylib.com/examples/web/loader.html?name=shapes_basic_shapes) - basic shapes drawing
 - [shapes_logo_raylib](http://www.raylib.com/examples/web/loader.html?name=shapes_logo_raylib) - raylib logo drawn using shapes
 

### Lesson 03: Inputs management (keyboard, mouse)

To manage inputs, raylib provides a set of functions to detect keyboard and mouse current state:
```c
bool IsKeyPressed(int key);               // Detect if a key has been pressed once
bool IsKeyDown(int key);                  // Detect if a key is being pressed
bool IsKeyReleased(int key);              // Detect if a key has been released once
bool IsKeyUp(int key);                    // Detect if a key is NOT being pressed
    
bool IsMouseButtonPressed(int button);    // Detect if a mouse button has been pressed once
bool IsMouseButtonDown(int button);       // Detect if a mouse button is being pressed
bool IsMouseButtonReleased(int button);   // Detect if a mouse button has been released once
bool IsMouseButtonUp(int button);         // Detect if a mouse button is NOT being pressed
int GetMouseX(void);                      // Returns mouse position X
int GetMouseY(void);                      // Returns mouse position Y
```
...

### Lesson 04: Collision detection and resolution
To check collisions between simple shapes (circle, rectangle), raylib provides the following functions:
```c
bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2);                                    // Check collision between two rectangles
bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2); // Check collision between two circles
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec);                  // Check collision between circle and rectangle
```
...

### Lesson 05: Textures loading and drawing
To load and draw textures, raylib provides the following functions:
```c
Texture2D LoadTexture(const char *fileName);       // Load an image file as texture into GPU memory
void UnloadTexture(Texture2D texture);             // Unload texture from GPU memory

void DrawTexture(Texture2D texture, int posX, int posY, Color tint); // Draw a texture in the canvas
```
...

### Lesson 06: SpriteFonts loading and text drawing
To draw text, raylib loads a default font on `InitWindow()`, that font is used when drawing text with:
```c
void DrawText(const char *text, int posX, int posY, int fontSize, Color color);
```
But raylib users can also load custom fonts, raylib support multiple fonts formats, including TTF format and BMFonts.


### Lesson 07: Sounds loading and playing
To load and play sounds, raylib provides the following functions:
```c
Sound LoadSound(const char *fileName);      // Load sound from file into memory
void UnloadSound(Sound sound);              // Unload sound from memory

void PlaySound(Sound sound);                // Play a sound
void PauseSound(Sound sound);               // Pause a sound
void ResumeSound(Sound sound);              // Resume a paused sound
void StopSound(Sound sound);                // Stop playing a sound
```
But notice that audio device must be initialized before loading any sound. To manage audio device, use the following functions:
```c
void InitAudioDevice(void);                  // Initialize audio device and context
void CloseAudioDevice(void);                 // Close the audio device and context (and music stream)
```



## Code provided:

[lessons/00_raylib_blocks_intro](lessons/00_raylib_blocks_intro.c) 
 - lessons/02_raylib_blocks_inputs
 - lessons/03_raylib_blocks_screen_manage
 - lessons/04_raylib_blocks_textures
 - lessons/05_raylib_blocks_audio
 - lessons/06_raylib_blocks_details
 - lessons/resources -> Some useful graphics/text/audio resources for the game

----------------------------------------
```c
solutions/00_raylib_blocks_intro
solutions/02_raylib_blocks_inputs
solutions/03_raylib_blocks_screen_manage
solutions/04_raylib_blocks_textures
solutions/05_raylib_blocks_audio
solutions/06_raylib_blocks_details
```
##Getting help 
We strongly encourage you using the online [raylib forum](forum.raylib.com) or the official [UOC forum]() to discuss challenges with other students. However, we recommend not to look at any source code written by other students or share your source code with others **while working on the challenge**.

##License
All code provided for this challenge is licensed under MIT License. Check [LICENSE](../LICENSE) for further details.
