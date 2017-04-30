
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
 - Sounds and music loading and playing
 - Image files loading (RAM and VRAM)

**Learning Outcomes:**
 - OpenGL 3.3 graphic pipeline functionality (basic shader)
 - Window creation, configuration and management (GLFW3)
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
[01](#lesson-01-introduction-to-opengl) | opengl functionality, <br>window and context creation, <br>extensions loading | [01_maze_game_intro.c](lessons/01_maze_game_intro.c) | InitWindow(), CloseWindow(), <br>BeginDrawing(), EndDrawing()
[02](#lesson-02-input-management) | ... | [02_maze_game_inputs.c](lessons/02_maze_game_inputs.c) | ...
[03](#lesson-03-textures-loading) | ... | [03_maze_game_textures.c](lessons/03_maze_game_textures.c) | ...
[04](#lesson-04-cubicmap-loading) | ... | [04_maze_game_cubicmap.c](lessons/04_maze_game_cubicmap.c) | ...
[05](#lesson-05-camera-first-person) | ... | [05_maze_game_camera.c](lessons/05_maze_game_camera.c) | ...
[06](#lesson-06-collisions) | ... | [06_maze_game_collisions.c](lessons/06_maze_game_collisions.c) | ...
[07](#lesson-07-models-loading) | ... | [07_maze_game_models.c](lessons/07_maze_game_models.c) | ...

**NOTE:** Most of the documentation for the challenge is directly included in the source code files as code comments, in the form of *TODO* points for every task to be completed. Read carefully those comments to understand every task and how implement the proposed solutions.

### Lesson 01: Introduction to OpenGL

*Lesson code file to review: [01_maze_game_intro.c](lessons/01_maze_game_intro.c)*

Introduction to modern OpenGL graphic pipeline
In this first lesson we will introduce raylib library and videogames programming principles. We will setup raylib and take a look to its functionality; we will see how videogame life cycle works and we will implement a basic screens management system for our game.

Learn modern OpenGL 3.3 functionality, understand graphic pipeline and programmable shaders.

### Lesson 02: Inputs management

*Lesson code file to review: [02_maze_game_inputs.c](lessons/02_maze_game_inputs.c)*

Read user inputs (keyboard-mouse-gamepad) using GLFW3 library.

### Lesson 03: Textures loading (VRAM) and drawing on screen

*Lesson code file to review: [03_maze_game_textures.c](lessons/03_maze_game_textures.c)*

Load image data from a file, decodifying information if required, convert that image data to a texture (GPU uploading) and leran to draw that texture on the canvas.

### Lesson 04: Level map data loading and vertex buffers generation (VBO)

*Lesson code file to review: [04_maze_game_cubicmap.c](lessons/04_maze_game_cubicmap.c)*

Load cubicmap from image file data, generate 3D mesh for the map (vertex data) and screen drawing.

Functions to be implemented:
```c
LoadCubicmap(), UnloadCubicmap()
void DrawCubicmap(Cubicmap map);
```

### Lesson 05: Camera system creation and management (1st person)

*Lesson code file to review: [05_maze_game_camera.c](lessons/05_maze_game_camera.c)*

Manage camera system and implement first person view.

Functions to be implemented:
```c
void UpdateCamera(Camera *camera);
```

### Lesson 06: AABB collision detection and resolution

*Lesson code file to review: [06_maze_game_collisions.c](lessons/06_maze_game_collisions.c)*

Implement 3D collision system based on cubemap.

Functions to be implemented:
```c
bool CheckCollisionCubicmap(void);
```

### Lesson 07: Models loading, transform and drawing (3d meshes)

*Lesson code file to review: [07_maze_game_models.c](lessons/07_maze_game_models.c)*

Support OBJ fileformat 3D models loading, simple mesh loading and scene placement.

## Getting help 
We strongly encourage you using the online [raylib forum](forum.raylib.com) to discuss challenges with other students. However, we recommend not to look at any source code written by other students or share your source code with others **while working on the challenge**.

## License
All code provided for this challenge is licensed under MIT License. Check [LICENSE](../LICENSE) for further details.

*Copyright (c) 2017 Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*
