#include "UI.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>

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
    char  keycode[128] = "";
    static int currentActor = 0;
    const char* actors[] = {"Enemy_Guardian_A", "Enemy_Guardian_B", "Enemy_Guardian_C"};
    ImGui::Begin("Test Window");
    ImGui::Text("Actor-Keycode Mapping");
    ImGui::InputTextWithHint("Keycode", "Keycode", keycode, IM_ARRAYSIZE(keycode));
    ImGui::Combo("Actors", &currentActor, actors, IM_ARRAYSIZE(actors));
    ImGui::Text(keycode);
    ImGui::Text(actors[currentActor]);
    ImGui::End();

    ImGui::ShowDemoWindow();
}

namespace Threads {
    DWORD WINAPI Threads::UIThread(LPVOID param)
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