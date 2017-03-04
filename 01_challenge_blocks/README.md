
# Challenge 01: Blocks game

*by Ramon Santamaria - @raysan5*

## Introduction
In this challenge we will implement a Blocks game similar to the well-known game [Arkanoid](https://en.wikipedia.org/wiki/Arkanoid) (Taito, 1986). Along this process we will learn how the videogames life cycle works, how to manage the window and player inputs, and how to draw some graphics on screen.

**Learning Outcomes:**
 - raylib functionality and possibilities
 - Videogame life cycle (Init -> Update -> Draw -> DeInit)
 - Basic screens management with screens transition
 - Graphic device and inputs management (keyboard, mouse)
 - Basic shapes drawing (circle, rectangle)
 - Collision detection and resolution
 - Textures loading and drawing
 - SpriteFonts loading and text drawing
 - Sounds loading and playing
 
This game is developed using [raylib](http://www.raylib.com/), a simple and easy-to-use library to learn videogames programming.

## Lessons Summary

Lesson | Learning outcome | Source file | Involved functions

 - Lesson 01: Windows 

Most of the documentation for the challenge is directly included in the code source files as code comments for every task to achieve. Read carefully those comments to understand every task and how implement the proposed solutions in the form of *TODO* points.

## Environment setup
Just download and install raylib. raylib is distributed on Windows as a standalone installer containing all required tools to develop videogames in C/C++.

raylib follows the KISS principle, providing simple functions with clear naming conventions. Main documentation for the library functionality is condensed in a single [cheatsheet](http://www.raylib.com/cheatsheet/cheatsheet.html).

raylib is structured in several C files, most of them decoupled from each other and functionality is mostly configurable on library compilation

## Introduction to raylib
 - raylib Features
 - raylib Architecture
 - raylib Data Structures
 - raylib Examples
 - raylib Wiki
 
To compile the code, you will be using the Notepad++ script `raylib_compile_execute`.

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
