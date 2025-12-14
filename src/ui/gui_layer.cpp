#include "gui_layer.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace UI {

    GuiLayer::GuiLayer() {
        // Context is initialized in App::init usually.
    }

    GuiLayer::~GuiLayer() {
    }

    void GuiLayer::begin() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GuiLayer::render() {
        // Render the generic demo window for now.
        // Eventually, this will be replaced with our own "LumaSort Settings" panel.
        ImGui::ShowDemoWindow();
    }

    void GuiLayer::end() {
        // Assemble draw data
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Handle Multi-Viewport / Docking features
        // This splits ImGui windows into their own OS windows if dragged outside the main viewport.
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

}
