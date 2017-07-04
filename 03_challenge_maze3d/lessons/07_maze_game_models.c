/*******************************************************************************************
*
*   Challenge 03:   MAZE GAME
*   Lesson 07:      models loading
*   Description:    Static 3D models loading and drawing
*
*   NOTE: This example requires the following header-only files:
*       glad.h      - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h   - Vector and matrix math functions
*       stb_image.h - Multiple formats image loading (BMP, PNG, TGA, JPG...)
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99
*
*   This example has been created using raylib 1.7 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#define GLAD_IMPLEMENTATION
#include "glad.h"               // GLAD extensions loading library
                                // NOTE: Includes required OpenGL headers
                                
#include <GLFW/glfw3.h>         // Windows/Context and inputs management

#define RAYMATH_STANDALONE
#define RAYMATH_IMPLEMENTATION
#include "raymath.h"            // Vector3 and Matrix math functions

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"          // Multiple image fileformats loading functions

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// LESSON 01: Basic data types
#ifndef __cplusplus
// Boolean type
typedef enum { false, true } bool;
#endif

// Color type, RGBA (32bit)
typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

typedef enum { LOG_INFO = 0, LOG_ERROR, LOG_WARNING, LOG_DEBUG, LOG_OTHER } TraceLogType;
    
// LESSON 03: Texture formats enum
typedef enum {
    UNCOMPRESSED_GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    UNCOMPRESSED_GRAY_ALPHA,        // 16 bpp (2 channels)
    UNCOMPRESSED_R5G6B5,            // 16 bpp
    UNCOMPRESSED_R8G8B8,            // 24 bpp
    UNCOMPRESSED_R5G5B5A1,          // 16 bpp (1 bit alpha)
    UNCOMPRESSED_R4G4B4A4,          // 16 bpp (4 bit alpha)
    UNCOMPRESSED_R8G8B8A8,          // 32 bpp
} TextureFormat;

// LESSON 03: Image struct (extended)
// NOTE: Image data is stored in CPU memory (RAM)
typedef struct Image {
    unsigned int width;         // Image base width
    unsigned int height;        // Image base height
    unsigned int mipmaps;       // Mipmap levels, 1 by default
    unsigned int format;        // Data format (TextureFormat type)
    unsigned char *data;        // Image raw data
} Image;

// LESSON 03: Texture2D type
// NOTE: Texture data is stored in GPU memory (VRAM)
typedef struct Texture2D {
    unsigned int id;        // OpenGL texture id
    int width;              // Texture base width
    int height;             // Texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (TextureFormat)
} Texture2D;

// LESSON 05: Camera type, defines a camera position/orientation in 3d space
typedef struct Camera {
    Vector3 position;       // Camera position
    Vector3 target;         // Camera target it looks-at
    Vector3 up;             // Camera up vector (rotation over its axis)
    float fovy;             // Camera field-of-view apperture in Y (degrees)
} Camera;

// LESSON 05: Camera move modes (first person)
typedef enum { 
    MOVE_FRONT = 0, 
    MOVE_BACK, 
    MOVE_RIGHT, 
    MOVE_LEFT, 
    MOVE_UP, 
    MOVE_DOWN 
} CameraMove;

// LESSON 07: Vertex data definning a mesh
typedef struct Mesh {
    int vertexCount;        // number of vertices stored in arrays
    float *vertices;        // vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;       // vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float *normals;         // vertex normals (XYZ - 3 components per vertex) (shader-location = 2)

    unsigned int vaoId;     // OpenGL Vertex Array Object id
    unsigned int vboId[3];  // OpenGL Vertex Buffer Objects id (3 types of vertex data supported)
} Mesh;

// LESSON 07: Shader type (default shader)
typedef struct Shader {
    unsigned int id;        // Shader program id

    // Vertex attributes locations (default locations)
    int vertexLoc;          // Vertex attribute location point    (default-location = 0)
    int texcoordLoc;        // Texcoord attribute location point  (default-location = 1)
    int normalLoc;          // Normal attribute location point    (default-location = 2)
    
    // Texture map locations (3 maps supported)
    int mapTexture0Loc;     // Map texture uniform location point (default-texture-unit = 0)
    int mapTexture1Loc;     // Map texture uniform location point (default-texture-unit = 1)
    int mapTexture2Loc;     // Map texture uniform location point (default-texture-unit = 2)
    
    // Uniform locations
    int mvpLoc;             // ModelView-Projection matrix uniform location point (vertex shader)
    int colDiffuseLoc;      // Diffuse color uniform location point (fragment shader)
    int colSpecularLoc;     // Specular color uniform location point (fragment shader)

} Shader;

// LESSON 07: Material type
typedef struct Material {
    Shader shader;          // Default shader (supports 3 map textures: diffuse, normal, specular)

    Texture2D texDiffuse;   // Diffuse texture  (binded to shader mapTexture0Loc)
    Texture2D texNormal;    // Normal texture   (binded to shader mapTexture1Loc)
    Texture2D texSpecular;  // Specular texture (binded to shader mapTexture2Loc)

    Color colDiffuse;       // Diffuse color
    Color colSpecular;      // Specular color
} Material;

// LESSON 07: Model struct
// NOTE: Model is defined by its Mesh (vertex data), Material (shader) and transform matrix
typedef struct Model {
    Mesh mesh;              // Vertex data buffers (RAM and VRAM)
    Matrix transform;       // Local transform matrix
    Material material;      // Shader and textures data
} Model;

//----------------------------------------------------------------------------------
// Global Variables Declaration
//----------------------------------------------------------------------------------
GLFWwindow *window;

static Texture2D texDefault;
static Shader shdrDefault;
static Matrix matProjection;
static Matrix matModelview;

static double currentTime, previousTime;    // Used to track timmings
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied

// LESSON 03: Keyboard/mouse input management
// Register keyboard/mouse states (current and previous)
static char previousKeyState[512] = { 0 };  // Registers previous frame key state
static char currentKeyState[512] = { 0 };   // Registers current frame key state
static char previousMouseState[3] = { 0 };  // Registers previous mouse button state
static char currentMouseState[3] = { 0 };   // Registers current mouse button state

// LESSON 05: Camera system management
static Vector2 cameraAngle = { 0.0f, 0.0f };

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------

// GLFW3 callback functions to be registered: Error, Key, MouseButton, MouseCursor
static void ErrorCallback(int error, const char* description);
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
static void MouseCursorPosCallback(GLFWwindow *window, double x, double y);

void TraceLog(int msgType, const char *text, ...);      // Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
static Shader LoadShaderDefault(void);                  // Load default shader (basic shader)

// LESSON 01: Window and context creation, extensions loading
//----------------------------------------------------------------------------------
static void InitWindow(int width, int height);          // Initialize window and context
static void InitGraphicsDevice(int width, int height);  // Initialize graphic device
static void CloseWindow(void);                          // Close window and free resources
static void SetTargetFPS(int fps);                      // Set target FPS (maximum)
static void SyncFrame(void);                            // Synchronize to desired framerate

// LESSON 02: Inputs management (keyboard and mouse)
//----------------------------------------------------------------------------------
static bool IsKeyPressed(int key);                  // Detect if a key has been pressed once
static bool IsKeyDown(int key);                     // Detect if a key is being pressed (key held down)
static bool IsMouseButtonPressed(int button);       // Detect if a mouse button has been pressed once
static bool IsMouseButtonDown(int button);          // Detect if a mouse button is being pressed
static Vector2 GetMousePosition(void);              // Returns mouse position XY
static void PollInputEvents(void);                  // Poll (store) all input events

// LESSON 03: Image data loading, texture creation and drawing
//----------------------------------------------------------------------------------
static Image LoadImage(const char *fileName);       // Load image data to CPU memory (RAM)
static void UnloadImage(Image image);               // Unload image data from CPU memory (RAM)
static Texture2D LoadTexture(unsigned char *data, int width, int height, int format);  // Load texture data in GPU memory (VRAM)
static void UnloadTexture(Texture2D texture);       // Unload texture data from GPU memory (VRAM)

static void DrawTexture(Texture2D texture, Vector2 position, Color tint);   // Draw texture in screen position coordinates

// LESSON 04: Level map loading, vertex buffer creation
//----------------------------------------------------------------------------------
static Model LoadCubicmap(Image cubicmap, float cubeSize);  // Load cubicmap (pixels image) into a 3d model
static void UnloadModel(Model model);                       // Unload model data from memory (RAM and VRAM)
static void UploadMeshData(Mesh *mesh);                     // Upload mesh data into VRAM

static void DrawModel(Model model, Vector3 position, float scale, Color tint);

// LESSON 05: Camera system management (1st person)
//----------------------------------------------------------------------------------
static void UpdateCamera(Camera *camera);

// LESSON 06: Collision detection and resolution
//----------------------------------------------------------------------------------
static Vector3 ResolveCollisionCubicmap(Image cubicmap, Vector3 mapPosition, Vector3 *playerPosition, float radius);

// LESSON 07: Models loading and drawing
//----------------------------------------------------------------------------------
static Model LoadModel(const char *fileName);
static Mesh LoadOBJ(const char *fileName);

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    // LESSON 02: Window and graphic device initialization and management
    InitWindow(screenWidth, screenHeight);          // Initialize Window using GLFW3
    
    InitGraphicsDevice(screenWidth, screenHeight);  // Initialize graphic device (OpenGL)
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------    

    // Main game loop     
    while (!glfwWindowShouldClose(window))
    {
        // Update
        //----------------------------------------------------------------------------------
        // ...
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear used buffers: Color and Depth (Depth is used for 3D)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
        
        glfwSwapBuffers(window);
        
        PollInputEvents();          // Register input events
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------
    
    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// GLFW3: Error callback function
static void ErrorCallback(int error, const char* description)
{
    TraceLog(LOG_ERROR, description);
}

// GLFW3: Keyboard callback function
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

// GLFW3: Mouse buttons callback function
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    
}

// GLFW3: Mouse cursor callback function
static void MouseCursorPosCallback(GLFWwindow *window, double x, double y)
{
    
}

// LESSON 01: Window and context creation, extensions loading
//----------------------------------------------------------------------------------
// Initialize window and context (OpenGL 3.3)
static void InitWindow(int width, int height)
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
   
    window = glfwCreateWindow(width, height, "CHALLENGE 03: 3D MAZE GAME", NULL, NULL);
    
    if (!window) glfwTerminate();
    else TraceLog(LOG_INFO, "GLFW3: Window created successfully");
    
    glfwSetWindowPos(window, 200, 200);
    
    glfwSetKeyCallback(window, KeyCallback);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
}

// Initialize graphic device (OpenGL 3.3)
static void InitGraphicsDevice(int width, int height)
{
    // Load OpenGL 3.3 supported extensions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) TraceLog(LOG_WARNING, "GLAD: Cannot load OpenGL extensions");
    else TraceLog(LOG_INFO, "GLAD: OpenGL extensions loaded successfully");
    
    // Print current OpenGL and GLSL version
    TraceLog(LOG_INFO, "GPU: Vendor:   %s", glGetString(GL_VENDOR));
    TraceLog(LOG_INFO, "GPU: Renderer: %s", glGetString(GL_RENDERER));
    TraceLog(LOG_INFO, "GPU: Version:  %s", glGetString(GL_VERSION));
    TraceLog(LOG_INFO, "GPU: GLSL:     %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize OpenGL context (states and resources)
    //----------------------------------------------------------
    
    // Init default white texture
    unsigned char pixels[4] = { 255, 255, 255, 255 };   // 1 pixel RGBA (4 bytes)

    // TODO: texDefault = rlglLoadTexture(pixels, 1, 1, UNCOMPRESSED_R8G8B8A8, 1);

    // Init default Shader (customized for GL 3.3 and ES2)
    // TODO: shdrDefault = LoadDefaultShader();

    // Init internal matProjection and matModelview matrices
    matProjection = MatrixIdentity();
    matModelview = MatrixIdentity();

    // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glDisable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
    glCullFace(GL_BACK);                                    // Cull the back face (default)
    glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
    glEnable(GL_CULL_FACE);                                 // Enable backface culling

#if defined(GRAPHICS_API_OPENGL_11)
    // Init state: Color hints (deprecated in OpenGL 3.0+)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Improve quality of color and texture coordinate interpolation
    glShadeModel(GL_SMOOTH);                                // Smooth shading between vertex (vertex colors interpolation)
#endif

    // Init state: Color/Depth buffers clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
    glClearDepth(1.0f);                                     // Set clear depth value (default)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)

    TraceLog(LOG_INFO, "OpenGL default states initialized successfully");

    // Initialize viewport and matProjection/matModelview matrices
    glViewport(0, 0, width, height);
    
    /*
    glMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
    glLoadIdentity();                                   // Reset current matrix (PROJECTION)
    glOrtho(0, screenWidth, screenHeight, 0, 0.0f, 1.0f); // Orthographic matProjection with top-left corner at (0,0)
    glMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
    glLoadIdentity();                                   // Reset current matrix (MODELVIEW)

    glClearColor(245, 245, 245, 255);                   // Define clear color
    glEnableDepthTest();                                // Enable DEPTH_TEST for 3D
    */
}

