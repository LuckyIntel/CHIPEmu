/*
    We benefit from OpenGL 3.3 here, OpenGL 3.3
    context uses CHIP-8's VIDEO_BUFFER to generate
    a texture here. Instead of telling the CPU to
    make pixels hand by hand, which takes alot of time,
    we are using the textures to our advantage.

    OpenGL 3.3 does not have such features as AZDO(1)
    and DSA(2) so I haven't implemented specific optimizations.
    Since this is just a simple program that runs fine on most
    devices we do not need to push the GPU harder for more
    optimizations.

    1: Approaching Zero Driver Overhead
    2: Direct State Access

    Most of the stuff here are handled in initRenderer function.
    Since the job we are dealing is just a few textures no need
    to bother implementing complex features.

    I have tried to optimize some of the OpenGL codes here before
    committing to github. For example glClearColor is called once
    since I just need black background. Also got rid of the unnecessary
    CPU overheads that just binds VAO, screenTexture and sProg over
    and over. Using them once is fine as we are not changing our context
    anyways.

    Every OpenGL 3.3 code I wrote here is independent from the emulator codes.
    You can simply use this code anywhere else if necessary.

    RECT_VERTICES' texture coordinates have been swapped in the y-axises because
    the textures were loading upside down. Don't try to "fix" the code, it's the
    way it works.
*/

#ifndef RENDERER_H
#define RENDERER_H
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#define APP_NAME "CHIPEmu"
#define APP_MINIMUM_WIDTH 800
#define APP_MINIMUM_HEIGHT 600
#define DEFAULT_CYCLE_DELAY 3 // Default delay

static int APP_WIDTH = APP_MINIMUM_WIDTH;
static int APP_HEIGHT = APP_MINIMUM_HEIGHT;

static int CYCLE_DELAY = DEFAULT_CYCLE_DELAY; // Delay of rendering

static unsigned int screen_scale = 10;
static GLFWwindow* window; // Window Pointer
static unsigned int sProg; // Shader Program
static unsigned int VAO, VBO, EBO; // Vertex Arrays, Buffers and Element Buffer
static unsigned int screenTexture; // Screen Texture ( the texture that carries our CHIP-8 buffers )

const float RECT_VERTICES[] = { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f }; // Rectangle Vertices and Texture Coordinations
const unsigned int RECT_INDICES[] = { 0, 1, 3, 1, 2, 3 }; // Rectangle Indices

/*
    A set of stuff that executes when the GLFW window
    resizes. Not something we need to evaluate ourself.
*/
void onResize(GLFWwindow* w, int width, int height)
{
    APP_WIDTH = width; APP_HEIGHT = height;
    glViewport(0, 0, APP_WIDTH, APP_HEIGHT);
};

/*
    Sets up OpenGL 3.3 and GLFW for use.

    Returns 0 if fails to init.
    Returns 1 if works without any problems.
*/
int initRenderer()
{
    unsigned int SCR_W = 64 * screen_scale;
    unsigned int SCR_H = 32 * screen_scale;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0); // Disables antialiasing, to protect the original look.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, APP_NAME, 0, 0);
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, APP_MINIMUM_WIDTH, APP_MINIMUM_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSwapInterval(1);

    if (!gladLoadGL()) return 0; // Check if glad(OpenGL 3.3) loads.

    sProg = glCreateProgram();
    unsigned int VShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int FShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(VShader, 1, &vertex_shader_code, NULL);
    glShaderSource(FShader, 1, &fragment_shader_code, NULL);
    
    glCompileShader(VShader);
    glCompileShader(FShader);

    glAttachShader(sProg, VShader);
    glAttachShader(sProg, FShader);
    glLinkProgram(sProg);

    glDeleteShader(VShader);
    glDeleteShader(FShader);
    
    glUseProgram(sProg); // We only need this program so better use it all the time.

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO); // Bind once, I need this all the time.
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RECT_VERTICES), RECT_VERTICES, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RECT_INDICES), RECT_INDICES, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    /*
        No need to set screenTexture as our first texture, OpenGL
        drivers automatically use the first texture available.

        Minimized CPU overhead.
    */
    //glUniform1i(glGetUniformLocation(sProg, "screenTexture"), 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, APP_WIDTH, APP_HEIGHT);

    return 1;
};

/*
    Evaluates glfwSetKeyCallback
*/
void interceptKeys(GLFWkeyfun keyHandlerFunction) { glfwSetKeyCallback(window, keyHandlerFunction); };

/*
    Does the cleanup.
    Deletes VAO, VBO, EBO
    Destroys our Window
    Terminates the GLFW
*/
void terminateRenderer()
{
    glDeleteTextures(1, &screenTexture);
    
    glDeleteVertexArrays(1, &VAO);
    
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();
};

/*
    Checks if the context should render or not.

    RENDERING CAN BE STOPPED BY SIMPLY USER PROMPT
    TO CLOSE THE APP OR ANY OTHER APPLICATION SIDE
    ERRORS.
*/
int shouldRender() { return !glfwWindowShouldClose(window); };

/*
    Clears the screen.
*/
void rendererClearScreen() { glClear(GL_COLOR_BUFFER_BIT); };

/*
    Updates the screen.

    Listens to user based events like pressing the close button
    or pressing a keyboard key.
*/
void rendererEvents() { glfwSwapBuffers(window); glfwPollEvents(); };

/*
    Gets the current time, returns glfwGetTime()
*/
double getCurrentTime() { return glfwGetTime(); };

/*
    Submits our VIDEO_BUFFER from CHIP8 struct to here, creates an image
    then renders our quad with this image. Does this every time.
*/
void renderScreen(void const* VIDEO_BUFFER)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 32, GL_RGBA, GL_UNSIGNED_BYTE, VIDEO_BUFFER);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
};

#endif