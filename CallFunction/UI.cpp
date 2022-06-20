#include "UI.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace UIProcessor {
    std::map<char, std::vector<TriggeredActor>>* keyCodeMap = nullptr;
    std::vector<TriggeredActor>* damageActors = nullptr;
    std::map<char, bool>* prevKeyStateMap = nullptr;

    char keycodeActor_inputKeycode[2] = "";
    char keycodeActor_inputActorName[128] = "";
    int keycodeActor_inputNum = 1;
    bool keycodeActor_inputActorRandomized = false;
    bool keycodeActor_inputWeaponsRandomized = false;

    char damageActor_inputActorName[128] = "";
    int damageActor_inputNum = 1;
    bool damageActor_inputActorRandomized = false;
    bool damageActor_inputWeaponsRandomized = false;

    char savePath[128] = "";
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


void DrawKeycodeActorWindow() {
    ImGui::Begin("Actor-Keycode Mapping");


    ImGui::InputTextWithHint("Keycode##keycodeActor", "Keycode", UIProcessor::keycodeActor_inputKeycode, 2); // Idk why 2 instead of 1, but ensures that only one char can be entered
    ImGui::InputTextWithHint("Actor Name##keycodeActor", "Actor Name", UIProcessor::keycodeActor_inputActorName, IM_ARRAYSIZE(UIProcessor::keycodeActor_inputActorName));
    ImGui::SliderInt("Num##keycodeActor", &UIProcessor::keycodeActor_inputNum, 1, 100);
    ImGui::Checkbox("Variant Randomized##keycodeActor", &UIProcessor::keycodeActor_inputActorRandomized);
    ImGui::Checkbox("Weapons Randomized##keycodeActor", &UIProcessor::keycodeActor_inputWeaponsRandomized);
    if (ImGui::Button("Add Actor##keycodeActor") && std::strcmp(UIProcessor::keycodeActor_inputKeycode, "") && std::strcmp(UIProcessor::keycodeActor_inputActorName, "")) {
        std::vector<TriggeredActor> actVec;
        actVec.push_back(TriggeredActor(UIProcessor::keycodeActor_inputActorName, UIProcessor::keycodeActor_inputNum, UIProcessor::keycodeActor_inputActorRandomized, UIProcessor::keycodeActor_inputWeaponsRandomized));
        if (UIProcessor::keyCodeMap->count(std::toupper(UIProcessor::keycodeActor_inputKeycode[0])) == 0) {
            UIProcessor::keyCodeMap->insert({ std::toupper(UIProcessor::keycodeActor_inputKeycode[0]), actVec });
            UIProcessor::prevKeyStateMap->emplace(std::toupper(UIProcessor::keycodeActor_inputKeycode[0]), false);
        }
        else {
            std::vector<TriggeredActor>* curentActVec = &UIProcessor::keyCodeMap->at(std::toupper(UIProcessor::keycodeActor_inputKeycode[0]));

            curentActVec->insert(curentActVec->end(), actVec.begin(), actVec.end());
        }
    }

    std::vector<char> keycodesToRemove;
    for (std::pair<char, std::vector<TriggeredActor>> pair : *UIProcessor::keyCodeMap) {
        std::string key(1, pair.first);

        ImGui::Text(key.c_str());

        ImGui::Indent(25);

        std::vector<int> actIndicesToRemove;
        unsigned int actorIdx = 0;
        for (TriggeredActor act : pair.second) {
            if (ImGui::Button(("Remove##keyCodeActor" + key + std::to_string(actorIdx)).c_str())) {
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

    for (char keycode : keycodesToRemove) // Apply keycode deletions
        UIProcessor::keyCodeMap->erase(keycode);

    ImGui::End();
}

void DrawDamageActorWindow() {
    ImGui::Begin("Actor-Damage Mapping");

    ImGui::InputTextWithHint("Actor Name##damageActor", "Actor Name", UIProcessor::damageActor_inputActorName, IM_ARRAYSIZE(UIProcessor::keycodeActor_inputActorName));
    ImGui::SliderInt("Num##damageActor", &UIProcessor::damageActor_inputNum, 1, 100);
    ImGui::Checkbox("Variant Randomized##damageActor", &UIProcessor::damageActor_inputActorRandomized);
    ImGui::Checkbox("Weapons Randomized##damageActor", &UIProcessor::damageActor_inputWeaponsRandomized);
    if (ImGui::Button("Add Actor##damageActor")) {
        UIProcessor::damageActors->push_back(TriggeredActor(UIProcessor::damageActor_inputActorName, UIProcessor::damageActor_inputNum, UIProcessor::damageActor_inputActorRandomized, UIProcessor::damageActor_inputWeaponsRandomized));
    }

    ImGui::Indent(25);

    std::vector<int> actIndicesToRemove;
    unsigned int actorIdx = 0;
    for (TriggeredActor act : *UIProcessor::damageActors) {
        if (ImGui::Button(("Remove##damageActor_" + std::to_string(actorIdx)).c_str())) {
            actIndicesToRemove.push_back(actorIdx);
        }


        ImGui::SameLine();

        if (ImGui::BeginTable(("damage_Actor_" + act.Name).c_str(), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
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
        UIProcessor::damageActors->erase(UIProcessor::damageActors->begin() + idx);

    ImGui::Unindent();

    ImGui::End();
}

void DrawLoadSaveWindow() {
    ImGui::Begin("Save Stuff");

    ImGui::InputText("Path", UIProcessor::savePath, 128);

    if (ImGui::Button("Save")) {
        UIProcessor::WriteSettings();
    }
    if (ImGui::Button("Load")) {
        UIProcessor::LoadSettings();
    }

    ImGui::End();
}

/// <summary>
/// This is the actual ImGui drawing code
/// </summary>
void DrawItems() {

    DrawKeycodeActorWindow();
    DrawDamageActorWindow();
    DrawLoadSaveWindow();
    

#ifndef NDEBUG
    ImGui::ShowDemoWindow();
#endif
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

namespace UIProcessor {
    void WriteSettings()
    {
        nlohmann::json saved;

        // Save keycode actors
        for (std::map<char, std::vector<TriggeredActor>>::iterator iter = keyCodeMap->begin(); iter != keyCodeMap->end(); iter++)
        {
            nlohmann::json keyCodeMapping;
            for (int i = 0; i < iter->second.size(); i++)
            {
                keyCodeMapping.push_back({ 
                    { "Name", iter->second[i].Name }, 
                    { "Num", iter->second[i].Num }, 
                    { "ActorRandomized", iter->second[i].ActorRandomized }, 
                    { "WeaponsRandomized", iter->second[i].WeaponsRandomized } 
                    });
            }

            saved["KeyCodeActors"][std::string(1, iter->first)] = keyCodeMapping;
        }
        // Save damage actors
        for (int i = 0; i < damageActors->size(); i++) {
            saved["DamageActors"].push_back({
                { "Name", (*damageActors)[i].Name},
                { "Num", (*damageActors)[i].Num },
                { "ActorRandomized", (*damageActors)[i].ActorRandomized },
                { "WeaponsRandomized", (*damageActors)[i].WeaponsRandomized }
                });
        }

        // Write file
        std::ofstream outStream(savePath);
        outStream << std::setw(4) << saved << std::endl;
    }
    void LoadSettings() {
        if (!std::filesystem::exists(savePath))
            return;
        std::ifstream inStream(savePath);
        nlohmann::json loaded;
        inStream >> loaded;

        keyCodeMap->clear();
        for (nlohmann::json::iterator keycodeIter = loaded["KeyCodeActors"].begin(); keycodeIter != loaded["KeyCodeActors"].end(); keycodeIter++) {
            std::vector<TriggeredActor> actors;
            for (nlohmann::json::iterator actorIter = keycodeIter.value().begin(); actorIter != keycodeIter.value().end(); actorIter++) {
                actors.push_back(TriggeredActor(actorIter.value()["Name"], actorIter.value()["Num"], actorIter.value()["ActorRandomized"], actorIter.value()["WeaponsRandomized"]));
            }
            keyCodeMap->insert(std::pair(keycodeIter.key()[0], actors));
            prevKeyStateMap->emplace(keycodeIter.key()[0], false);
        }

        damageActors->clear();
        for (nlohmann::json::iterator actorIter = loaded["DamageActors"].begin(); actorIter != loaded["DamageActors"].end(); actorIter++) {
            damageActors->push_back(TriggeredActor(actorIter.value()["Name"], actorIter.value()["Num"], actorIter.value()["ActorRandomized"], actorIter.value()["WeaponsRandomized"]));
        }
    }
}