// Close window and free resources
static void CloseWindow(void)
{
    // TODO: Unload default shader
    glDeleteTextures(1, &texDefault);

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

// LESSON 02: Inputs management (keyboard and mouse)
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

// Detect if a mouse button is being pressed
static bool IsMouseButtonDown(int button)
{
    return glfwGetMouseButton(window, button);
}

// Detect if a mouse button has been pressed once
static bool IsMouseButtonPressed(int button)
{
    if ((currentMouseState[button] != previousMouseState[button]) && (currentMouseState[button] == 1)) return true;
    else return false;
}

// Returns mouse position XY
static Vector2 GetMousePosition(void)
{
    Vector2 mousePosition;
    double mouseX;
    double mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);

    mousePosition.x = (float)mouseX;
    mousePosition.y = (float)mouseY;
    
    return mousePosition;
}

// Poll (store) all input events
static void PollInputEvents(void)
{
    // Register previous keys states and mouse button states
    for (int i = 0; i < 512; i++) previousKeyState[i] = currentKeyState[i];
    for (int i = 0; i < 3; i++) previousMouseState[i] = currentMouseState[i];
    
    // Input events polling (managed by GLFW3 through callback)
    glfwPollEvents();
}

// LESSON 03: Image data loading, texture creation and drawing
//----------------------------------------------------------------------------------
// Load image data to CPU memory (RAM)
// NOTE: We use stb_image library to support multiple fileformats
static Image LoadImage(const char *fileName)
{
    Image image = { 0 };
    const char *fileExt;
    
    if ((fileExt = strrchr(fileName, '.')) != NULL)
    {
        // Check if file extension is supported
        if ((strcmp(fileExt, ".bmp") == 0) ||
            (strcmp(fileExt, ".png") == 0) ||
            (strcmp(fileExt, ".tga") == 0) ||
            (strcmp(fileExt, ".jpg") == 0) ||
            (strcmp(fileExt, ".gif") == 0) ||
            (strcmp(fileExt, ".psd") == 0))
        {
            int imgWidth = 0;
            int imgHeight = 0;
            int imgBpp = 0;
            
            FILE *imFile = fopen(fileName, "rb");
            
            if (imFile != NULL)
            {
                // NOTE: Using stb_image to load images (Supports: BMP, TGA, PNG, JPG, ...)
                image.data = stbi_load_from_file(imFile, &imgWidth, &imgHeight, &imgBpp, 0);
                
                fclose(imFile);

                image.width = imgWidth;
                image.height = imgHeight;
                image.mipmaps = 1;

                if (imgBpp == 1) image.format = UNCOMPRESSED_GRAYSCALE;
                else if (imgBpp == 2) image.format = UNCOMPRESSED_GRAY_ALPHA;
                else if (imgBpp == 3) image.format = UNCOMPRESSED_R8G8B8;
                else if (imgBpp == 4) image.format = UNCOMPRESSED_R8G8B8A8;
            }
        }
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

    // TODO: texture.id = rlglLoadTexture(image.data, image.width, image.height, UNCOMPRESSED_R8G8B8A8, 1);

    return texture;
}

// Unload texture data from GPU memory (VRAM)
static void UnloadTexture(Texture2D texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
}

// LESSON 04: Level map loading, vertex buffer creation
//----------------------------------------------------------------------------------

// Load cubicmap (pixels image) into a 3d model
// NOTE: Mesh data is generated from 2d image data
static Model LoadCubicmap(Image cubicmap, float cubeSize)
{
    Model model = { 0 };
    
    // TODO: model.mesh = GenMeshCubicmap(cubicmap, cubeSize);
    // TODO: model.material = LoadMaterialDefault();
    model.transform = MatrixIdentity();
    
    return model;
}

// Unload model data from memory (RAM and VRAM)
// NOTE: Unloads Mesh data and Material shader
static void UnloadModel(Model model)
{
    // TODO: Unload mesh data
    // TODO: Unload material
}

static void DrawModel(Model model, Vector3 position, float scale, Color tint)
{
    // Calculate transformation matrix from function parameters
    // Get transform matrix (scale -> translation)
    Matrix matScale = MatrixScale(scale, scale, scale);
    Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);
    
    Matrix matTransform = MatrixMultiply(matScale, matTranslation);

    // Combine model transform matrix with matrix generated by function parameters (matTransform)
    model.transform = MatrixMultiply(model.transform, matTransform);
    model.material.colDiffuse = tint;       // Assign tint as diffuse color

    // TODO: Draw model
    glUseProgram(material.shader.id);       // Bind material shader

    // Upload to shader material.colDiffuse
    glUniform4f(material.shader.colDiffuseLoc, (float)material.colDiffuse.r/255, (float)material.colDiffuse.g/255, (float)material.colDiffuse.b/255, (float)material.colDiffuse.a/255);

    // Upload to shader material.colSpecular (if available)
    if (material.shader.colSpecularLoc != -1) glUniform4f(material.shader.colSpecularLoc, (float)material.colSpecular.r/255, (float)material.colSpecular.g/255, (float)material.colSpecular.b/255, (float)material.colSpecular.a/255);

    // At this point the matModelview matrix just contains the view matrix (camera)
    // That's because Begin3dMode() sets it an no model-drawing function modifies it, all use rlPushMatrix() and rlPopMatrix()
    Matrix matView = matModelview;         // View matrix (camera)
    Matrix matProjection = matProjection;  // Projection matrix (perspective)

    // Calculate model-view matrix combining matModel and matView
    Matrix matModelView = MatrixMultiply(transform, matView);           // Transform to camera-space coordinates
    
    // Set shader textures (diffuse, normal, specular)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.texDiffuse.id);
    glUniform1i(material.shader.mapTexture0Loc, 0);         // Diffuse texture fits in active texture unit 0

    if ((material.texNormal.id != 0) && (material.shader.mapTexture1Loc != -1))
    {
        // Upload to shader specular map flag
        glUniform1i(glGetUniformLocation(material.shader.id, "useNormal"), 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.texNormal.id);
        glUniform1i(material.shader.mapTexture1Loc, 1);     // Normal texture fits in active texture unit 1
    }

    if ((material.texSpecular.id != 0) && (material.shader.mapTexture2Loc != -1))
    {
        // Upload to shader specular map flag
        glUniform1i(glGetUniformLocation(material.shader.id, "useSpecular"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.texSpecular.id);
        glUniform1i(material.shader.mapTexture2Loc, 2);    // Specular texture fits in active texture unit 2
    }

    // Bind mesh VAO (vertex array objects)
    glBindVertexArray(mesh.vaoId);
    
    // Calculate model-view-matProjection matrix (MVP)
    Matrix matMVP = MatrixMultiply(matModelview, matProjection);        // Transform to screen-space coordinates

    // Send combined model-view-matProjection matrix to shader
    glUniformMatrix4fv(material.shader.mvpLoc, 1, false, MatrixToFloat(matMVP));

    // Draw call!
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    
    if (material.texNormal.id != 0)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (material.texSpecular.id != 0)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glActiveTexture(GL_TEXTURE0);       // Set shader active texture to default 0
    glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
    glBindVertexArray(0);               // Unbind VAO
    glUseProgram(0);                    // Unbind shader program
}

// LESSON 05: Camera system management (1st person)
//----------------------------------------------------------------------------------
static void UpdateCamera(Camera *camera)
{
    // TODO: DisableCursor();
    
    // Some useful defines
    #define PLAYER_MOVEMENT_SENSITIVITY                     20.0f
    #define CAMERA_MOUSE_MOVE_SENSITIVITY                   0.003f
    #define CAMERA_FIRST_PERSON_FOCUS_DISTANCE              25.0f
    #define CAMERA_FIRST_PERSON_MIN_CLAMP                   85.0f
    #define CAMERA_FIRST_PERSON_MAX_CLAMP                  -85.0f

    #define CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER  5.0f
    #define CAMERA_FIRST_PERSON_STEP_DIVIDER                30.0f
    #define CAMERA_FIRST_PERSON_WAVING_DIVIDER              200.0f
    
    static float playerEyesPosition = 1.85f;              // Default player eyes position from ground (in meters) 

    static int cameraMoveControl[6]  = { 'W', 'S', 'D', 'A', 'E', 'Q' };
    static int cameraPanControlKey = 2;                   // raylib: MOUSE_MIDDLE_BUTTON
    static int cameraAltControlKey = 342;                 // raylib: KEY_LEFT_ALT
    static int cameraSmoothZoomControlKey = 341;          // raylib: KEY_LEFT_CONTROL


    static int swingCounter = 0;    // Used for 1st person swinging movement
    static Vector2 previousMousePosition = { 0.0f, 0.0f };

    // Mouse movement detection
    Vector2 mousePositionDelta = { 0.0f, 0.0f };
    Vector2 mousePosition = GetMousePosition();
    
    // Keys input detection
    bool panKey = IsMouseButtonDown(cameraPanControlKey);
    bool altKey = IsKeyDown(cameraAltControlKey);
    bool szoomKey = IsKeyDown(cameraSmoothZoomControlKey);
    
    bool direction[6] = { IsKeyDown(cameraMoveControl[MOVE_FRONT]),
                          IsKeyDown(cameraMoveControl[MOVE_BACK]),
                          IsKeyDown(cameraMoveControl[MOVE_RIGHT]),
                          IsKeyDown(cameraMoveControl[MOVE_LEFT]),
                          IsKeyDown(cameraMoveControl[MOVE_UP]),
                          IsKeyDown(cameraMoveControl[MOVE_DOWN]) };

    mousePositionDelta.x = mousePosition.x - previousMousePosition.x;
    mousePositionDelta.y = mousePosition.y - previousMousePosition.y;

    previousMousePosition = mousePosition;

    camera->position.x += (sinf(cameraAngle.x)*direction[MOVE_BACK] -
                           sinf(cameraAngle.x)*direction[MOVE_FRONT] -
                           cosf(cameraAngle.x)*direction[MOVE_LEFT] +
                           cosf(cameraAngle.x)*direction[MOVE_RIGHT])/PLAYER_MOVEMENT_SENSITIVITY;
                           
    camera->position.y += (sinf(cameraAngle.y)*direction[MOVE_FRONT] -
                           sinf(cameraAngle.y)*direction[MOVE_BACK] +
                           1.0f*direction[MOVE_UP] - 1.0f*direction[MOVE_DOWN])/PLAYER_MOVEMENT_SENSITIVITY;
                           
    camera->position.z += (cosf(cameraAngle.x)*direction[MOVE_BACK] -
                           cosf(cameraAngle.x)*direction[MOVE_FRONT] +
                           sinf(cameraAngle.x)*direction[MOVE_LEFT] -
                           sinf(cameraAngle.x)*direction[MOVE_RIGHT])/PLAYER_MOVEMENT_SENSITIVITY;

    bool isMoving = false;  // Required for swinging

    for (int i = 0; i < 6; i++) if (direction[i]) { isMoving = true; break; }
    
    // Camera orientation calculation
    cameraAngle.x += (mousePositionDelta.x*-CAMERA_MOUSE_MOVE_SENSITIVITY);
    cameraAngle.y += (mousePositionDelta.y*-CAMERA_MOUSE_MOVE_SENSITIVITY);
    
    // Angle clamp
    if (cameraAngle.y > CAMERA_FIRST_PERSON_MIN_CLAMP*DEG2RAD) cameraAngle.y = CAMERA_FIRST_PERSON_MIN_CLAMP*DEG2RAD;
    else if (cameraAngle.y < CAMERA_FIRST_PERSON_MAX_CLAMP*DEG2RAD) cameraAngle.y = CAMERA_FIRST_PERSON_MAX_CLAMP*DEG2RAD;

    // Camera is always looking at player
    camera->target.x = camera->position.x - sinf(cameraAngle.x)*CAMERA_FIRST_PERSON_FOCUS_DISTANCE;
    camera->target.y = camera->position.y + sinf(cameraAngle.y)*CAMERA_FIRST_PERSON_FOCUS_DISTANCE;
    camera->target.z = camera->position.z - cosf(cameraAngle.x)*CAMERA_FIRST_PERSON_FOCUS_DISTANCE;
    
    if (isMoving) swingCounter++;

    // Camera position update
    // NOTE: On CAMERA_FIRST_PERSON player Y-movement is limited to player 'eyes position'
    camera->position.y = playerEyesPosition - sinf(swingCounter/CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER)/CAMERA_FIRST_PERSON_STEP_DIVIDER;

    camera->up.x = sinf(swingCounter/(CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER*2))/CAMERA_FIRST_PERSON_WAVING_DIVIDER;
    camera->up.z = -sinf(swingCounter/(CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER*2))/CAMERA_FIRST_PERSON_WAVING_DIVIDER;
}

// LESSON 06: Collision detection and resolution
//----------------------------------------------------------------------------------
// Detect and resolve cubicmap collisions
// NOTE: player position (or camera) is modified inside this function
// TODO: This functions needs to be completely reviewed!
static Vector3 ResolveCollisionCubicmap(Image cubicmap, Vector3 mapPosition, Vector3 *playerPosition, float radius)
{
    #define CUBIC_MAP_HALF_BLOCK_SIZE   0.5

    Color *cubicmapPixels; // = GetImageData(cubicmap);     // TODO

    // Detect the cell where the player is located
    Vector3 impactDirection = { 0.0f, 0.0f, 0.0f };

    int locationCellX = 0;
    int locationCellY = 0;

    locationCellX = floor(playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE);
    locationCellY = floor(playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE);

    if ((locationCellX >= 0) && (locationCellY >= 0) && (locationCellX < cubicmap.width) && (locationCellY < cubicmap.height))
    {
        // Multiple Axis --------------------------------------------------------------------------------------------

        // Axis x-, y-
        if ((locationCellX > 0) && (locationCellY > 0))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX - 1)].r != 0) &&
                (cubicmapPixels[(locationCellY - 1)*cubicmap.width + (locationCellX)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX < radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY < radius))
                {
                    playerPosition->x = locationCellX + mapPosition.x - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    playerPosition->z = locationCellY + mapPosition.z - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                }
            }
        }

        // Axis x-, y+
        if ((locationCellX > 0) && (locationCellY < cubicmap.height - 1))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX - 1)].r != 0) &&
                (cubicmapPixels[(locationCellY + 1)*cubicmap.width + (locationCellX)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX < radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY > 1 - radius))
                {
                    playerPosition->x = locationCellX + mapPosition.x - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    playerPosition->z = locationCellY + mapPosition.z + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                }
            }
        }

        // Axis x+, y-
        if ((locationCellX < cubicmap.width - 1) && (locationCellY > 0))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX + 1)].r != 0) &&
                (cubicmapPixels[(locationCellY - 1)*cubicmap.width + (locationCellX)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX > 1 - radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY < radius))
                {
                    playerPosition->x = locationCellX + mapPosition.x + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    playerPosition->z = locationCellY + mapPosition.z - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                }
            }
        }

        // Axis x+, y+
        if ((locationCellX < cubicmap.width - 1) && (locationCellY < cubicmap.height - 1))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX + 1)].r != 0) &&
                (cubicmapPixels[(locationCellY + 1)*cubicmap.width + (locationCellX)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX > 1 - radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY > 1 - radius))
                {
                    playerPosition->x = locationCellX + mapPosition.x + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    playerPosition->z = locationCellY + mapPosition.z + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                }
            }
        }

        // Single Axis ---------------------------------------------------------------------------------------------------

        // Axis x-
        if (locationCellX > 0)
        {
            if (cubicmapPixels[locationCellY*cubicmap.width + (locationCellX - 1)].r != 0)
            {
                if ((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX < radius)
                {
                    playerPosition->x = locationCellX + mapPosition.x - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 1.0f, 0.0f, 0.0f };
                }
            }
        }
        // Axis x+
        if (locationCellX < cubicmap.width - 1)
        {
            if (cubicmapPixels[locationCellY*cubicmap.width + (locationCellX + 1)].r != 0)
            {
                if ((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX > 1 - radius)
                {
                    playerPosition->x = locationCellX + mapPosition.x + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 1.0f, 0.0f, 0.0f };
                }
            }
        }
        // Axis y-
        if (locationCellY > 0)
        {
            if (cubicmapPixels[(locationCellY - 1)*cubicmap.width + (locationCellX)].r != 0)
            {
                if ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY < radius)
                {
                    playerPosition->z = locationCellY + mapPosition.z - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 0.0f, 0.0f, 1.0f };
                }
            }
        }
        // Axis y+
        if (locationCellY < cubicmap.height - 1)
        {
            if (cubicmapPixels[(locationCellY + 1)*cubicmap.width + (locationCellX)].r != 0)
            {
                if ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY > 1 - radius)
                {
                    playerPosition->z = locationCellY + mapPosition.z + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    impactDirection = (Vector3){ 0.0f, 0.0f, 1.0f };
                }
            }
        }

        // Diagonals -------------------------------------------------------------------------------------------------------

        // Axis x-, y-
        if ((locationCellX > 0) && (locationCellY > 0))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX - 1)].r == 0) &&
                (cubicmapPixels[(locationCellY - 1)*cubicmap.width + (locationCellX)].r == 0) &&
                (cubicmapPixels[(locationCellY - 1)*cubicmap.width + (locationCellX - 1)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX < radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY < radius))
                {
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX) > ((playerPosition->z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY)) playerPosition->x = locationCellX + mapPosition.x - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    else playerPosition->z = locationCellY + mapPosition.z - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);

                    // Return ricochet
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX < radius/3) &&
                        ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY < radius/3))
                    {
                        impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                    }
                }
            }
        }

        // Axis x-, y+
        if ((locationCellX > 0) && (locationCellY < cubicmap.height - 1))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX - 1)].r == 0) &&
                (cubicmapPixels[(locationCellY + 1)*cubicmap.width + (locationCellX)].r == 0) &&
                (cubicmapPixels[(locationCellY + 1)*cubicmap.width + (locationCellX - 1)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX < radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY > 1 - radius))
                {
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX) > (1 - ((playerPosition->z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY))) playerPosition->x = locationCellX + mapPosition.x - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    else playerPosition->z = locationCellY + mapPosition.z + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);

                    // Return ricochet
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX < radius/3) &&
                        ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY > 1 - radius/3))
                    {
                        impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                    }
                }
            }
        }

        // Axis x+, y-
        if ((locationCellX < cubicmap.width - 1) && (locationCellY > 0))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX + 1)].r == 0) &&
                (cubicmapPixels[(locationCellY - 1)*cubicmap.width + (locationCellX)].r == 0) &&
                (cubicmapPixels[(locationCellY - 1)*cubicmap.width + (locationCellX + 1)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX > 1 - radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY < radius))
                {
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX) < (1 - ((playerPosition->z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY))) playerPosition->x = locationCellX + mapPosition.x + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    else playerPosition->z = locationCellY + mapPosition.z - (CUBIC_MAP_HALF_BLOCK_SIZE - radius);

                    // Return ricochet
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX > 1 - radius/3) &&
                        ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY < radius/3))
                    {
                        impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                    }
                }
            }
        }

        // Axis x+, y+
        if ((locationCellX < cubicmap.width - 1) && (locationCellY < cubicmap.height - 1))
        {
            if ((cubicmapPixels[locationCellY*cubicmap.width + (locationCellX + 1)].r == 0) &&
                (cubicmapPixels[(locationCellY + 1)*cubicmap.width + (locationCellX)].r == 0) &&
                (cubicmapPixels[(locationCellY + 1)*cubicmap.width + (locationCellX + 1)].r != 0))
            {
                if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX > 1 - radius) &&
                    ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY > 1 - radius))
                {
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX) < ((playerPosition->z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY)) playerPosition->x = locationCellX + mapPosition.x + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);
                    else playerPosition->z = locationCellY + mapPosition.z + (CUBIC_MAP_HALF_BLOCK_SIZE - radius);

                    // Return ricochet
                    if (((playerPosition->x - mapPosition.x + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellX > 1 - radius/3) &&
                        ((playerPosition->z - mapPosition.z + CUBIC_MAP_HALF_BLOCK_SIZE) - locationCellY > 1 - radius/3))
                    {
                        impactDirection = (Vector3){ 1.0f, 0.0f, 1.0f };
                    }
                }
            }
        }
    }

    // Floor collision
    if (playerPosition->y <= radius)
    {
        playerPosition->y = radius + 0.01f;
        impactDirection = (Vector3) { impactDirection.x, 1, impactDirection.z};
    }
    // Roof collision
    else if (playerPosition->y >= (1.5f - radius))
    {
        playerPosition->y = (1.5f - radius) - 0.01f;
        impactDirection = (Vector3) { impactDirection.x, 1, impactDirection.z};
    }

    free(cubicmapPixels);

    return impactDirection;
}

