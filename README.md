# Graphics Programming
This repo contains some challenges about Graphics Programming. Challenges are organized in folders and include the solutions. They are intended to be used as tutorials to learn graphics programming for videogames development.

Some of those challenges are based on [raylib](http://www.raylib.com/), a simple and easy-to-use library to learn videogames programming.

## Challenge 01. Blocks game

A blocks game where player has to break a wall of blocks controlling a ball with a paddle.

This game is developed using [raylib](http://www.raylib.com/) library.

**Learning outcomes:**
 - raylib functionality and possibilities
 - Videogame life cycle (Init -> Update -> Draw -> DeInit)
 - Basic screens management with screens transition
 - Basic shapes drawing (circle, rectangle)
 - Inputs management (keyboard, mouse)
 - Collision detection and resolution
 - Textures loading and drawing
 - Fonts loading and text drawing
 - Sounds loading and playing

*TODO: add animated gif*

## Challenge 02. Dungeon 2D game

A tile-based dungeon 2D game where player moves around a 2D dungeon finding keys and defeating monsters.

This game is developed using raylib [rlgl](https://github.com/raysan5/raylib/blob/develop/src/rlgl.c) low-level library module.

**Learning outcomes:**
 - rlgl functionality and possibilities
 - Window creation, configuration and management (GLFW3)
 - Keyboard and mouse inputs management (GLFW3)
 - Basic shaped drawing defining vertex data (immediate-mode)
 - Image files loading (RAM and VRAM) and drawing
 - Tile map data loading from a text file
 - Sprites based animation

*TODO: add animated gif*


## Challenge 03. Maze 3D game

A cubes-based maze 3D game where player has to escape from a 3D laberythn.

This game is developed using [OpenGL](https://en.wikipedia.org/wiki/OpenGL) graphics library.

**Learning outcomes:**
 - OpenGL 3.3 graphic pipeline functionality (basic shader)
 - Window creation, configuration and management (GLFW3)
 - Context creation (OpenGL 3.3) and extensions loading (GLAD)
 - Inputs management (keyboard, mouse, gamepad) (GLFW3)
 - Image files loading (RAM and VRAM) and drawing on screen
 - Level map data loading and vertex buffers generation (VBO)
 - Models loading, transform and drawing (3d meshes)
 - Camera system creation and management (1st person)
 - AABB collision detection and resolution

*TODO: add animated gif*

# License

All challenges and solutions are licensed under MIT License. Check [LICENSE](LICENSE) for further details.

*Copyright (c) 2017 Ramon Santamaria ([@raysan5](https://twitter.com/raysan5))*
