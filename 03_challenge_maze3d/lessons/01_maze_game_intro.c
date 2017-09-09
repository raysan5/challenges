/*******************************************************************************************
*
*   Challenge 03:   MAZE GAME
*   Lesson 01:      OpenGL intro
*   Description:    Introduction to OpenGL 3.3 Core profile
*
*   NOTE: This example requires the following header-only files:
*       glad.h      - OpenGL extensions loader (stripped to only required extensions)
*       raymath.h   - Vector and matrix math functions
*       stb_image.h - Multiple formats image loading (BMP, PNG, TGA, JPG...)
*
*   Compile example using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -Iexternal -lglfw3 -lopengl32 -lgdi32 -Wall -std=c99
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

// Rectangle type
typedef struct Rectangle {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

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

#define WHITE   (Color){ 255, 255, 255, 255 }

//----------------------------------------------------------------------------------
// Global Variables Declaration
//----------------------------------------------------------------------------------
GLFWwindow *window;

static Texture2D texDefault;
static Shader shdrDefault;

static Matrix matProjection;                // Projection matrix to draw our world
static Matrix matModelview;                 // Modelview matrix to draw our world

static double currentTime, previousTime;    // Used to track timmings
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied

// LESSON 03: Keyboard/mouse input management
// Register keyboard/mouse states (current and previous)
static char previousKeyState[512] = { 0 };  // Registers previous frame key state
static char currentKeyState[512] = { 0 };   // Registers current frame key state
static char previousMouseState[3] = { 0 };  // Registers previous mouse button state
static char currentMouseState[3] = { 0 };   // Registers current mouse button state

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
static Color *GetImageData(Image image);            // Get pixel data from image as Color array
static Texture2D LoadTexture(unsigned char *data, int width, int height, int format);  // Load texture data in GPU memory (VRAM)
static void UnloadTexture(Texture2D texture);       // Unload texture data from GPU memory (VRAM)

static void DrawTexture(Texture2D texture, Vector2 position, Color tint);   // Draw texture in screen position coordinates

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
    
    // Define our camera
    Camera camera;
    camera.position = Vector3One();
    camera.target = Vector3Zero();
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    
    // Calculate projection matrix (from perspective) and view matrix from camera look at
    matProjection = MatrixPerspective(camera.fovy*DEG2RAD, (double)screenWidth/(double)screenHeight, 0.01, 1000.0);
    matModelview = MatrixLookAt(camera.position, camera.target, camera.up);
    
    // 2D projection
    // matProjection = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
    // matModelview = MatrixIdentity();
    
    // Load model diffuse texture
    Image imDiffuse = LoadImage("resources/cubemap_atlas01.png");
    Texture2D texDiffuse = LoadTexture(imDiffuse.data, imDiffuse.width, imDiffuse.height, imDiffuse.format);
    UnloadImage(imDiffuse);
    
    Vector3 mapPosition = Vector3Zero();
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------    

    // Main game loop     
    while (!glfwWindowShouldClose(window))
    {
        // Update
        //----------------------------------------------------------------------------------
        Vector3 oldCamPos = camera.position;

        matModelview = MatrixLookAt(camera.position, camera.target, camera.up);
        
        // Check player collision (we simplify to 2D collision detection)
        Vector2 playerPos = { camera.position.x, camera.position.z };
        float playerRadius = 0.1f;  // Collision radius (player is modelled as a cilinder for collision)
        
        int playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
        int playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);
        
        if (IsKeyPressed(GLFW_KEY_SPACE)) printf("Player map cell position: (%i, %i)\n", playerCellX, playerCellY);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear used buffers: Color and Depth (Depth is used for 3D)
        
        
        
        glfwSwapBuffers(window);            // Swap buffers: show back buffer into front
        PollInputEvents();                  // Register input events (keyboard, mouse)
        SyncFrame();                        // Wait required time to target framerate
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
    else currentKeyState[key] = action;
}

// GLFW3: Mouse buttons callback function
static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    currentMouseState[button] = action;
}

// GLFW3: Mouse cursor callback function
static void MouseCursorPosCallback(GLFWwindow *window, double x, double y)
{
    //mousePosition.x = (float)x;
    //mousePosition.y = (float)y;
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
    
    glfwSetKeyCallback(window, KeyCallback);                    // Track keyboard events
    glfwSetMouseButtonCallback(window, MouseButtonCallback);    // Track mouse button events
    glfwSetCursorPosCallback(window, MouseCursorPosCallback);   // Track mouse position changes
    
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// Disable cursor for first person camera
    
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

    texDefault = LoadTexture(pixels, 1, 1, UNCOMPRESSED_R8G8B8A8);

    // Init default Shader (customized for GL 3.3 and ES2)
    shdrDefault = LoadShaderDefault();

    // Init state: Depth test
    glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
    glEnable(GL_DEPTH_TEST);                                // Enable depth testing for 3D

    // Init state: Blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
    glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

    // Init state: Culling
    // NOTE: All shapes/models triangles are drawn CCW
    glCullFace(GL_BACK);                                    // Cull the back face (default)
    glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
    glEnable(GL_CULL_FACE);                                 // Enable backface culling

    // Init state: Color/Depth buffers clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
    glClearDepth(1.0f);                                     // Set clear depth value (default)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)

    TraceLog(LOG_INFO, "OpenGL default states initialized successfully");

    // Initialize viewport
    glViewport(0, 0, width, height);
    
    // Init internal matProjection and matModelview matrices
    matProjection = MatrixIdentity();
    matModelview = MatrixIdentity();
}

// Close window and free resources
static void CloseWindow(void)
{
    // TODO: Unload default shader
    glDeleteTextures(1, &texDefault.id);

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
                image.data = stbi_load_from_file(imFile, &imgWidth, &imgHeight, &imgBpp, 4);
                
                fclose(imFile);

                image.width = imgWidth;
                image.height = imgHeight;
                image.format = UNCOMPRESSED_R8G8B8A8;
                
                TraceLog(LOG_INFO, "Image loaded successfully (%ix%i)", image.width, image.height);
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

// Unload texture data from GPU memory (VRAM)
static void UnloadTexture(Texture2D texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
}

// Load texture data in GPU memory (VRAM)
static Texture2D LoadTexture(unsigned char *data, int width, int height, int format)
{
    Texture2D texture = { 0 };
    
    // NOTE: Texture2D struct is defined inside rlgl
    texture.width = width;
    texture.height = height;
    texture.format = UNCOMPRESSED_R8G8B8A8;
    texture.mipmaps = 1;
    
    glBindTexture(GL_TEXTURE_2D, 0);    // Free any old binding

    glGenTextures(1, &texture.id);              // Generate Pointer to the texture
    glBindTexture(GL_TEXTURE_2D, texture.id);

    switch (format)
    {
        case UNCOMPRESSED_GRAYSCALE:
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, (unsigned char *)data);

            // With swizzleMask we define how a one channel texture will be mapped to RGBA
            // Required GL >= 3.3 or EXT_texture_swizzle/ARB_texture_swizzle
            GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

            TraceLog(LOG_INFO, "[TEX ID %i] Grayscale texture loaded and swizzled", texture.id);
        } break;
        case UNCOMPRESSED_GRAY_ALPHA:
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, (unsigned char *)data);

            GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        } break;
        case UNCOMPRESSED_R5G6B5: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, (unsigned short *)data); break;
        case UNCOMPRESSED_R8G8B8: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *)data); break;
        case UNCOMPRESSED_R5G5B5A1: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, (unsigned short *)data); break;
        case UNCOMPRESSED_R4G4B4A4: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA4, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, (unsigned short *)data); break;
        case UNCOMPRESSED_R8G8B8A8: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *)data); break;
        default: TraceLog(LOG_WARNING, "Texture format not recognized"); break;
    }
    
    // Configure texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
    // Magnification and minification filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    
    // Unbind current texture
    glBindTexture(GL_TEXTURE_2D, 0);

    if (texture.id > 0) TraceLog(LOG_INFO, "[TEX ID %i] Texture created successfully (%ix%i)", texture.id, width, height);
    else TraceLog(LOG_WARNING, "Texture could not be created");

    return texture;
}

// Auxiliar functions
//----------------------------------------------------------------------------------
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
        "out vec3 fragNormal;               \n"
        "uniform mat4 mvp;                  \n"
        "void main()                        \n"
        "{                                  \n"
        "    fragTexCoord = vertexTexCoord; \n"
        "    fragNormal = vertexNormal;     \n"
        "    gl_Position = mvp*vec4(vertexPosition, 1.0); \n"
        "}                                  \n";

    // Fragment shader directly defined, no external file required
    char fDefaultShaderStr[] =
        "#version 330                       \n"
        "in vec2 fragTexCoord;              \n"
        "in vec3 fragNormal;                \n"
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
        shader.mvpLoc  = glGetUniformLocation(shader.id, "mvp");

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

// Get pixel data from image as Color array
static Color *GetImageData(Image image)
{
    Color *pixels = (Color *)malloc(image.width*image.height*sizeof(Color));

    int k = 0;

    for (int i = 0; i < image.width*image.height; i++)
    {
        switch (image.format)
        {
            case UNCOMPRESSED_GRAYSCALE:
            {
                pixels[i].r = ((unsigned char *)image.data)[k];
                pixels[i].g = ((unsigned char *)image.data)[k];
                pixels[i].b = ((unsigned char *)image.data)[k];
                pixels[i].a = 255;

                k++;
            } break;
            case UNCOMPRESSED_GRAY_ALPHA:
            {
                pixels[i].r = ((unsigned char *)image.data)[k];
                pixels[i].g = ((unsigned char *)image.data)[k];
                pixels[i].b = ((unsigned char *)image.data)[k];
                pixels[i].a = ((unsigned char *)image.data)[k + 1];

                k += 2;
            } break;
            case UNCOMPRESSED_R5G5B5A1:
            {
                unsigned short pixel = ((unsigned short *)image.data)[k];

                pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111000000) >> 6)*(255/31));
                pixels[i].b = (unsigned char)((float)((pixel & 0b0000000000111110) >> 1)*(255/31));
                pixels[i].a = (unsigned char)((pixel & 0b0000000000000001)*255);

                k++;
            } break;
            case UNCOMPRESSED_R5G6B5:
            {
                unsigned short pixel = ((unsigned short *)image.data)[k];

                pixels[i].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)*(255/31));
                pixels[i].g = (unsigned char)((float)((pixel & 0b0000011111100000) >> 5)*(255/63));
                pixels[i].b = (unsigned char)((float)(pixel & 0b0000000000011111)*(255/31));
                pixels[i].a = 255;

                k++;
            } break;
            case UNCOMPRESSED_R4G4B4A4:
            {
                unsigned short pixel = ((unsigned short *)image.data)[k];

                pixels[i].r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12)*(255/15));
                pixels[i].g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)*(255/15));
                pixels[i].b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)*(255/15));
                pixels[i].a = (unsigned char)((float)(pixel & 0b0000000000001111)*(255/15));

                k++;
            } break;
            case UNCOMPRESSED_R8G8B8A8:
            {
                pixels[i].r = ((unsigned char *)image.data)[k];
                pixels[i].g = ((unsigned char *)image.data)[k + 1];
                pixels[i].b = ((unsigned char *)image.data)[k + 2];
                pixels[i].a = ((unsigned char *)image.data)[k + 3];

                k += 4;
            } break;
            case UNCOMPRESSED_R8G8B8:
            {
                pixels[i].r = (unsigned char)((unsigned char *)image.data)[k];
                pixels[i].g = (unsigned char)((unsigned char *)image.data)[k + 1];
                pixels[i].b = (unsigned char)((unsigned char *)image.data)[k + 2];
                pixels[i].a = 255;

                k += 3;
            } break;
            default: TraceLog(LOG_WARNING, "Format not supported for pixel data retrieval"); break;
        }
    }

    return pixels;
}