// LESSON 07: Models loading and drawing
//----------------------------------------------------------------------------------

// Load static model data (RAM and VRAM)
// NOTE: Loads Mesh data and setups default Material with default Shader
static Model LoadModel(const char *fileName)
{
    Model model = { 0 };
    const char *fileExt;
    
    if ((fileExt = strrchr(fileName, '.')) != NULL)
    {
        // Check if file extension is supported
        if (strcmp(fileExt, ".obj") == 0) model.mesh = LoadOBJ(fileName);
    }
    
    //model.material.shader = shdrDefault;
    //model.material.map[MAP_DIFFUSE].texture = 
    
    model.transform = MatrixIdentity();

    return model;
}

// Load static mesh from OBJ file (RAM and VRAM)
static Mesh LoadOBJ(const char *fileName)
{
    Mesh mesh = { 0 };

    char dataType;
    char comments[200];

    int vertexCount = 0;
    int normalCount = 0;
    int texcoordCount = 0;
    int triangleCount = 0;

    FILE *objFile = fopen(fileName, "rt");

    if (objFile == NULL)
    {
        TraceLog(LOG_WARNING, "[%s] OBJ file could not be opened", fileName);
        return mesh;
    }

    // First reading pass: Get vertexCount, normalCount, texcoordCount, triangleCount
    // NOTE: vertex, texcoords and normals could be optimized (to be used indexed on faces definition)
    // NOTE: faces MUST be defined as TRIANGLES (3 vertex per face)
    while (!feof(objFile))
    {
        dataType = '\0';
        fscanf(objFile, "%c", &dataType);

        switch (dataType)
        {
            case '#':   // Comments
            case 'o':   // Object name (One OBJ file can contain multible named meshes)
            case 'g':   // Group name
            case 's':   // Smoothing level
            case 'm':   // mtllib [external .mtl file name]
            case 'u':   // usemtl [material name]
            {
                fgets(comments, 200, objFile);
            } break;
            case 'v':
            {
                fscanf(objFile, "%c", &dataType);

                if (dataType == 't')    // Read texCoord
                {
                    texcoordCount++;
                    fgets(comments, 200, objFile);
                }
                else if (dataType == 'n')    // Read normals
                {
                    normalCount++;
                    fgets(comments, 200, objFile);
                }
                else    // Read vertex
                {
                    vertexCount++;
                    fgets(comments, 200, objFile);
                }
            } break;
            case 'f':
            {
                triangleCount++;
                fgets(comments, 200, objFile);
            } break;
            default: break;
        }
    }

    TraceLog(LOG_DEBUG, "[%s] Mesh vertices: %i", fileName, vertexCount);
    TraceLog(LOG_DEBUG, "[%s] Mesh texcoords: %i", fileName, texcoordCount);
    TraceLog(LOG_DEBUG, "[%s] Mesh normals: %i", fileName, normalCount);
    TraceLog(LOG_DEBUG, "[%s] Mesh triangles: %i", fileName, triangleCount);

    // Once we know the number of vertices to store, we create required arrays
    Vector3 *midVertices = (Vector3 *)malloc(vertexCount*sizeof(Vector3));
    Vector3 *midNormals = NULL;
    if (normalCount > 0) midNormals = (Vector3 *)malloc(normalCount*sizeof(Vector3));
    Vector2 *midTexCoords = NULL;
    if (texcoordCount > 0) midTexCoords = (Vector2 *)malloc(texcoordCount*sizeof(Vector2));

    int countVertex = 0;
    int countNormals = 0;
    int countTexCoords = 0;

    rewind(objFile);        // Return to the beginning of the file, to read again

    // Second reading pass: Get vertex data to fill intermediate arrays
    // NOTE: This second pass is required in case of multiple meshes defined in same OBJ
    while (!feof(objFile))
    {
        fscanf(objFile, "%c", &dataType);

        switch (dataType)
        {
            case '#': case 'o': case 'g': case 's': case 'm': case 'u': case 'f': fgets(comments, 200, objFile); break;
            case 'v':
            {
                fscanf(objFile, "%c", &dataType);

                if (dataType == 't')    // Read texCoord
                {
                    fscanf(objFile, "%f %f%*[^\n]s\n", &midTexCoords[countTexCoords].x, &midTexCoords[countTexCoords].y);
                    countTexCoords++;

                    fscanf(objFile, "%c", &dataType);
                }
                else if (dataType == 'n')    // Read normals
                {
                    fscanf(objFile, "%f %f %f", &midNormals[countNormals].x, &midNormals[countNormals].y, &midNormals[countNormals].z);
                    countNormals++;

                    fscanf(objFile, "%c", &dataType);
                }
                else    // Read vertex
                {
                    fscanf(objFile, "%f %f %f", &midVertices[countVertex].x, &midVertices[countVertex].y, &midVertices[countVertex].z);
                    countVertex++;

                    fscanf(objFile, "%c", &dataType);
                }
            } break;
            default: break;
        }
    }

    // At this point all vertex data (v, vt, vn) has been gathered on midVertices, midTexCoords, midNormals
    // Now we can organize that data into our Mesh struct

    mesh.vertexCount = triangleCount*3;

    // Additional arrays to store vertex data as floats
    mesh.vertices = (float *)malloc(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)malloc(mesh.vertexCount*2*sizeof(float));
    mesh.normals = (float *)malloc(mesh.vertexCount*3*sizeof(float));

    int vCounter = 0;       // Used to count vertices float by float
    int tcCounter = 0;      // Used to count texcoords float by float
    int nCounter = 0;       // Used to count normals float by float

    int vCount[3], vtCount[3], vnCount[3];    // Used to store triangle indices for v, vt, vn

    rewind(objFile);        // Return to the beginning of the file, to read again

    if (normalCount == 0) TraceLog(LOG_INFO, "[%s] No normals data on OBJ, normals will be generated from faces data", fileName);

    // Third reading pass: Get faces (triangles) data and fill VertexArray
    while (!feof(objFile))
    {
        fscanf(objFile, "%c", &dataType);

        switch (dataType)
        {
            case '#': case 'o': case 'g': case 's': case 'm': case 'u': case 'v': fgets(comments, 200, objFile); break;
            case 'f':
            {
                // NOTE: It could be that OBJ does not have normals or texcoords defined!

                if ((normalCount == 0) && (texcoordCount == 0)) fscanf(objFile, "%i %i %i", &vCount[0], &vCount[1], &vCount[2]);
                else if (normalCount == 0) fscanf(objFile, "%i/%i %i/%i %i/%i", &vCount[0], &vtCount[0], &vCount[1], &vtCount[1], &vCount[2], &vtCount[2]);
                else if (texcoordCount == 0) fscanf(objFile, "%i//%i %i//%i %i//%i", &vCount[0], &vnCount[0], &vCount[1], &vnCount[1], &vCount[2], &vnCount[2]);
                else fscanf(objFile, "%i/%i/%i %i/%i/%i %i/%i/%i", &vCount[0], &vtCount[0], &vnCount[0], &vCount[1], &vtCount[1], &vnCount[1], &vCount[2], &vtCount[2], &vnCount[2]);

                mesh.vertices[vCounter] = midVertices[vCount[0]-1].x;
                mesh.vertices[vCounter + 1] = midVertices[vCount[0]-1].y;
                mesh.vertices[vCounter + 2] = midVertices[vCount[0]-1].z;
                vCounter += 3;
                mesh.vertices[vCounter] = midVertices[vCount[1]-1].x;
                mesh.vertices[vCounter + 1] = midVertices[vCount[1]-1].y;
                mesh.vertices[vCounter + 2] = midVertices[vCount[1]-1].z;
                vCounter += 3;
                mesh.vertices[vCounter] = midVertices[vCount[2]-1].x;
                mesh.vertices[vCounter + 1] = midVertices[vCount[2]-1].y;
                mesh.vertices[vCounter + 2] = midVertices[vCount[2]-1].z;
                vCounter += 3;

                if (normalCount > 0)
                {
                    mesh.normals[nCounter] = midNormals[vnCount[0]-1].x;
                    mesh.normals[nCounter + 1] = midNormals[vnCount[0]-1].y;
                    mesh.normals[nCounter + 2] = midNormals[vnCount[0]-1].z;
                    nCounter += 3;
                    mesh.normals[nCounter] = midNormals[vnCount[1]-1].x;
                    mesh.normals[nCounter + 1] = midNormals[vnCount[1]-1].y;
                    mesh.normals[nCounter + 2] = midNormals[vnCount[1]-1].z;
                    nCounter += 3;
                    mesh.normals[nCounter] = midNormals[vnCount[2]-1].x;
                    mesh.normals[nCounter + 1] = midNormals[vnCount[2]-1].y;
                    mesh.normals[nCounter + 2] = midNormals[vnCount[2]-1].z;
                    nCounter += 3;
                }
                else
                {
                    // If normals not defined, they are calculated from the 3 vertices [N = (V2 - V1) x (V3 - V1)]
                    Vector3 norm = VectorCrossProduct(VectorSubtract(midVertices[vCount[1]-1], midVertices[vCount[0]-1]), VectorSubtract(midVertices[vCount[2]-1], midVertices[vCount[0]-1]));
                    VectorNormalize(&norm);

                    mesh.normals[nCounter] = norm.x;
                    mesh.normals[nCounter + 1] = norm.y;
                    mesh.normals[nCounter + 2] = norm.z;
                    nCounter += 3;
                    mesh.normals[nCounter] = norm.x;
                    mesh.normals[nCounter + 1] = norm.y;
                    mesh.normals[nCounter + 2] = norm.z;
                    nCounter += 3;
                    mesh.normals[nCounter] = norm.x;
                    mesh.normals[nCounter + 1] = norm.y;
                    mesh.normals[nCounter + 2] = norm.z;
                    nCounter += 3;
                }

                if (texcoordCount > 0)
                {
                    // NOTE: If using negative texture coordinates with a texture filter of GL_CLAMP_TO_EDGE doesn't work!
                    // NOTE: Texture coordinates are Y flipped upside-down
                    mesh.texcoords[tcCounter] = midTexCoords[vtCount[0]-1].x;
                    mesh.texcoords[tcCounter + 1] = 1.0f - midTexCoords[vtCount[0]-1].y;
                    tcCounter += 2;
                    mesh.texcoords[tcCounter] = midTexCoords[vtCount[1]-1].x;
                    mesh.texcoords[tcCounter + 1] = 1.0f - midTexCoords[vtCount[1]-1].y;
                    tcCounter += 2;
                    mesh.texcoords[tcCounter] = midTexCoords[vtCount[2]-1].x;
                    mesh.texcoords[tcCounter + 1] = 1.0f - midTexCoords[vtCount[2]-1].y;
                    tcCounter += 2;
                }
            } break;
            default: break;
        }
    }

    fclose(objFile);

    // Security check, just in case no normals or no texcoords defined in OBJ
    if (texcoordCount == 0) for (int i = 0; i < (2*mesh.vertexCount); i++) mesh.texcoords[i] = 0.0f;

    // Now we can free temp mid* arrays
    free(midVertices);
    free(midNormals);
    free(midTexCoords);

    // NOTE: At this point we have all vertex, texcoord, normal data for the model in mesh struct
    TraceLog(LOG_INFO, "[%s] Mesh loaded successfully in RAM (CPU)", fileName);
    
    // Upload mesh data into VRAM
    UploadMeshData(&mesh);

    return mesh;
}

