#include "UI.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>

namespace UIProcessor {
    std::map<char, std::vector<KeyCodeActor>>* keyCodeMap = nullptr;

    char inputKeycode[1] = "";
    char inputActorName[128] = "";
    int inputNum = 1;
    bool inputActorRandomized = false;
    bool inputWeaponsRandomized = false;
}

void ShowMenu(GLFWwindow* Window)
{
    glfwSetWindowAttrib(Window, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
}

void HideMenu(GLFWwindow* Window)
{
    glfwSetWindowAttrib(Window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
}


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


/// <summary>
/// This is the actual ImGui drawing code
/// </summary>
void DrawItems() {
    
    ImGui::Begin("Actor-Keycode Mapping");

    
    ImGui::InputTextWithHint("Keycode", "Keycode", UIProcessor::inputKeycode, 2); // Idk why 2 instead of 1, but ensures that only one char can be entered
    ImGui::InputTextWithHint("Actor Name", "Actor Name", UIProcessor::inputActorName, IM_ARRAYSIZE(UIProcessor::inputActorName));
    ImGui::SliderInt("Num", &UIProcessor::inputNum, 1, 100);
    ImGui::Checkbox("Variant Randomized", &UIProcessor::inputActorRandomized);
    ImGui::Checkbox("Weapons Randomized", &UIProcessor::inputWeaponsRandomized);
    if (ImGui::Button("Add Actor") && std::strcmp(UIProcessor::inputKeycode, "") && std::strcmp(UIProcessor::inputActorName, "")) {
        std::vector<KeyCodeActor> actVec;
        actVec.push_back(KeyCodeActor(UIProcessor::inputActorName, UIProcessor::inputNum, UIProcessor::inputActorRandomized, UIProcessor::inputWeaponsRandomized));
        if (UIProcessor::keyCodeMap->count(std::toupper(UIProcessor::inputKeycode[0])) == 0) {
            UIProcessor::keyCodeMap->insert({ std::toupper(UIProcessor::inputKeycode[0]), actVec });
        }
        else {
            std::vector<KeyCodeActor>* curentActVec = &UIProcessor::keyCodeMap->at(std::toupper(UIProcessor::inputKeycode[0]));
            
            curentActVec->insert(curentActVec->end(), actVec.begin(), actVec.end());
        }
    }

    std::vector<char> keycodesToRemove;
    for (std::pair<char, std::vector<KeyCodeActor>> pair : *UIProcessor::keyCodeMap) {
        std::string key(1, pair.first);

        ImGui::Text(key.c_str());

        ImGui::Indent(25);

        std::vector<int> actIndicesToRemove;
        unsigned int actorIdx = 0;
        for (KeyCodeActor act : pair.second) {
            if (ImGui::Button(("Remove##" + key + std::to_string(actorIdx)).c_str())) {
                actIndicesToRemove.push_back(actorIdx);
            }

            ImGui::SameLine();

            if (ImGui::BeginTable((key + act.Name).c_str(), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Actor Name"); ImGui::TableSetColumnIndex(1); ImGui::Text(act.Name.c_str());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Num"); ImGui::TableSetColumnIndex(1); ImGui::Text(std::to_string(act.Num).c_str());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Variant Randomized"); ImGui::TableSetColumnIndex(1); ImGui::Text(std::to_string(act.ActorRandomized).c_str());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Weapons Randomized"); ImGui::TableSetColumnIndex(1); ImGui::Text(std::to_string(act.WeaponsRandomized).c_str());
                
                ImGui::EndTable();
            }
            actorIdx++;
        }
        for (int idx : actIndicesToRemove) // Apply actor deletions
            UIProcessor::keyCodeMap->at(pair.first).erase(UIProcessor::keyCodeMap->at(pair.first).begin() + idx);
        if (pair.second.size() < 1)
            keycodesToRemove.push_back(pair.first);

        ImGui::Unindent();
    }

    ImGui::End();

#ifndef NDEBUG
    ImGui::ShowDemoWindow();
#endif
    for (char keycode : keycodesToRemove) // Apply keycode deletions
        UIProcessor::keyCodeMap->erase(keycode);
}

namespace Threads {
    DWORD WINAPI UIThread(LPVOID param)
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return 1;

        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        GLFWmonitor* Monitor = glfwGetPrimaryMonitor();
        if (!Monitor)
        {
            return 0;
        }

        int Width = glfwGetVideoMode(Monitor)->width;
        int Height = glfwGetVideoMode(Monitor)->height;

        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        //glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

        // Create window with graphics context
        GLFWwindow* Window = glfwCreateWindow(1, 1, "BotwEdit", NULL, NULL);
        if (Window == NULL)
            return 1;

        glfwSetWindowAttrib(Window, GLFW_DECORATED, GLFW_FALSE);
        //glfwSetWindowAttrib(Window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);

        glfwMakeContextCurrent(Window);
        glfwSwapInterval(1); // Enable vsync

        if (glewInit() != GLEW_OK)
        {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.IniFilename = NULL; // Let's not save configs... we don't wanna pollute the cemu directory. Maybe once we can get the dll directory we can put it there.

        io.WantCaptureMouse = false;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(Window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        bool bMenuVisible = true;

        float worldSpeedSliderValue = 1;
        float linkSpeedSliderValue = 1;



        // Main loop
        while (!glfwWindowShouldClose(Window))
        {
            glfwPollEvents();

            // Start the ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (GetAsyncKeyState(VK_OEM_3) & 1)
            {
                bMenuVisible = !bMenuVisible;
                if (bMenuVisible)
                {
                    //ShowMenu(Window);
                }
                else
                {
                    //HideMenu(Window);
                }
            }

            DrawItems();


            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(Window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(Window);
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(Window);
        glfwTerminate();

        return 0;
    }
}
