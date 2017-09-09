/*******************************************************************************************
*
*   Challenge 02:   DUNGEON GAME
*   Lesson 05:      image and textures
*   Description:    Image data loading, texture creation and drawing
*
*   NOTE: This example requires OpenGL 3.3 or ES2 for shaders support,
*       OpenGL 1.1 does not support shaders but it can also be used.
*
*   Compile rlgl module using:
*       gcc -c external/rlgl.c -Wall -std=c99 -DRLGL_STANDALONE -DRAYMATH_IMPLEMENTATION -DGRAPHICS_API_OPENGL_33
*
*   NOTE: rlgl module requires the following header-only files:
*       glad.h    - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h - Vector and matrix math functions
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -Iexternal rlgl.o -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99
*
*   Copyright (c) 2017 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <GLFW/glfw3.h>         // Windows/Context and inputs management

#define RLGL_STANDALONE
#include "rlgl.h"               // rlgl library: OpenGL 1.1 immediate-mode style coding

#include <stdio.h>              // Standard input-output C library
#include <stdlib.h>             // Memory management functions: malloc(), free()
#include <string.h>             // String manipulation functions: strrchr(), strcmp()

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Rectangle type
typedef struct Rectangle {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

// LESSON 05: Image struct
// NOTE: Image data is stored in CPU memory (RAM)
typedef struct Image {
    unsigned int width;         // Image width
    unsigned int height;        // Image height
    Color *data;                // Image data (Color - 32 bpp - R8G8B8A8)
} Image;

#define WHITE   (Color){ 255, 255, 255, 255 }       // White color definition

//----------------------------------------------------------------------------------
// Global Variables Declaration
//----------------------------------------------------------------------------------

// LESSON 01: Window and graphic device initialization and management
GLFWwindow *window;

// Timming required variables
static double currentTime, previousTime;    // Used to track timmings
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied

// LESSON 03: Keyboard input management
// Register keyboard states (current and previous)
static char previousKeyState[512] = { 0 };  // Registers previous frame key state
static char currentKeyState[512] = { 0 };   // Registers current frame key state

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

// LESSON 02: Window and graphic device initialization and management
//----------------------------------------------------------------------------------
static void ErrorCallback(int error, const char* description);                              // GLFW3: Error callback function
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);   // GLFW3: Keyboard callback function

static void InitWindow(int width, int height);          // Initialize window and context
static void InitGraphicsDevice(int width, int height);  // Initialize graphic device
static void CloseWindow(void);                          // Close window and free resources
static void SetTargetFPS(int fps);                      // Set target FPS (maximum)
static void SyncFrame(void);                            // Synchronize to desired framerate

// LESSON 03: Inputs management (keyboard and mouse)
//----------------------------------------------------------------------------------
static bool IsKeyDown(int key);                     // Detect if a key is being pressed (key held down)
static bool IsKeyPressed(int key);                  // Detect if a key has been pressed once
static void PollInputEvents(void);                  // Poll (store) all input events

// LESSON 04: Basic shapes drawing
//----------------------------------------------------------------------------------
static void DrawLine(Vector2 startPos, Vector2 endPos, Color color);                // Draw a line
static void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);          // Draw a triangle
static void DrawRectangle(int posX, int posY, int width, int height, Color color);  // Draw a rectangle
static void DrawRectangleRec(Rectangle rec, Color color);                           // Draw a rectangle

// LESSON 05: Image data loading, texture creation, texture drawing
//----------------------------------------------------------------------------------
static Image LoadImage(const char *fileName);       // Load image data to CPU memory (RAM)
static void UnloadImage(Image image);               // Unload image data from CPU memory (RAM)
static Texture2D LoadTextureFromImage(Image image); // Load texture from image data into GPU memory (VRAM)
static void UnloadTexture(Texture2D texture);       // Unload texture data from GPU memory (VRAM)
static Image LoadBMP(const char *fileName);         // Load BMP image file data

static void DrawTexture(Texture2D texture, int posX, int posY, Color tint);   // Draw texture in screen position coordinates
static void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint);    // Draw a Texture2D with extended parameters
static void DrawTextureRec(Texture2D texture, Rectangle sourceRec, Vector2 position, Color tint);           // Draw a part of a texture (defined by a rectangle)
static void DrawTexturePro(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector2 origin, float rotation, Color tint); // Draw a part of a texture (defined by a rectangle) with 'pro' parameters

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    // LESSON 01: Window and graphic device initialization and management
    InitWindow(screenWidth, screenHeight);          // Initialize Window using GLFW3
    
    InitGraphicsDevice(screenWidth, screenHeight);  // Initialize graphic device (OpenGL)
       
    // Load player texture
    Image imPlayer = LoadImage("resources/player.bmp");
    Texture2D texPlayer = LoadTextureFromImage(imPlayer);
    UnloadImage(imPlayer);

    // Load tileset texture
    Image imTileset = LoadImage("resources/tileset.bmp");
    Texture2D texTileset = LoadTextureFromImage(imTileset);
    UnloadImage(imTileset);
    
    // Init player position
    Rectangle player = { 100, 100, 8, 8 };
    Rectangle oldPlayer = player;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------    

    // Main game loop    
    while (!glfwWindowShouldClose(window))
    {
        // Update
        //----------------------------------------------------------------------------------
        // Player movement logic
        oldPlayer = player;
        
        if (IsKeyDown(GLFW_KEY_DOWN)) player.y += 2;
        else if (IsKeyDown(GLFW_KEY_UP)) player.y -= 2;
        
        if (IsKeyDown(GLFW_KEY_RIGHT)) player.x += 2;
        else if (IsKeyDown(GLFW_KEY_LEFT)) player.x -= 2;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        rlClearScreenBuffers();             // Clear current framebuffer

        DrawTexture(texTileset, screenWidth/2 - texTileset.width/2, screenHeight/2 - texTileset.height/2, WHITE);   // Draw tileset texture
        
        DrawTexture(texPlayer, player.x, player.y, WHITE); // Draw player texture
        
        rlglDraw();                         // Internal buffers drawing (2D data)

        glfwSwapBuffers(window);            // Swap buffers: show back buffer into front
        PollInputEvents();                  // Register input events (keyboard, mouse)
        SyncFrame();                        // Wait required time to target framerate
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texPlayer);       // Unload player texture
    UnloadTexture(texTileset);      // Unload tileset texture
    
    rlglClose();                    // Unload rlgl internal buffers and default shader/texture
    
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definitions
//----------------------------------------------------------------------------------

// GLFW3: Error callback
static void ErrorCallback(int error, const char* description)
{
    TraceLog(LOG_ERROR, description);
}

// GLFW3: Keyboard callback
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else currentKeyState[key] = action;
}

// LESSON 01: Window creation and management
//----------------------------------------------------------------------------------
// Initialize window and context (OpenGL 3.3)
static void InitWindow(int screenWidth, int screenHeight)
{
    // GLFW3 Initialization + OpenGL 3.3 Context + Extensions
    glfwSetErrorCallback(ErrorCallback);
    
    if (!glfwInit()) TraceLog(LOG_WARNING, "GLFW3: Can not initialize GLFW");
    else TraceLog(LOG_INFO, "GLFW3: GLFW initialized successfully");
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
   
    window = glfwCreateWindow(screenWidth, screenHeight, "CHALLENGE 02: 2D DUNGEON GAME", NULL, NULL);
    
    if (!window) glfwTerminate();
    else TraceLog(LOG_INFO, "GLFW3: Window created successfully");
    
    glfwSetWindowPos(window, 200, 200);
    
    glfwSetKeyCallback(window, KeyCallback);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
}

// Close window and free resources
static void CloseWindow(void)
{
    glfwDestroyWindow(window);      // Close window
    glfwTerminate();                // Free GLFW3 resources
}

// Set target FPS (maximum)
static void SetTargetFPS(int fps)
{
    if (fps < 1) targetTime = 0.0;
    else targetTime = 1.0/(double)fps;
}

// Synchronize to desired framerate
static void SyncFrame(void)
{
    // Frame time control system
    currentTime = glfwGetTime();
    frameTime = currentTime - previousTime;
    previousTime = currentTime;

    // Wait for some milliseconds...
    if (frameTime < targetTime)
    {
        double prevTime = glfwGetTime();
        double nextTime = 0.0;

        // Busy wait loop
        while ((nextTime - prevTime) < (targetTime - frameTime)) nextTime = glfwGetTime();

        currentTime = glfwGetTime();
        double extraTime = currentTime - previousTime;
        previousTime = currentTime;

        frameTime += extraTime;
    }
}

// LESSON 02: Graphic device initialization and management
//----------------------------------------------------------------------------------
// Initialize graphic device (OpenGL 3.3)
static void InitGraphicsDevice(int width, int height)
{
    // Load OpenGL 3.3 supported extensions
    rlLoadExtensions(glfwGetProcAddress);

    // Initialize OpenGL context (states and resources)
    rlglInit(width, height);

    // Initialize viewport and internal projection/modelview matrices
    rlViewport(0, 0, width, height);
    rlMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
    rlLoadIdentity();                                   // Reset current matrix (PROJECTION)
    rlOrtho(0, width, height, 0, 0.0f, 1.0f);           // Orthographic projection with top-left corner at (0,0)
    rlMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
    rlLoadIdentity();                                   // Reset current matrix (MODELVIEW)

    rlClearColor(0, 0, 0, 255);                         // Define clear color (BLACK)
    rlEnableDepthTest();                                // Enable DEPTH_TEST for 3D
}

// LESSON 03: Inputs management (keyboard and mouse)
//----------------------------------------------------------------------------------
// Detect if a key is being pressed (key held down)
static bool IsKeyDown(int key)
{
    return glfwGetKey(window, key);
}

// Detect if a key has been pressed once
static bool IsKeyPressed(int key)
{
    if ((currentKeyState[key] != previousKeyState[key]) && (currentKeyState[key] == 1)) return true;
    else return false;
}

// Poll (store) all input events
static void PollInputEvents(void)
{
    // Register previous keys states (required to check variations)
    for (int i = 0; i < 512; i++) previousKeyState[i] = currentKeyState[i];

    // Input events polling (managed by GLFW3 through callback)
    glfwPollEvents();
}

// LESSON 04: Basic shapes drawing
//----------------------------------------------------------------------------------
// Draw a line
static void DrawLine(Vector2 startPos, Vector2 endPos, Color color)
{
    rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(startPos.x, startPos.y);
        rlVertex2f(endPos.x, endPos.y);
    rlEnd();
}

// Draw a triangle
static void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
    rlEnableTexture(GetTextureDefault().id); // Default white texture

    rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(v1.x, v1.y);
        rlVertex2f(v2.x, v2.y);
        rlVertex2f(v2.x, v2.y);
        rlVertex2f(v3.x, v3.y);
    rlEnd();
    
    rlDisableTexture();
}

// Draw color-filled rectangle
static void DrawRectangle(int posX, int posY, int width, int height, Color color)
{
#define TRIS_RECTANGLE
#if defined(TRIS_RECTANGLE)
    // NOTE: We use rlgl OpenGL 1.1 style vertex definition
    rlBegin(RL_TRIANGLES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2i(posX, posY);
        rlVertex2i(posX, posY + height);
        rlVertex2i(posX + width, posY + height);

        rlVertex2i(posX, posY);
        rlVertex2i(posX + width, posY + height);
        rlVertex2i(posX + width, posY);
    rlEnd();
#else   // QUADS_RECTANGLE
    // NOTE: Alternative implementation using RL_QUADS
    rlEnableTexture(GetDefaultTexture().id);    // Default white texture
    rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlNormal3f(0.0f, 0.0f, 1.0f);

        rlTexCoord2f(0.0f, 0.0f);
        rlVertex2f(position.x, position.y);

        rlTexCoord2f(0.0f, 1.0f);
        rlVertex2f(position.x, position.y + size.y);

        rlTexCoord2f(1.0f, 1.0f);
        rlVertex2f(position.x + size.x, position.y + size.y);

        rlTexCoord2f(1.0f, 0.0f);
        rlVertex2f(position.x + size.x, position.y);
    rlEnd();
    rlDisableTexture();
#endif
}

// Draw a color-filled rectangle
static void DrawRectangleRec(Rectangle rec, Color color)
{
    DrawRectangle(rec.x, rec.y, rec.width, rec.height, color);
}

// LESSON 05: Image data loading, texture creation, texture drawing
//----------------------------------------------------------------------------------
// Load image data to CPU memory (RAM)
static Image LoadImage(const char *fileName)
{
    Image image = { 0 };
    const char *fileExt;
    
    if ((fileExt = strrchr(fileName, '.')) != NULL)
    {
        // Check if file extension is supported
        if (strcmp(fileExt, ".bmp") == 0) image = LoadBMP(fileName);
    }

    return image;
}

// Unload image data from CPU memory (RAM)
static void UnloadImage(Image image)
{
    if (image.data != NULL) free(image.data);
}

// Load texture from image data into GPU memory (VRAM)
static Texture2D LoadTextureFromImage(Image image)
{
    #define UNCOMPRESSED_R8G8B8A8    7      // Texture format (must match image.data)
    
    Texture2D texture = { 0 };
    
    // NOTE: Texture2D struct is defined inside rlgl
    texture.width = image.width;
    texture.height = image.height;
    texture.format = UNCOMPRESSED_R8G8B8A8;
    texture.mipmaps = 1;

    texture.id = rlLoadTexture(image.data, image.width, image.height, UNCOMPRESSED_R8G8B8A8, 1);

    return texture;
}

// Unload texture data from GPU memory (VRAM)
static void UnloadTexture(Texture2D texture)
{
    if (texture.id > 0) rlDeleteTextures(texture.id);
}

// Load bmp fileformat data
// NOTE: Be careful with lines padding and v-flipped data 
static Image LoadBMP(const char *fileName)
{
    Image image = { 0 };

	int imgWidth;
	int imgHeight;
	short imgBpp;
	int imgDataOffset;

	FILE *bmpFile = fopen(fileName, "rb");

	fseek(bmpFile, 10, SEEK_SET);
	fread(&imgDataOffset, 4, 1, bmpFile);   // Read bmp data offset
	fseek(bmpFile, 18, SEEK_SET);
	fread(&imgWidth, 4, 1, bmpFile);        // Read bmp width
	fread(&imgHeight, 4, 1, bmpFile);       // Read bmp height
	fseek(bmpFile, 28, SEEK_SET);
	fread(&imgBpp, 2, 1, bmpFile);          // Read bmp bit-per-pixel (usually 24bpp - B8G8R8)
	
	Color *imgData = (Color *)malloc(imgWidth*imgHeight*sizeof(Color));
	
	fseek(bmpFile, imgDataOffset, SEEK_SET);
	
	// Calculate image padding per line
	int padding = (imgWidth*imgBpp)%32;
	int unusedData = 0;
	int extraBytes = 0;
	
	if ((padding/8) > 0) extraBytes = 4 - (padding/8);

	// Read image data
	for(int j = 0; j < imgHeight; j++)
	{
		for(int i = 0; i < imgWidth; i++)
		{
			fread(&imgData[j*imgWidth + i].b, 1, 1, bmpFile);
			fread(&imgData[j*imgWidth + i].g, 1, 1, bmpFile);
			fread(&imgData[j*imgWidth + i].r, 1, 1, bmpFile);
            imgData[j*imgWidth + i].a = 255;    // Set alpha to fully opaque by default
            
            // NOTE: We consider a color key: MAGENTA RGB{ 255, 0, 255 },
            // in that case, pixel will be transparent
            if ((imgData[j*imgWidth + i].r == 255) && 
                (imgData[j*imgWidth + i].g == 0) && 
                (imgData[j*imgWidth + i].b == 255)) imgData[j*imgWidth + i].a = 0;
		}
        
		fread(&unusedData, extraBytes, 1, bmpFile);
	}
	
	fclose(bmpFile);
	
	image.data = (Color *)malloc(imgWidth*imgHeight*sizeof(Color));
	
	// Flip image vertically
    for (int j = 0; j < imgHeight; j++)
	{
		for (int i = 0; i < imgWidth; i++)
		{
			image.data[j*imgWidth + i] = imgData[((imgHeight - 1) - j)*imgWidth + i];
		}
	}
	
	free(imgData);
    
    image.width = imgWidth;
    image.height = imgHeight;
    
    TraceLog(LOG_INFO, "[%s] BMP Image loaded successfully (%ix%i)", fileName, imgWidth, imgHeight);
    
    return image;
}

// NOTE: Multiple versions od DrawTexture() are provided with multiple options

// Draw texture in screen position coordinates
static void DrawTexture(Texture2D texture, int posX, int posY, Color tint)
{
    DrawTextureEx(texture, (Vector2){ posX, posY }, 0, 1.0f, tint);
}

// Draw a Texture2D with extended parameters
static void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint)
{
    Rectangle sourceRec = { 0, 0, texture.width, texture.height };
    Rectangle destRec = { (int)position.x, (int)position.y, texture.width*scale, texture.height*scale };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, tint);
}

// Draw a part of a texture (defined by a rectangle)
static void DrawTextureRec(Texture2D texture, Rectangle sourceRec, Vector2 position, Color tint)
{
    Rectangle destRec = { (int)position.x, (int)position.y, abs(sourceRec.width), abs(sourceRec.height) };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, tint);
}

// Draw a part of a texture (defined by a rectangle) with 'pro' parameters
// NOTE: origin is relative to destination rectangle size
static void DrawTexturePro(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector2 origin, float rotation, Color tint)
{
    // Check if texture is valid
    if (texture.id != 0)
    {
        if (sourceRec.width < 0) sourceRec.x -= sourceRec.width;
        if (sourceRec.height < 0) sourceRec.y -= sourceRec.height;

        rlEnableTexture(texture.id);

        rlPushMatrix();
            rlTranslatef((float)destRec.x, (float)destRec.y, 0);
            rlRotatef(rotation, 0, 0, 1);
            rlTranslatef(-origin.x, -origin.y, 0);

            rlBegin(RL_QUADS);
                rlColor4ub(tint.r, tint.g, tint.b, tint.a);
                rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

                // Bottom-left corner for texture and quad
                rlTexCoord2f((float)sourceRec.x/texture.width, (float)sourceRec.y/texture.height);
                rlVertex2f(0.0f, 0.0f);

                // Bottom-right corner for texture and quad
                rlTexCoord2f((float)sourceRec.x/texture.width, (float)(sourceRec.y + sourceRec.height)/texture.height);
                rlVertex2f(0.0f, (float)destRec.height);

                // Top-right corner for texture and quad
                rlTexCoord2f((float)(sourceRec.x + sourceRec.width)/texture.width, (float)(sourceRec.y + sourceRec.height)/texture.height);
                rlVertex2f((float)destRec.width, (float)destRec.height);

                // Top-left corner for texture and quad
                rlTexCoord2f((float)(sourceRec.x + sourceRec.width)/texture.width, (float)sourceRec.y/texture.height);
                rlVertex2f((float)destRec.width, 0.0f);
            rlEnd();
        rlPopMatrix();

        rlDisableTexture();
    }
}
