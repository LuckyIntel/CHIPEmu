/*
    Main file of our CPP program. To make
    it look a little bit beautiful we are
    just using our own wrappers here.
    
    However the point here is NOT making
    this thing look good, we are mostly
    trying to make an optimized app here.

    This file WAS actually named main.c
    and was being compiled in GCC. I decided
    to make it main.cpp after deciding to
    include dear imgui to the project.

    You can use this as just a main.c file
    if you get rid of the dear imgui wrapper
    (GUI.hpp) and manually load ROMs with the
    loadFromCH8File function.
*/

#include "includes/CHIPEmu/CPU.h" // CHIP-8 Code are in this file.
#include "includes/CHIPEmu/Audio.h" // A basic miniaudio.h wrapper of mine.
#include "includes/CHIPEmu/GUI.hpp" // dear imgui wrapper.
#include "includes/CHIPEmu/Renderer.h" // GLFW + OpenGL 3.3 wrapper, simple yet works.

/*
    This is gonna be handled by our renderer's
    interceptKeys function that relies on glfwSetKeyCallback
*/
void KEYBOARD_HANDLER(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { GUI_SHOULD_RENDER = !GUI_SHOULD_RENDER; };
    
    int cIF = 0; // Checks key state. 1 if pressed or holding, 0 if released.
    if (action == GLFW_PRESS || action == GLFW_REPEAT) cIF = 1;
    else if (action == GLFW_RELEASE) cIF = 0;

    switch (key)
    {
        case GLFW_KEY_X: CHIP8.keypad[0] = cIF; break;
        case GLFW_KEY_1: CHIP8.keypad[1] = cIF; break;
        case GLFW_KEY_2: CHIP8.keypad[2] = cIF; break;
        case GLFW_KEY_3: CHIP8.keypad[3] = cIF; break;
        case GLFW_KEY_Q: CHIP8.keypad[4] = cIF; break;
        case GLFW_KEY_W: CHIP8.keypad[5] = cIF; break;
        case GLFW_KEY_E: CHIP8.keypad[6] = cIF; break;
        case GLFW_KEY_A: CHIP8.keypad[7] = cIF; break;
        case GLFW_KEY_S: CHIP8.keypad[8] = cIF; break;
        case GLFW_KEY_D: CHIP8.keypad[9] = cIF; break;
        case GLFW_KEY_Z: CHIP8.keypad[0xA] = cIF; break;
        case GLFW_KEY_C: CHIP8.keypad[0xB] = cIF; break;
        case GLFW_KEY_4: CHIP8.keypad[0xC] = cIF; break;
        case GLFW_KEY_R: CHIP8.keypad[0xD] = cIF; break;
        case GLFW_KEY_F: CHIP8.keypad[0xE] = cIF; break;
        case GLFW_KEY_V: CHIP8.keypad[0xF] = cIF; break; 
        default: break;
    };
};

int main()
{
    initCHIP8();
    initAudio();
    initRenderer();
    initGUI();
    interceptKeys(KEYBOARD_HANDLER);

    //loadFromCH8File("Cave.ch8");
    //loadFromCH8File("Soccer.ch8");
    //loadFromCH8File("Breakout.ch8");

    double lastTime = getCurrentTime();
    double currentTime;
    double deltaTime;
    double acc;
    double timerAcc;
    float target;

    while (shouldRender())
    {
        //printf("Cycle Delay: %d\n", CYCLE_DELAY);
        currentTime = getCurrentTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        acc += deltaTime;
        timerAcc += deltaTime;
        target = CYCLE_DELAY * DELAY_FACTOR;
        
        while (acc >= target) { CHIP8CYCLE(); acc -= target; };
        while (timerAcc >= 0.016666668f) { CHIP8TIMERCYCLE(); timerAcc -= 0.016666668f; };

        setGUI();
        rendererClearScreen();
        renderScreen(CHIP8.VIDEO_BUFFER);
        renderGUI();
        rendererEvents();
    };

    terminateAudio();
    terminateRenderer();
};