// Upload mesh data into VRAM
static void UploadMeshData(Mesh *mesh)
{
    GLuint vaoId = 0;           // Vertex Array Objects (VAO)
    GLuint vboId[3] = { 0 };    // Vertex Buffer Objects (VBOs)

    // Initialize Quads VAO (Buffer A)
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // NOTE: Attributes must be uploaded considering default locations points

    // Enable vertex attributes: position (shader-location = 0)
    glGenBuffers(1, &vboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->vertexCount, mesh->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);

    // Enable vertex attributes: texcoords (shader-location = 1)
    glGenBuffers(1, &vboId[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->vertexCount, mesh->texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(1);

    // Enable vertex attributes: normals (shader-location = 2)
    if (mesh->normals != NULL)
    {
        glGenBuffers(1, &vboId[2]);
        glBindBuffer(GL_ARRAY_BUFFER, vboId[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->vertexCount, mesh->normals, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(2);
    }
    else
    {
        // Default normal vertex attribute set 1.0f
        glVertexAttrib3f(2, 1.0f, 1.0f, 1.0f);
        glDisableVertexAttribArray(2);
    }

    mesh->vboId[0] = vboId[0];     // Vertex position VBO
    mesh->vboId[1] = vboId[1];     // Texcoords VBO
    mesh->vboId[2] = vboId[2];     // Normals VBO

    mesh->vaoId = vaoId;
    
    TraceLog(LOG_INFO, "[VAO ID %i] Mesh uploaded successfully to VRAM (GPU)", mesh->vaoId);
}

// Load default shader
static Shader LoadShaderDefault(void)
{
    Shader shader = { 0 };
    
    // STEP 01: Define shader code
    // NOTE: It can be defined in external text file and just loaded
    //-------------------------------------------------------------------------------

    // Vertex shader directly defined, no external file required
    char vDefaultShaderStr[] =
        "#version 330                       \n"
        "in vec3 vertexPosition;            \n"
        "in vec2 vertexTexCoord;            \n"
        "in vec3 vertexNormal;              \n"
        "out vec2 fragTexCoord;             \n"
        "out vec4 fragNormal;               \n"
        "uniform mat4 mvpMatrix;            \n"
        "void main()                        \n"
        "{                                  \n"
        "    fragTexCoord = vertexTexCoord; \n"
        "    fragNormal = vertexNormal;     \n"
        "    gl_Position = mvpMatrix*vec4(vertexPosition, 1.0); \n"
        "}                                  \n";

    // Fragment shader directly defined, no external file required
    char fDefaultShaderStr[] =
        "#version 330       \n"
        "in vec2 fragTexCoord;              \n"
        "in vec4 fragNormal;                \n"
        "out vec4 finalColor;               \n"
        "uniform sampler2D texture0;        \n"
        "uniform vec4 colDiffuse;           \n"
        "void main()                        \n"
        "{                                  \n"
        "    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
        "    finalColor = texelColor*colDiffuse;        \n"
        "}                                  \n";

    // STEP 02: Load shader program 
    // NOTE: Vertex shader and fragment shader are compiled at runtime
    //-------------------------------------------------------------------------------
    GLuint vertexShader;
    GLuint fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *pvs = vDefaultShaderStr;
    const char *pfs = fDefaultShaderStr;

    glShaderSource(vertexShader, 1, &pvs, NULL);
    glShaderSource(fragmentShader, 1, &pfs, NULL);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    shader.id = glCreateProgram();

    glAttachShader(shader.id, vertexShader);
    glAttachShader(shader.id, fragmentShader);

    // Default attribute shader locations must be binded before linking
    glBindAttribLocation(shader.id, 0, "vertexPosition");
    glBindAttribLocation(shader.id, 1, "vertexTexCoord");
    glBindAttribLocation(shader.id, 2, "vertexNormal");

    // NOTE: If some attrib name is not found in the shader, it locations becomes -1

    glLinkProgram(shader.id);

    // NOTE: All uniform variables are intitialised to 0 when a program links

    // Shaders already compiled into program, not required any more
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (shader.id != 0) TraceLog(LOG_INFO, "[SHDR ID %i] Default shader loaded successfully", shader.id);
    else TraceLog(LOG_WARNING, "[SHDR ID %i] Default shader could not be loaded", shader.id);

    // STEP 03: Load default shader locations
    // NOTE: Connection points (locations) between shader and our code must be retrieved
    //-----------------------------------------------------------------------------------
    if (shader.id != 0) 
    {
        // NOTE: Default shader attrib locations have been fixed before linking:
        //          vertex position location    = 0
        //          vertex texcoord location    = 1
        //          vertex normal location      = 2

        // Get handles to GLSL input attibute locations
        shader.vertexLoc = glGetAttribLocation(shader.id, "vertexPosition");
        shader.texcoordLoc = glGetAttribLocation(shader.id, "vertexTexCoord");
        shader.normalLoc = glGetAttribLocation(shader.id, "vertexNormal");

        // Get handles to GLSL uniform locations (vertex shader)
        shader.mvpLoc  = glGetUniformLocation(shader.id, "mvpMatrix");

        // Get handles to GLSL uniform locations (fragment shader)
        shader.colDiffuseLoc = glGetUniformLocation(shader.id, "colDiffuse");
        shader.colSpecularLoc = glGetUniformLocation(shader.id, "colSpecular");
        shader.mapTexture0Loc = glGetUniformLocation(shader.id, "texture0");
        shader.mapTexture1Loc = glGetUniformLocation(shader.id, "texture1");
        shader.mapTexture2Loc = glGetUniformLocation(shader.id, "texture2"); 
    }

    return shader;
}

// Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
void TraceLog(int msgType, const char *text, ...)
{
    va_list args;
    va_start(args, text);

    switch (msgType)
    {
        case LOG_INFO: fprintf(stdout, "INFO: "); break;
        case LOG_ERROR: fprintf(stdout, "ERROR: "); break;
        case LOG_WARNING: fprintf(stdout, "WARNING: "); break;
        case LOG_DEBUG: fprintf(stdout, "DEBUG: "); break;
        default: break;
    }

    vfprintf(stdout, text, args);
    fprintf(stdout, "\n");

    va_end(args);

    if (msgType == LOG_ERROR) exit(1);
}