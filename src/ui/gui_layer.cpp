#include "../app.h"
#include "gui_layer.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nfd.h>

namespace UI {

    GuiLayer::GuiLayer() {
        // Initialize Native File Dialog
        NFD_Init();
    }

    GuiLayer::~GuiLayer() {
        // Cleanup Native File Dialog
        NFD_Quit();
    }

    void GuiLayer::begin() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GuiLayer::render(App* app) {
        ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin("LumaSort Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Input Control");
        ImGui::Separator();
        
        const char* items[] = { "Webcam", "Image", "Canvas" };
        int currentItem = static_cast<int>(app->m_InputMode);
        if (ImGui::Combo("Input Mode", &currentItem, items, 3)) {
            app->m_InputMode = static_cast<InputMode>(currentItem);
        }

        if (app->m_InputMode == InputMode::IMAGE) {
            ImGui::Spacing();
            if (ImGui::Button("Load Source Image", ImVec2(-1, 0))) {
                nfdchar_t *outPath = nullptr;
                nfdfilteritem_t filters[1] = { { "Image Files", "jpg,png,bmp,jpeg" } };
                nfdresult_t result = NFD_OpenDialog(&outPath, filters, 1, nullptr);
                if (result == NFD_OKAY) {
                    app->loadSourceImage(std::string(outPath));
                    NFD_FreePath(outPath);
                }
            }
        } else if (app->m_InputMode == InputMode::CANVAS) {
            ImGui::Spacing();
            
            // Drawing Tools
            ImGui::Text("Tools:");
            ImGui::SameLine();
            if (ImGui::RadioButton("Pen", app->m_DrawTool == App::DrawTool::PEN)) {
                app->m_DrawTool = App::DrawTool::PEN;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Eraser", app->m_DrawTool == App::DrawTool::ERASER)) {
                app->m_DrawTool = App::DrawTool::ERASER;
            }
            
            // Brush Size
            ImGui::SliderFloat("Brush Size", &app->m_BrushSize, 1.0f, 20.0f);
            
            // VIBGYOR Color Palette
            ImGui::Text("Colors (VIBGYOR):");
            const glm::vec3 vibgyor[] = {
                glm::vec3(0.56f, 0.0f, 1.0f),   // Violet
                glm::vec3(0.29f, 0.0f, 0.51f),  // Indigo  
                glm::vec3(0.0f, 0.0f, 1.0f),    // Blue
                glm::vec3(0.0f, 1.0f, 0.0f),    // Green
                glm::vec3(1.0f, 1.0f, 0.0f),    // Yellow
                glm::vec3(1.0f, 0.5f, 0.0f),    // Orange
                glm::vec3(1.0f, 0.0f, 0.0f),    // Red
                glm::vec3(1.0f, 1.0f, 1.0f),    // White
                glm::vec3(0.0f, 0.0f, 0.0f)     // Black
            };
            const char* colorNames[] = { "V", "I", "B", "G", "Y", "O", "R", "W", "K" };
            
            for (int i = 0; i < 9; ++i) {
                ImVec4 col(vibgyor[i].r, vibgyor[i].g, vibgyor[i].b, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Button, col);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(col.x * 0.8f, col.y * 0.8f, col.z * 0.8f, 1.0f));
                if (ImGui::Button(colorNames[i], ImVec2(30, 30))) {
                    app->m_DrawColor = vibgyor[i];
                }
                ImGui::PopStyleColor(2);
                if (i < 8) ImGui::SameLine();
            }
            
            ImGui::Spacing();
            if (ImGui::Button("Clear Canvas", ImVec2(-1, 0))) {
                app->clearCanvas();
            }
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Target Control");
        ImGui::Separator();
        
        if (ImGui::Button("Load Target Image", ImVec2(-1, 0))) {
            nfdchar_t *outPath = nullptr;
            nfdfilteritem_t filters[1] = { { "Image Files", "jpg,png,bmp,jpeg" } };
            nfdresult_t result = NFD_OpenDialog(&outPath, filters, 1, nullptr);
            if (result == NFD_OKAY) {
                app->loadTargetImage(std::string(outPath));
                NFD_FreePath(outPath);
            }
        }

        // Preview Target
        if (app->m_TargetPreview && app->m_TargetPreview->getID() != 0) {
            ImGui::Spacing();
            ImGui::Text("Target Preview:");
            float width = 200.0f;
            float aspect = (float)app->m_TargetPreview->getHeight() / (float)app->m_TargetPreview->getWidth();
            ImGui::Image((ImTextureID)(intptr_t)app->m_TargetPreview->getID(), ImVec2(width, width * aspect));
        }

        // Transform Control Section
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Transform");
        ImGui::Separator();
        
        if (!app->isTransforming()) {
            if (ImGui::Button("Start Transform", ImVec2(-1, 40))) {
                app->startTransform();
            }
        } else {
            if (ImGui::Button("Stop Transform", ImVec2(-1, 40))) {
                app->stopTransform();
            }
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Physics Parameters");
        ImGui::Separator();
        
        ImGui::SliderFloat("Particle Speed", &app->m_ParticleSpeed, 0.001f, 0.1f);
        ImGui::SliderFloat("Flow Strength", &app->m_FlowStrength, 0.0f, 0.001f, "%.5f");
        ImGui::SliderFloat("Noise Scale", &app->m_NoiseScale, 1.0f, 20.0f);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Particles: %zu", app->m_Particles.size());
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        ImGui::End();
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
