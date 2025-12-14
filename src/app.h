#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "graphics/renderer.h"
#include "ui/gui_layer.h"

class App {
public:
    App(const std::string& title, int width, int height);
    ~App();

    void run();

private:
    void init();
    void shutdown();
    void render();

    GLFWwindow* m_Window = nullptr;
    std::string m_Title;
    int m_Width;
    int m_Height;

    std::unique_ptr<Graphics::Renderer> m_Renderer;
    std::unique_ptr<UI::GuiLayer> m_GuiLayer;
};
