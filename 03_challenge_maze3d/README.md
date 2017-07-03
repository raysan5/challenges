
# Challenge 03: 3D Maze game

*by Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*

## Introduction
In this challenge we will implement a 3D Maze game similar to classic game [Doom](https://en.wikipedia.org/wiki/Doom_(series)) (id Software, 1993). Along this process we will learn how to manage vertex data and contruct levels with it.

This game is developed using [OpenGL](https://en.wikipedia.org/wiki/OpenGL). A low level graphics library used to access and control GPU processes like data transmission (vertex, textures, shaders) and drawing.

Before starting with this challenge, it's recommended to complete the previous challenges: 
 - [Challenge 01: Blocks game](../01_challenge_blocks) - A blocks game where player has to break a wall of blocks controlling a ball with a paddle.
 - [Challenge 02: 2D Dungeon game](../02_challenge_dungeon2d) - A tile-based dungeon 2D game where player moves around a 2D dungeon finding keys and defeating monsters.

It's assumed that all concepts explained in those challenges have already been learnt by student.

**Previous knowledge required:**
 - Videogame life cycle (Init -> Update -> Draw -> DeInit)
 - Basic screens management with screens transition
 - Collision detection and resolution
 - Sounds and music loading and playing

**Learning Outcomes:**
 - OpenGL 3.3 graphic pipeline functionality
 - Window creation, configuration and management (GLFW3)
 - Context creation (OpenGL 3.3) and extensions loading (GLAD)
 - Inputs management (keyboard, mouse) (GLFW3)
 - Image loading (RAM), texture creation (VRAM) and drawing
 - Level map data loading and vertex buffers generation (VBO)
 - Camera system creation and management (1st person)
 - Collision detection and resolution (AABB collisions)
 - Models loading, transform and drawing (3d meshes)
 
**NOTE:** All code provided is in C language for simplicity and clearness but it's up to the student to use more complex C++ code structures (OOP) if desired.
 
## Lessons Summary

Lesson | Learning outcome | Source file | Related functions
:-----:|------------------|:------------|:-----------------:
[01](#lesson-01-introduction-to-opengl) | opengl functionality, <br>window and context creation, <br>extensions loading | [01_maze_game_intro.c](lessons/01_maze_game_intro.c) | InitWindow(), <br>CloseWindow(), <br>InitGraphicsDevice()
[02](#lesson-02-inputs-management) | inputs management (gamepad) | [02_maze_game_inputs.c](lessons/02_maze_game_inputs.c) | IsKeyDown(), IsKeyPressed(), <br>IsMouseButtonDown(), IsMouseButtonPressed(), <br>GetMousePosition()
[03](#lesson-03-textures-loading) | image loading, <br>texture creation and drawing | [03_maze_game_textures.c](lessons/03_maze_game_textures.c) | LoadImage(), LoadTexture()
[04](#lesson-04-level-map-loading) | level map loading, <br>vertex buffers creation | [04_maze_game_cubicmap.c](lessons/04_maze_game_cubicmap.c) | LoadCubicmap(), <br>UnloadModel(), <br>DrawModel()
[05](#lesson-05-camera-system-management-1st-person) | camera system (1st person) | [05_maze_game_camera.c](lessons/05_maze_game_camera.c) | UpdateCamera()
[06](#lesson-06-collision-detection-and-resolution) | collision detection and resolution | [06_maze_game_collisions.c](lessons/06_maze_game_collisions.c) | ResolveCollisionCubicmap()
[07](#lesson-07-models-loading) | models loading and drawing | [07_maze_game_models.c](lessons/07_maze_game_models.c) | LoadModel(), LoadOBJ()

**NOTE:** Most of the documentation for the challenge is directly included in the source code files as code comments, in the form of *TODO* points for every task to be completed. Read carefully those comments to understand every task and how implement the proposed solutions.

### Lesson 01: Introduction to OpenGL

*Lesson code file to review: [01_maze_game_intro.c](lessons/01_maze_game_intro.c)*

For a great introduction to modern OpenGL, [check this documentation](https://learnopengl.com/).

For windows creation and management using GLFW3, check [Lesson 02 from Challenge 02](../02_challenge_dungeon2d/README.md#lesson-02-window-creation-and-management).

**Graphic Device initialization**

Once the window is created with the correct configuration for the desired graphic device context (in our case, OpenGL 3.3 Core profile), we need to initialize any required OpenGL extensión and initialize some context configuration parameters.

Functions to be implemented:
```c
void InitGraphicsDevice(int screenWidth, int screenHeight); // Initialize graphics device context
```

### Lesson 02: Inputs management

*Lesson code file to review: [02_maze_game_inputs.c](lessons/02_maze_game_inputs.c)*

We will read user inputs from keyboard and mouse, to do that we will use GLFW3 library, to abstract our code from multiple platforms. In GLFW3 inputs come as events polled at a regular basis (usually every frame) and can be read in callback functions.

Functions to be implemented:
```c
bool IsKeyPressed(int key);                  // Detect if a key has been pressed once
bool IsKeyDown(int key);                     // Detect if a key is being pressed (key held down)
bool IsMouseButtonPressed(int button);       // Detect if a mouse button has been pressed once
bool IsMouseButtonDown(int button);          // Detect if a mouse button is being pressed
Vector2 GetMousePosition(void);              // Returns mouse position XY
```

### Lesson 03: Textures loading

*Lesson code file to review: [03_maze_game_textures.c](lessons/03_maze_game_textures.c)*

We will load image data from a file, decompressing and/or decodifying information if required, and convert that image data to a texture (GPU uploading) and learn to draw that texture on the canvas.

In our implementation we will support multiple image fileformats (.bmp, .jpg, .tga, .png...) using the header-only library: [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h).

Once image data is loaded, we will convert it to a texture and use it for drawing on the screen.

Functions to be implemented:
```c
Image LoadImage(const char *fileName);                   // Load image data from file (RAM)
void UnloadImage(Image image);                           // Unload image data from RAM
Texture2D LoadTextureFromImage(Image image);             // Load texture from image data (VRAM)
void UnloadTexture(Texture2D texture);                   // Unload texture from VRAM
void DrawTexture(Texture2D texture, Vector2 position, Color tint); // Draw texture in screen position coordinates
```

### Lesson 04: Level map loading

*Lesson code file to review: [04_maze_game_cubicmap.c](lessons/04_maze_game_cubicmap.c)*

In this lesson we are loading the level map from image data and we will generate a 3D cubes-based mesh for the level, defining all required vertex data.

*TODO: Image comparing source image and generated 3D mesh*

Functions to be implemented:
```c
Model LoadCubicmap(Image cubicmap, float cubeSize);   // Load cubicmap image into a 3d model
void UnloadModel(Model model);                        // Unload model data from memory (RAM and VRAM)

void DrawModel(Model model, Vector3 position, float scale, Color tint);   // Draw model on screen
```

### Lesson 05: Camera system management (1st person)

*Lesson code file to review: [05_maze_game_camera.c](lessons/05_maze_game_camera.c)*

We will learn how to manage a camera system in 3d and we will implement a first person camera.

Actually, a camera system, as we understand it, doesn't exist, it only consist of a series of transformation we apply to all elements of our 3D world to simulate a 3D perspective and positioning.

Functions to be implemented:
```c
void UpdateCamera(Camera *camera);            // Update camera system with transformation relative to user inputs 
```

### Lesson 06: Collision detection and resolution

*Lesson code file to review: [06_maze_game_collisions.c](lessons/06_maze_game_collisions.c)*

Collision detection for our 3D cubes-based map could be simplyfied to a 2D grid problem, just comparing player position and a defined player radius against cell positions in the grid that represent walls.

*TODO: Illustrative image?*

Functions to be implemented:
```c
Vector3 ResolveCollisionCubicmap(Image cubicmap, Vector3 position, Vector3 *playerPosition, float radius);   // Check collision between map and player data
```

### Lesson 07: Models loading

*Lesson code file to review: [07_maze_game_models.c](lessons/07_maze_game_models.c)*

In this lesson we will learn to load simple 3D models from OBJ fileformat, one of the most simple mesh formats. Once mesh is loaded, we can place it anywhere in the scene and draw it using a texture.

*TODO: Relationship diagram between Model = Mesh + Transform + Material (Shader + Textures + Parameters)*

Functions to be implemented:
```c
Model LoadModel(const char *fileName);          // Load 3d model from file
Mesh LoadOBJ(const char *fileName);             // Load mesh from OBJ file
```

## Getting help 
We strongly encourage you using the online [raylib forum](forum.raylib.com) to discuss challenges with other students. However, we recommend not to look at any source code written by other students or share your source code with others **while working on the challenge**.

## License
All code provided for this challenge is licensed under MIT License. Check [LICENSE](../LICENSE) for further details.

*Copyright (c) 2017 Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*
