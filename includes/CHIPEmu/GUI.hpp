/*
    This is a simple wrapper for dear imgui.

    This file is dependent to CPU.h and Renderer.h
    (GLFW + OpenGL 3.3) wrapper I made because
    it needs to access window and other stuff.

    Since loading a ROM required a file picker
    and writing a cross platform one from scratch
    is quite time consuming, I decided to use
    portable-file-dialogs instead.
*/
#ifndef GUI_H
#define GUI_H
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <PortableFileDialogs/portable-file-dialogs.h>
#include <algorithm>
#include <string>
#include "CPU.h" // CHIP-8 Code (we require loadFromCH8File function)
#include "Renderer.h" // OpenGL 3.3 + GLFW (necessary for window)
#define IMGUI_WINDOW_WIDTH 250.0f

static ImGuiViewport* viewport;
static int GUI_SHOULD_RENDER = 1;

static pfd::open_file* PFD_SESSION = nullptr;

const ImGuiWindowFlags window_specs = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus;

/*
    Initializes dear imgui and
    sets up dear imgui related
    variables.
*/
void initGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr; // Disables imugi.ini

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // GLSL 330 Core

    viewport = ImGui::GetMainViewport();
};

/*
    Sets up GUI elements for the next render call.
*/
void setGUI()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    if (GUI_SHOULD_RENDER) return;

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(ImVec2(IMGUI_WINDOW_WIDTH, viewport->Size.y));
    if (ImGui::Begin("CHIPEmu Properties", 0, window_specs))
    {
        ImGui::Text("CHIPEmu - A CHIP-8 Emulator");
        if (ImGui::Button("Load a ROM") && PFD_SESSION == nullptr)
        {
            PFD_SESSION = new pfd::open_file(
                "Select a ROM file.",
                ".",
                {"CHIP-8 Files (.ch8)", "*.ch8"}
            );
        }
        ImGui::SliderInt("CycleDelay", &CYCLE_DELAY, 1, 8, "Cycle Delay: %d");
        
        if (PFD_SESSION && PFD_SESSION->ready())
        {
            std::vector<std::string> res = PFD_SESSION->result();
            //printf("%s", res[0].c_str());

            if (!res.empty()) { initCHIP8(); loadFromCH8File(res[0].c_str()); };
                
            delete PFD_SESSION;
            PFD_SESSION = nullptr;
        };
        ImGui::End();
    };
};

/*
    Renders the GUI elements that have been set up
    before.
*/
void renderGUI()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
};

#endif