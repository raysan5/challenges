
# Challenge 03: 3D Maze game

*by Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*

## Introduction
In this challenge we will implement a 3D Maze game similar to classic game [Doom](https://en.wikipedia.org/wiki/Doom_(series)) (id Software, 1993). Along this process we will learn how to manage vertex data and contruct levels with it

This game is developed using [OpenGL](https://en.wikipedia.org/wiki/OpenGL). A low level graphics library to access and control GPU: data transmission (vertex, textures, shaders...), drawing process, graphic pipeline...

This game is developed using [rlgl](), a [raylib](http://www.raylib.com/) auxiliar module intended to simplify low level GPU access and teach basic principles of graphics programming like vertex buffers usage, textures binding, shaders usage...

Before starting with this challenge, it's recommended to have completed the previous challenges: 
 - [Challenge 01: Blocks game](../01_challenge_blocks) - A blocks game where player has to break a wall of blocks controlling a ball with a paddle.
 - [Challenge 02: 2D Dungeon game](../02_challenge_dungeon2d) - A tile-based dungeon 2D game where player moves around a 2D dungeon finding keys and defeating monsters.

It's assumed that all concepts explained in those challenges are already learn by student.

**Previous knowledge required:**
 - Videogame life cycle (Init -> Update -> Draw -> DeInit)
 - Basic screens management with screens transition
 - Collision detection and resolution
 - Sounds loading and playing
 - Window creation, configuration and management (GLFW3)
 - Keyboard and mouse inputs management (GLFW3)
 - Image files loading (RAM and VRAM) and drawing

**Learning Outcomes:**
 - OpenGL 3.3 graphic pipeline functionality (basic shader)
 - Context creation (OpenGL 3.3) and extensions loading (GLAD)
 - Inputs management (keyboard, mouse, gamepad) (GLFW3)
 - Textures loading (VRAM) and drawing on screen
 - Level map data loading and vertex buffers generation (VBO)
 - Models loading, transform and drawing (3d meshes)
 - Camera system creation and management (1st person)
 - AABB collision detection and resolution
 
**NOTE:** All code provided is in C language for simplicity and clearness but it's up to the student to use more complex C++ code structures (OOP) if desired.
 
## Lessons Summary

Lesson | Learning outcome | Source file | Related functions
:-----:|------------------|:------------|:-----------------:
[01](#lesson-01-introduction-to-raylib-and-videogames-programming) | raylib functionality, <br>videogame lyfe cycle, <br>basic screens management | [01_blocks_game_intro.c](lessons/01_blocks_game_intro.c) | InitWindow(), CloseWindow(), <br>BeginDrawing(), EndDrawing()

**NOTE:** Most of the documentation for the challenge is directly included in the source code files as code comments, in the form of *TODO* points for every task to be completed. Read carefully those comments to understand every task and how implement the proposed solutions.

### Lesson 01: Introduction to modern OpenGL graphic pipeline

*Lesson code file to review: [01_blocks_game_intro.c](lessons/01_blocks_game_intro.c)*

In this first lesson we will introduce raylib library and videogames programming principles. We will setup raylib and take a look to its functionality; we will see how videogame life cycle works and we will implement a basic screens management system for our game.

Learn modern OpenGL 3.3 functionality, understand graphic pipeline and programmable shaders.

### Lesson 02: Context creation and inputs management

Read user inputs (keyboard-mouse-gamepad) using GLFW3 library.

### Lesson 03: Textures loading (VRAM) and drawing on screen

Load image data from a file, decodifying information if required, convert that image data to a texture (GPU uploading) and leran to draw that texture on the canvas.

### Lesson 04: Level map data loading and vertex buffers generation (VBO)

Load cubicmap from image file data, generate 3D mesh for the map (vertex data) and screen drawing.

Functions to be implemented:
```c
LoadCubicmap(), UnloadCubicmap()
void DrawCubicmap(Cubicmap map);
```

### Lesson 05: Camera system creation and management (1st person)

Manage camera system and implement first person view.

Functions to be implemented:
```c
void UpdateCamera(Camera *camera);
```

### Lesson 06: AABB collision detection and resolution

Implement 3D collision system based on cubemap.

Functions to be implemented:
```c
bool CheckCollisionCubicmap(void);
```

### Lesson 07: Models loading, transform and drawing (3d meshes)

Support OBJ fileformat 3D models loading, simple mesh loading and scene placement.

## Getting help 
We strongly encourage you using the online [raylib forum](forum.raylib.com) to discuss challenges with other students. However, we recommend not to look at any source code written by other students or share your source code with others **while working on the challenge**.

## License
All code provided for this challenge is licensed under MIT License. Check [LICENSE](../LICENSE) for further details.

*Copyright (c) 2017 Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*
