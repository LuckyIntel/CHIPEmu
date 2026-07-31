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

    I have added Joystick support thanks to
    GLFW3. However only maximum of 1 controller
    is being supported, you can't plug in an
    another controller and expect to play it
    with your friends and etc.
*/

#include "includes/CHIPEmu/CPU.h" // CHIP-8 Code are in this file.
#include "includes/CHIPEmu/Audio.h" // A basic miniaudio.h wrapper of mine.
#include "includes/CHIPEmu/GUI.hpp" // dear imgui wrapper.
#include "includes/CHIPEmu/Renderer.h" // GLFW + OpenGL 3.3 wrapper, simple yet works.

int Joystick = -1;
int JoystickHatsCount = 0;
const unsigned char* JoystickHats = NULL;

/*
    This is gonna be handled by our renderer's
    interceptKeys function that relies on glfwSetKeyCallback
*/
void KEYBOARD_HANDLER(GLFWwindow* w, int key, int scancode, int action, int mods);

/*
    This is also being handled by the interceptKeys function
    relying on the glfwSetJosytickCallback
*/
void JOYSTICK_LISTENER(int jid, int event);

/*
    Works mostly the same as KEYBOARD_HANDLER
*/
void JOYSTICK_HANDLER();

int main()
{
    initCHIP8();
    initAudio();
    initRenderer();
    initGUI();
    interceptKeys(KEYBOARD_HANDLER, JOYSTICK_LISTENER);

    if (glfwJoystickPresent(0)) JOYSTICK_LISTENER(0, GLFW_CONNECTED);

    double lastTime = getCurrentTime();
    double currentTime;
    double deltaTime;
    double acc;
    double timerAcc;
    float target;

    while (shouldRender())
    {
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
        JOYSTICK_HANDLER();
        rendererEvents();
    }

    terminateAudio();
    terminateRenderer();
}

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
    }
}

void JOYSTICK_LISTENER(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        Joystick = jid;
        JoystickHats = glfwGetJoystickHats(Joystick, &JoystickHatsCount);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        Joystick = -1;
    }
}

void JOYSTICK_HANDLER()
{
    if (Joystick == -1) return;
    if (!glfwJoystickIsGamepad(Joystick)) return;
    if (JoystickHatsCount == 0) return;

    GLFWgamepadstate state;
    if (glfwGetGamepadState(Joystick, &state))
    {
        CHIP8.keypad[4] = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.5f) ? 1 : 0;
        CHIP8.keypad[6] = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.5f) ? 1 : 0;
    }
}