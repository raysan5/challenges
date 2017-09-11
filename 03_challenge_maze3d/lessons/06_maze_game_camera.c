/*******************************************************************************************
*
*   Challenge 03:   MAZE GAME
*   Lesson 06:      camera
*   Description:    Camera system (1st person)
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

#include <stdarg.h>             // Required for TraceLog()

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

// Camera type, defines a camera position/orientation in 3d space
typedef struct Camera {
    Vector3 position;       // Camera position
    Vector3 target;         // Camera target it looks-at
    Vector3 up;             // Camera up vector (rotation over its axis)
    float fovy;             // Camera field-of-view apperture in Y (degrees)
} Camera;

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

// LESSON 03: Shader type (default shader)
typedef struct Shader {
    unsigned int id;        // Shader program id

    // Vertex attributes locations (default locations)
    int vertexLoc;          // Vertex attribute location point    (default-location = 0)
    int texcoordLoc;        // Texcoord attribute location point  (default-location = 1)
    int normalLoc;          // Normal attribute location point    (default-location = 2)
    
    // Uniform locations
    int mvpLoc;             // ModelView-Projection matrix uniform location point (vertex shader)
    int colorLoc;           // Diffuse color uniform location point (fragment shader)
    int mapTextureLoc;      // Map texture uniform location point (default-texture-unit = 0)

} Shader;

// LESSON 04: Vertex data defining a mesh
typedef struct Mesh {
    int vertexCount;        // number of vertices stored in arrays
    float *vertices;        // vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;       // vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float *normals;         // vertex normals (XYZ - 3 components per vertex) (shader-location = 2)

    unsigned int vaoId;     // OpenGL Vertex Array Object id
    unsigned int vboId[3];  // OpenGL Vertex Buffer Objects id (3 types of vertex data supported)
} Mesh;

// LESSON 04: Material type
typedef struct Material {
    Shader shader;          // Default shader
    Texture2D texDiffuse;   // Diffuse texture
    Color colDiffuse;       // Diffuse color
} Material;

// LESSON 04: Model struct
// NOTE: Model is defined by its Mesh (vertex data), Material (shader) and transform matrix
typedef struct Model {
    Mesh mesh;              // Vertex data buffers (RAM and VRAM)
    Matrix transform;       // Local transform matrix
    Material material;      // Shader and textures data
} Model;

// LESSON 06: Camera move modes (first person)
typedef enum { 
    MOVE_FRONT = 0, 
    MOVE_BACK, 
    MOVE_RIGHT, 
    MOVE_LEFT, 
    MOVE_UP, 
    MOVE_DOWN 
} CameraMove;

//----------------------------------------------------------------------------------
// Global Variables Declaration
//----------------------------------------------------------------------------------
GLFWwindow *window;

static Matrix matProjection;                // Projection matrix to draw our world
static Matrix matModelview;                 // Modelview matrix to draw our world

static double currentTime, previousTime;    // Used to track timmings
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied

// LESSON 02: Keyboard/mouse input management
// Register keyboard/mouse states (current and previous)
static char previousKeyState[512] = { 0 };  // Registers previous frame key state
static char currentKeyState[512] = { 0 };   // Registers current frame key state
static char previousMouseState[3] = { 0 };  // Registers previous mouse button state
static char currentMouseState[3] = { 0 };   // Registers current mouse button state

// LESSON 03: Default texture (white) and shader
static Shader shdrDefault;                  // Default shader to draw (vertex and fragment processing)
static unsigned int quadId;                 // Quad VAO id to be used on texture drawing

// LESSON 06: Camera system management
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
static unsigned int LoadQuad(float width, float height); // Load quad vertex data and return id
static Shader LoadShaderDefault(void);              // Load default shader (basic shader)
static Image LoadImage(const char *fileName);       // Load image data to CPU memory (RAM)
static void UnloadImage(Image image);               // Unload image data from CPU memory (RAM)
static Color *GetImageData(Image image);            // Get pixel data from image as Color array
static Texture2D LoadTexture(unsigned char *data, int width, int height, int format);          // Load texture data in GPU memory (VRAM)
static void UnloadTexture(Texture2D texture);       // Unload texture data from GPU memory (VRAM)

static void DrawTexture(Texture2D texture, Vector2 position, Color tint);   // Draw texture in screen position coordinates

#define WHITE   (Color){ 255, 255, 255, 255 }

// LESSON 04: Model loading, vertex buffer creation
//----------------------------------------------------------------------------------
static Mesh LoadOBJ(const char *fileName);                  // Load static mesh from OBJ file
static void UploadMeshData(Mesh *mesh);                     // Upload mesh data into VRAM
static Model LoadModel(Mesh mesh, Texture2D diffuse);       // Load mesh data and texture into a 3d model
static void UnloadModel(Model model);                       // Unload model data from memory (RAM and VRAM)

static void DrawModel(Model model, Vector3 position, float scale, Color tint);  // Draw model in screen

// LESSON 05: Cubicmap generation, loading and drawing
//----------------------------------------------------------------------------------
static Mesh GenMeshCubicmap(Image cubicmap, float cubeSize); // Generate cubicmap mesh from image data

// LESSON 06: Camera system management (1st person)
//----------------------------------------------------------------------------------
static void UpdateCamera(Camera *camera);                   // Update camera for first person movement

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
    
    // LESSON 03: Init default Shader (customized for GL 3.3 and ES2)
    shdrDefault = LoadShaderDefault();

    // Define our camera
    Camera camera;
    camera.position = Vector3One();
    camera.target = Vector3Zero();
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    
    // LESSON 03: Calculate 2D projection matrix and model transform matrix
    //matProjection = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
    //matModelview = MatrixIdentity();

    // LESSON 04: Calculate 3D projection matrix (from perspective) and view matrix from camera look at
    matProjection = MatrixPerspective(camera.fovy*DEG2RAD, (double)screenWidth/(double)screenHeight, 0.01, 1000.0);
    matModelview = MatrixLookAt(camera.position, camera.target, camera.up);

    // LESSON 04: Load 3d model
    //Mesh mesh = LoadOBJ("resources/dwarf.obj");     // Load mesh data from OBJ file
    //UploadMeshData(&mesh);                          // Upload mesh data to GPU memory (VRAM)
    
    // LESSON 04: Load model diffuse texture
    //Image image = LoadImage("resources/dwarf_diffuse.png");
    //Texture2D texture = LoadTexture(image.data, image.width, image.height, image.format);
    //UnloadImage(image);
    
    // LESSON 05: Cubicmap generation
    Image imMap = LoadImage("resources/map04.png");
    Mesh mesh = GenMeshCubicmap(imMap, 1.0f);
    UploadMeshData(&mesh);
    UnloadImage(imMap);
    
    // LESSON 05: Load cubicmap texture
    Image image = LoadImage("resources/cubemap_atlas01.png");
    Texture2D texture = LoadTexture(image.data, image.width, image.height, image.format);
    UnloadImage(image);
    
    Model model = LoadModel(mesh, texture);
    
    Vector3 position = Vector3Zero();   // Model position on screen

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------    

    // Main game loop     
    while (!glfwWindowShouldClose(window))
    {
        // Update
        //----------------------------------------------------------------------------------
        Vector3 oldCamPos = camera.position;
        
        // LESSON 06: Camera update and modelview matrix update
        UpdateCamera(&camera);
        matModelview = MatrixLookAt(camera.position, camera.target, camera.up);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear used buffers: Color and Depth (Depth is used for 3D)
        
        // LESSON 03: Draw loaded texture on screen
        //DrawTexture(texture, position, WHITE);
        
        // LESSON 04: Draw loaded 3d model
        DrawModel(model, position, 1.0f, WHITE);
        
        glfwSwapBuffers(window);            // Swap buffers: show back buffer into front
        PollInputEvents();                  // Register input events (keyboard, mouse)
        SyncFrame();                        // Wait required time to target framerate
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(model);         // Unload model data (includes texture unloading)

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
    // LESSON 03: Unload default shader
    glUseProgram(0);
    glDeleteProgram(shdrDefault.id);

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
        shader.colorLoc = glGetUniformLocation(shader.id, "colDiffuse");
        shader.mapTextureLoc = glGetUniformLocation(shader.id, "texture0");
    }

    return shader;
}

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

// Unload texture data from GPU memory (VRAM)
static void UnloadTexture(Texture2D texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
}

// Draw texture in screen position coordinates
static void DrawTexture(Texture2D texture, Vector2 position, Color tint)
{
    glUseProgram(shdrDefault.id);

    // Define translation matrix to translate quad to screen center
    matModelview = MatrixTranslate(position.x, position.y, 0);

    // Create modelview-projection matrix
    Matrix matMVP = MatrixMultiply(matModelview, matProjection);

    glUniformMatrix4fv(shdrDefault.mvpLoc, 1, false, MatrixToFloat(matMVP));
    glUniform4f(shdrDefault.colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform1i(shdrDefault.mapTextureLoc, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glBindVertexArray(quadId);
                
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            
    glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
    glBindVertexArray(0);               // Unbind VAO
    glUseProgram(0);                    // Unbind shader program
}

// Load a quad to draw a texture
// NOTE: We need to define positions, coordinates and normals (optional)
static unsigned int LoadQuad(float width, float height)
{
    unsigned int quadVAO = 0;       // Quad VAO id
    unsigned int quadVBO = 0;       // Quad VBO id for individual buffer
    
    float vertices[] = {
        // Positions - Texture Coords
        0.0f,    0.0f, 0.0f,   0.0f, 0.0f,
        0.0f,  height, 0.0f,   0.0f, 1.0f,
        width,   0.0f, 0.0f,   1.0f, 0.0f,
        width, height, 0.0f,   1.0f, 1.0f,
    };

    // Generate quad VAO/VBO ids
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);

    // Fill VBO buffer
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));

    return quadVAO;
}
// LESSON 04: Model loading, vertex buffer creation
//----------------------------------------------------------------------------------
// Load static mesh from OBJ file (RAM)
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
                    Vector3 norm = Vector3CrossProduct(Vector3Subtract(midVertices[vCount[1]-1], midVertices[vCount[0]-1]), Vector3Subtract(midVertices[vCount[2]-1], midVertices[vCount[0]-1]));
                    Vector3Normalize(&norm);

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

// Load model (initialize)
// NOTE: Default material uses default shader and diffuse texture
static Model LoadModel(Mesh mesh, Texture2D diffuse)
{
    Model model = { 0 };
    
    model.mesh = mesh;
    model.material.shader = shdrDefault;
    model.material.texDiffuse = diffuse;
    model.transform = MatrixIdentity();
    
    return model;
}

// Unload model data from memory (RAM and VRAM)
// NOTE: Unloads Mesh data and Material shader
static void UnloadModel(Model model)
{
    // Unload mesh data
    if (model.mesh.vertices != NULL) free(model.mesh.vertices);
    if (model.mesh.texcoords != NULL) free(model.mesh.texcoords);
    if (model.mesh.normals != NULL) free(model.mesh.normals);

    if (model.mesh.vboId[0] != 0) glDeleteBuffers(1, &model.mesh.vboId[0]);   // vertex
    if (model.mesh.vboId[1] != 0) glDeleteBuffers(1, &model.mesh.vboId[1]);   // texcoords
    if (model.mesh.vboId[2] != 0) glDeleteBuffers(1, &model.mesh.vboId[2]);   // normals
    
    if (model.mesh.vaoId != 0) glDeleteVertexArrays(1, &model.mesh.vaoId);
    
    // Unload material texture
    // NOTE: Default shader is unloaded on CloseWindow()
    if (model.material.texDiffuse.id > 0) glDeleteTextures(1, &model.material.texDiffuse.id);
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
    
    model.material.colDiffuse = tint;           // Assign tint as diffuse color

    glUseProgram(model.material.shader.id);     // Bind material shader

    // Upload to shader material.colDiffuse
    glUniform4f(model.material.shader.colorLoc, 
                (float)model.material.colDiffuse.r/255, 
                (float)model.material.colDiffuse.g/255, 
                (float)model.material.colDiffuse.b/255, 
                (float)model.material.colDiffuse.a/255);

    // Set shader textures (diffuse, normal, specular)
    // NOTE: Diffuse texture fits in active texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model.material.texDiffuse.id);
    glUniform1i(model.material.shader.mapTextureLoc, 0);

    // Bind mesh VAO (vertex array objects)
    glBindVertexArray(model.mesh.vaoId);
    
    // Calculate model-view-matProjection matrix (MVP)
    Matrix matMVP = MatrixMultiply(matModelview, matProjection);    // Transform to screen-space coordinates

    // Send combined model-view-matProjection matrix to shader
    glUniformMatrix4fv(model.material.shader.mvpLoc, 1, false, MatrixToFloat(matMVP));

    // Draw call!
    glDrawArrays(GL_TRIANGLES, 0, model.mesh.vertexCount);

    glActiveTexture(GL_TEXTURE0);       // Set shader active texture to default 0
    glBindTexture(GL_TEXTURE_2D, 0);    // Unbind textures
    glBindVertexArray(0);               // Unbind VAO
    glUseProgram(0);                    // Unbind shader program
}

// LESSON 05: Cubicmap generation, loading and drawing
//----------------------------------------------------------------------------------
// Generate cubicmap mesh from image data
static Mesh GenMeshCubicmap(Image cubicmap, float cubeSize)
{
    Mesh mesh = { 0 };

    Color *cubicmapPixels = GetImageData(cubicmap);

    int mapWidth = cubicmap.width;
    int mapHeight = cubicmap.height;

    // NOTE: Max possible number of triangles numCubes * (12 triangles by cube)
    int maxTriangles = cubicmap.width*cubicmap.height*12;

    int vCounter = 0;       // Used to count vertices
    int tcCounter = 0;      // Used to count texcoords
    int nCounter = 0;       // Used to count normals

    float w = cubeSize;
    float h = cubeSize;
    float h2 = cubeSize;

    Vector3 *mapVertices = (Vector3 *)malloc(maxTriangles*3*sizeof(Vector3));
    Vector2 *mapTexcoords = (Vector2 *)malloc(maxTriangles*3*sizeof(Vector2));
    Vector3 *mapNormals = (Vector3 *)malloc(maxTriangles*3*sizeof(Vector3));

    // Define the 6 normals of the cube, we will combine them accordingly later...
    Vector3 n1 = { 1.0f, 0.0f, 0.0f };
    Vector3 n2 = { -1.0f, 0.0f, 0.0f };
    Vector3 n3 = { 0.0f, 1.0f, 0.0f };
    Vector3 n4 = { 0.0f, -1.0f, 0.0f };
    Vector3 n5 = { 0.0f, 0.0f, 1.0f };
    Vector3 n6 = { 0.0f, 0.0f, -1.0f };

    // NOTE: We use texture rectangles to define different textures for top-bottom-front-back-right-left (6)
    typedef struct RectangleF {
        float x;
        float y;
        float width;
        float height;
    } RectangleF;

    RectangleF rightTexUV = { 0.0f, 0.0f, 0.5f, 0.5f };
    RectangleF leftTexUV = { 0.5f, 0.0f, 0.5f, 0.5f };
    RectangleF frontTexUV = { 0.0f, 0.0f, 0.5f, 0.5f };
    RectangleF backTexUV = { 0.5f, 0.0f, 0.5f, 0.5f };
    RectangleF topTexUV = { 0.0f, 0.5f, 0.5f, 0.5f };
    RectangleF bottomTexUV = { 0.5f, 0.5f, 0.5f, 0.5f };

    for (int z = 0; z < mapHeight; ++z)
    {
        for (int x = 0; x < mapWidth; ++x)
        {
            // Define the 8 vertex of the cube, we will combine them accordingly later...
            Vector3 v1 = { w*(x - 0.5f), h2, h*(z - 0.5f) };
            Vector3 v2 = { w*(x - 0.5f), h2, h*(z + 0.5f) };
            Vector3 v3 = { w*(x + 0.5f), h2, h*(z + 0.5f) };
            Vector3 v4 = { w*(x + 0.5f), h2, h*(z - 0.5f) };
            Vector3 v5 = { w*(x + 0.5f), 0, h*(z - 0.5f) };
            Vector3 v6 = { w*(x - 0.5f), 0, h*(z - 0.5f) };
            Vector3 v7 = { w*(x - 0.5f), 0, h*(z + 0.5f) };
            Vector3 v8 = { w*(x + 0.5f), 0, h*(z + 0.5f) };

            // We check pixel color to be WHITE, we will full cubes
            if ((cubicmapPixels[z*cubicmap.width + x].r == 255) &&
                (cubicmapPixels[z*cubicmap.width + x].g == 255) &&
                (cubicmapPixels[z*cubicmap.width + x].b == 255))
            {
                // Define triangles (Checking Collateral Cubes!)
                //----------------------------------------------

                // Define top triangles (2 tris, 6 vertex --> v1-v2-v3, v1-v3-v4)
                mapVertices[vCounter] = v1;
                mapVertices[vCounter + 1] = v2;
                mapVertices[vCounter + 2] = v3;
                mapVertices[vCounter + 3] = v1;
                mapVertices[vCounter + 4] = v3;
                mapVertices[vCounter + 5] = v4;
                vCounter += 6;

                mapNormals[nCounter] = n3;
                mapNormals[nCounter + 1] = n3;
                mapNormals[nCounter + 2] = n3;
                mapNormals[nCounter + 3] = n3;
                mapNormals[nCounter + 4] = n3;
                mapNormals[nCounter + 5] = n3;
                nCounter += 6;

                mapTexcoords[tcCounter] = (Vector2){ topTexUV.x, topTexUV.y };
                mapTexcoords[tcCounter + 1] = (Vector2){ topTexUV.x, topTexUV.y + topTexUV.height };
                mapTexcoords[tcCounter + 2] = (Vector2){ topTexUV.x + topTexUV.width, topTexUV.y + topTexUV.height };
                mapTexcoords[tcCounter + 3] = (Vector2){ topTexUV.x, topTexUV.y };
                mapTexcoords[tcCounter + 4] = (Vector2){ topTexUV.x + topTexUV.width, topTexUV.y + topTexUV.height };
                mapTexcoords[tcCounter + 5] = (Vector2){ topTexUV.x + topTexUV.width, topTexUV.y };
                tcCounter += 6;

                // Define bottom triangles (2 tris, 6 vertex --> v6-v8-v7, v6-v5-v8)
                mapVertices[vCounter] = v6;
                mapVertices[vCounter + 1] = v8;
                mapVertices[vCounter + 2] = v7;
                mapVertices[vCounter + 3] = v6;
                mapVertices[vCounter + 4] = v5;
                mapVertices[vCounter + 5] = v8;
                vCounter += 6;

                mapNormals[nCounter] = n4;
                mapNormals[nCounter + 1] = n4;
                mapNormals[nCounter + 2] = n4;
                mapNormals[nCounter + 3] = n4;
                mapNormals[nCounter + 4] = n4;
                mapNormals[nCounter + 5] = n4;
                nCounter += 6;

                mapTexcoords[tcCounter] = (Vector2){ bottomTexUV.x + bottomTexUV.width, bottomTexUV.y };
                mapTexcoords[tcCounter + 1] = (Vector2){ bottomTexUV.x, bottomTexUV.y + bottomTexUV.height };
                mapTexcoords[tcCounter + 2] = (Vector2){ bottomTexUV.x + bottomTexUV.width, bottomTexUV.y + bottomTexUV.height };
                mapTexcoords[tcCounter + 3] = (Vector2){ bottomTexUV.x + bottomTexUV.width, bottomTexUV.y };
                mapTexcoords[tcCounter + 4] = (Vector2){ bottomTexUV.x, bottomTexUV.y };
                mapTexcoords[tcCounter + 5] = (Vector2){ bottomTexUV.x, bottomTexUV.y + bottomTexUV.height };
                tcCounter += 6;

                if (((z < cubicmap.height - 1) &&
                (cubicmapPixels[(z + 1)*cubicmap.width + x].r == 0) &&
                (cubicmapPixels[(z + 1)*cubicmap.width + x].g == 0) &&
                (cubicmapPixels[(z + 1)*cubicmap.width + x].b == 0)) || (z == cubicmap.height - 1))
                {
                    // Define front triangles (2 tris, 6 vertex) --> v2 v7 v3, v3 v7 v8
                    // NOTE: Collateral occluded faces are not generated
                    mapVertices[vCounter] = v2;
                    mapVertices[vCounter + 1] = v7;
                    mapVertices[vCounter + 2] = v3;
                    mapVertices[vCounter + 3] = v3;
                    mapVertices[vCounter + 4] = v7;
                    mapVertices[vCounter + 5] = v8;
                    vCounter += 6;

                    mapNormals[nCounter] = n6;
                    mapNormals[nCounter + 1] = n6;
                    mapNormals[nCounter + 2] = n6;
                    mapNormals[nCounter + 3] = n6;
                    mapNormals[nCounter + 4] = n6;
                    mapNormals[nCounter + 5] = n6;
                    nCounter += 6;

                    mapTexcoords[tcCounter] = (Vector2){ frontTexUV.x, frontTexUV.y };
                    mapTexcoords[tcCounter + 1] = (Vector2){ frontTexUV.x, frontTexUV.y + frontTexUV.height };
                    mapTexcoords[tcCounter + 2] = (Vector2){ frontTexUV.x + frontTexUV.width, frontTexUV.y };
                    mapTexcoords[tcCounter + 3] = (Vector2){ frontTexUV.x + frontTexUV.width, frontTexUV.y };
                    mapTexcoords[tcCounter + 4] = (Vector2){ frontTexUV.x, frontTexUV.y + frontTexUV.height };
                    mapTexcoords[tcCounter + 5] = (Vector2){ frontTexUV.x + frontTexUV.width, frontTexUV.y + frontTexUV.height };
                    tcCounter += 6;
                }

                if (((z > 0) &&
                (cubicmapPixels[(z - 1)*cubicmap.width + x].r == 0) &&
                (cubicmapPixels[(z - 1)*cubicmap.width + x].g == 0) &&
                (cubicmapPixels[(z - 1)*cubicmap.width + x].b == 0)) || (z == 0))
                {
                    // Define back triangles (2 tris, 6 vertex) --> v1 v5 v6, v1 v4 v5
                    // NOTE: Collateral occluded faces are not generated
                    mapVertices[vCounter] = v1;
                    mapVertices[vCounter + 1] = v5;
                    mapVertices[vCounter + 2] = v6;
                    mapVertices[vCounter + 3] = v1;
                    mapVertices[vCounter + 4] = v4;
                    mapVertices[vCounter + 5] = v5;
                    vCounter += 6;

                    mapNormals[nCounter] = n5;
                    mapNormals[nCounter + 1] = n5;
                    mapNormals[nCounter + 2] = n5;
                    mapNormals[nCounter + 3] = n5;
                    mapNormals[nCounter + 4] = n5;
                    mapNormals[nCounter + 5] = n5;
                    nCounter += 6;

                    mapTexcoords[tcCounter] = (Vector2){ backTexUV.x + backTexUV.width, backTexUV.y };
                    mapTexcoords[tcCounter + 1] = (Vector2){ backTexUV.x, backTexUV.y + backTexUV.height };
                    mapTexcoords[tcCounter + 2] = (Vector2){ backTexUV.x + backTexUV.width, backTexUV.y + backTexUV.height };
                    mapTexcoords[tcCounter + 3] = (Vector2){ backTexUV.x + backTexUV.width, backTexUV.y };
                    mapTexcoords[tcCounter + 4] = (Vector2){ backTexUV.x, backTexUV.y };
                    mapTexcoords[tcCounter + 5] = (Vector2){ backTexUV.x, backTexUV.y + backTexUV.height };
                    tcCounter += 6;
                }

                if (((x < cubicmap.width - 1) &&
                (cubicmapPixels[z*cubicmap.width + (x + 1)].r == 0) &&
                (cubicmapPixels[z*cubicmap.width + (x + 1)].g == 0) &&
                (cubicmapPixels[z*cubicmap.width + (x + 1)].b == 0)) || (x == cubicmap.width - 1))
                {
                    // Define right triangles (2 tris, 6 vertex) --> v3 v8 v4, v4 v8 v5
                    // NOTE: Collateral occluded faces are not generated
                    mapVertices[vCounter] = v3;
                    mapVertices[vCounter + 1] = v8;
                    mapVertices[vCounter + 2] = v4;
                    mapVertices[vCounter + 3] = v4;
                    mapVertices[vCounter + 4] = v8;
                    mapVertices[vCounter + 5] = v5;
                    vCounter += 6;

                    mapNormals[nCounter] = n1;
                    mapNormals[nCounter + 1] = n1;
                    mapNormals[nCounter + 2] = n1;
                    mapNormals[nCounter + 3] = n1;
                    mapNormals[nCounter + 4] = n1;
                    mapNormals[nCounter + 5] = n1;
                    nCounter += 6;

                    mapTexcoords[tcCounter] = (Vector2){ rightTexUV.x, rightTexUV.y };
                    mapTexcoords[tcCounter + 1] = (Vector2){ rightTexUV.x, rightTexUV.y + rightTexUV.height };
                    mapTexcoords[tcCounter + 2] = (Vector2){ rightTexUV.x + rightTexUV.width, rightTexUV.y };
                    mapTexcoords[tcCounter + 3] = (Vector2){ rightTexUV.x + rightTexUV.width, rightTexUV.y };
                    mapTexcoords[tcCounter + 4] = (Vector2){ rightTexUV.x, rightTexUV.y + rightTexUV.height };
                    mapTexcoords[tcCounter + 5] = (Vector2){ rightTexUV.x + rightTexUV.width, rightTexUV.y + rightTexUV.height };
                    tcCounter += 6;
                }

                if (((x > 0) &&
                (cubicmapPixels[z*cubicmap.width + (x - 1)].r == 0) &&
                (cubicmapPixels[z*cubicmap.width + (x - 1)].g == 0) &&
                (cubicmapPixels[z*cubicmap.width + (x - 1)].b == 0)) || (x == 0))
                {
                    // Define left triangles (2 tris, 6 vertex) --> v1 v7 v2, v1 v6 v7
                    // NOTE: Collateral occluded faces are not generated
                    mapVertices[vCounter] = v1;
                    mapVertices[vCounter + 1] = v7;
                    mapVertices[vCounter + 2] = v2;
                    mapVertices[vCounter + 3] = v1;
                    mapVertices[vCounter + 4] = v6;
                    mapVertices[vCounter + 5] = v7;
                    vCounter += 6;

                    mapNormals[nCounter] = n2;
                    mapNormals[nCounter + 1] = n2;
                    mapNormals[nCounter + 2] = n2;
                    mapNormals[nCounter + 3] = n2;
                    mapNormals[nCounter + 4] = n2;
                    mapNormals[nCounter + 5] = n2;
                    nCounter += 6;

                    mapTexcoords[tcCounter] = (Vector2){ leftTexUV.x, leftTexUV.y };
                    mapTexcoords[tcCounter + 1] = (Vector2){ leftTexUV.x + leftTexUV.width, leftTexUV.y + leftTexUV.height };
                    mapTexcoords[tcCounter + 2] = (Vector2){ leftTexUV.x + leftTexUV.width, leftTexUV.y };
                    mapTexcoords[tcCounter + 3] = (Vector2){ leftTexUV.x, leftTexUV.y };
                    mapTexcoords[tcCounter + 4] = (Vector2){ leftTexUV.x, leftTexUV.y + leftTexUV.height };
                    mapTexcoords[tcCounter + 5] = (Vector2){ leftTexUV.x + leftTexUV.width, leftTexUV.y + leftTexUV.height };
                    tcCounter += 6;
                }
            }
            // We check pixel color to be BLACK, we will only draw floor and roof
            else if  ((cubicmapPixels[z*cubicmap.width + x].r == 0) &&
                      (cubicmapPixels[z*cubicmap.width + x].g == 0) &&
                      (cubicmapPixels[z*cubicmap.width + x].b == 0))
            {
                // Define top triangles (2 tris, 6 vertex --> v1-v2-v3, v1-v3-v4)
                mapVertices[vCounter] = v1;
                mapVertices[vCounter + 1] = v3;
                mapVertices[vCounter + 2] = v2;
                mapVertices[vCounter + 3] = v1;
                mapVertices[vCounter + 4] = v4;
                mapVertices[vCounter + 5] = v3;
                vCounter += 6;

                mapNormals[nCounter] = n4;
                mapNormals[nCounter + 1] = n4;
                mapNormals[nCounter + 2] = n4;
                mapNormals[nCounter + 3] = n4;
                mapNormals[nCounter + 4] = n4;
                mapNormals[nCounter + 5] = n4;
                nCounter += 6;

                mapTexcoords[tcCounter] = (Vector2){ topTexUV.x, topTexUV.y };
                mapTexcoords[tcCounter + 1] = (Vector2){ topTexUV.x + topTexUV.width, topTexUV.y + topTexUV.height };
                mapTexcoords[tcCounter + 2] = (Vector2){ topTexUV.x, topTexUV.y + topTexUV.height };
                mapTexcoords[tcCounter + 3] = (Vector2){ topTexUV.x, topTexUV.y };
                mapTexcoords[tcCounter + 4] = (Vector2){ topTexUV.x + topTexUV.width, topTexUV.y };
                mapTexcoords[tcCounter + 5] = (Vector2){ topTexUV.x + topTexUV.width, topTexUV.y + topTexUV.height };
                tcCounter += 6;

                // Define bottom triangles (2 tris, 6 vertex --> v6-v8-v7, v6-v5-v8)
                mapVertices[vCounter] = v6;
                mapVertices[vCounter + 1] = v7;
                mapVertices[vCounter + 2] = v8;
                mapVertices[vCounter + 3] = v6;
                mapVertices[vCounter + 4] = v8;
                mapVertices[vCounter + 5] = v5;
                vCounter += 6;

                mapNormals[nCounter] = n3;
                mapNormals[nCounter + 1] = n3;
                mapNormals[nCounter + 2] = n3;
                mapNormals[nCounter + 3] = n3;
                mapNormals[nCounter + 4] = n3;
                mapNormals[nCounter + 5] = n3;
                nCounter += 6;

                mapTexcoords[tcCounter] = (Vector2){ bottomTexUV.x + bottomTexUV.width, bottomTexUV.y };
                mapTexcoords[tcCounter + 1] = (Vector2){ bottomTexUV.x + bottomTexUV.width, bottomTexUV.y + bottomTexUV.height };
                mapTexcoords[tcCounter + 2] = (Vector2){ bottomTexUV.x, bottomTexUV.y + bottomTexUV.height };
                mapTexcoords[tcCounter + 3] = (Vector2){ bottomTexUV.x + bottomTexUV.width, bottomTexUV.y };
                mapTexcoords[tcCounter + 4] = (Vector2){ bottomTexUV.x, bottomTexUV.y + bottomTexUV.height };
                mapTexcoords[tcCounter + 5] = (Vector2){ bottomTexUV.x, bottomTexUV.y };
                tcCounter += 6;
            }
        }
    }

    // Move data from mapVertices temp arays to vertices float array
    mesh.vertexCount = vCounter;

    mesh.vertices = (float *)malloc(mesh.vertexCount*3*sizeof(float));
    mesh.normals = (float *)malloc(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)malloc(mesh.vertexCount*2*sizeof(float));

    int fCounter = 0;

    // Move vertices data
    for (int i = 0; i < vCounter; i++)
    {
        mesh.vertices[fCounter] = mapVertices[i].x;
        mesh.vertices[fCounter + 1] = mapVertices[i].y;
        mesh.vertices[fCounter + 2] = mapVertices[i].z;
        fCounter += 3;
    }

    fCounter = 0;

    // Move normals data
    for (int i = 0; i < nCounter; i++)
    {
        mesh.normals[fCounter] = mapNormals[i].x;
        mesh.normals[fCounter + 1] = mapNormals[i].y;
        mesh.normals[fCounter + 2] = mapNormals[i].z;
        fCounter += 3;
    }

    fCounter = 0;

    // Move texcoords data
    for (int i = 0; i < tcCounter; i++)
    {
        mesh.texcoords[fCounter] = mapTexcoords[i].x;
        mesh.texcoords[fCounter + 1] = mapTexcoords[i].y;
        fCounter += 2;
    }

    free(mapVertices);
    free(mapNormals);
    free(mapTexcoords);

    free(cubicmapPixels);   // Free image pixel data
    
    TraceLog(LOG_INFO, "Mesh generated successfully (vertexCount: %i)", mesh.vertexCount);

    return mesh;
}

// LESSON 06: Camera system management (1st person)
//----------------------------------------------------------------------------------
static void UpdateCamera(Camera *camera)
{
    // Some useful defines
    #define PLAYER_MOVEMENT_SENSITIVITY                     20.0f
    #define CAMERA_MOUSE_MOVE_SENSITIVITY                   0.003f
    #define CAMERA_FIRST_PERSON_FOCUS_DISTANCE              25.0f
    #define CAMERA_FIRST_PERSON_MIN_CLAMP                   85.0f
    #define CAMERA_FIRST_PERSON_MAX_CLAMP                  -85.0f

    #define CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER  5.0f
    #define CAMERA_FIRST_PERSON_STEP_DIVIDER                30.0f
    #define CAMERA_FIRST_PERSON_WAVING_DIVIDER              200.0f
    
    static float playerEyesPosition = 0.6f;              // Default player eyes position from ground (in meters) 

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
