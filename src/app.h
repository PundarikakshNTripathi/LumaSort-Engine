#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "graphics/renderer.h"
#include "graphics/canvas.h"
#include "ui/gui_layer.h"
#include "core/particle.h"
#include <vector>

#include <opencv2/opencv.hpp>

enum class InputMode {
    WEBCAM,
    IMAGE,
    CANVAS
};

/**
 * @class App
 * @brief The central backbone of the LumaSort Engine.
 * 
 * This class manages the application lifecycle, including:
 * - Window creation and context management (GLFW)
 * - The main game/application loop
 * - Dispatching render commands to subsystems
 * - Graceful shutdown and resource cleanup
 */
class App {
public:
    /**
     * @brief Constructs the App instance.
     * @param title Window title text.
     * @param width Initial window width.
     * @param height Initial window height.
     */
    App(const std::string& title, int width, int height);
    
    /** 
     * @brief Destructor ensures all subsystems (ImGui, GLFW) are shut down cleanly.
     */
    ~App();

    /**
     * @brief Starts the main loop.
     * 
     * This method blocks the calling thread until the application decides to exit
     * (e.g., user closes the window).
     */
    void run();

private:
    /** 
     * @brief Internal initialization routine to setup GLFW, Glad, and ImGui.
     * Called automatically by the constructor.
     */
    void init();

    /**
     * @brief Internal cleanup routine.
     * Called automatically by the destructor.
     */
    void shutdown();

    /**
     * @brief The per-frame render function.
     * Clears the screen and coordinates drawing between the 3D renderer and 2D UI layer.
     */
    void render();

    /**
     * @brief Updates application state (Input handling, sorting logic).
     */
    void update();

    /**
     * @brief Processes input based on current mode.
     */
    void processInput();

    // Window State
    GLFWwindow* m_Window = nullptr;
    std::string m_Title;
    int m_Width;
    int m_Height;

    // Subsystems
    // Using unique_ptr for strict ownership - the App owns these systems explicitly.
    std::unique_ptr<Graphics::Renderer> m_Renderer;
    std::unique_ptr<UI::GuiLayer> m_GuiLayer;
    std::unique_ptr<Canvas> m_Canvas;

    // Input State
    InputMode m_InputMode = InputMode::WEBCAM;
    cv::VideoCapture m_Webcam;
    cv::Mat m_CurrentFrame;
    cv::Mat m_StaticImage; // Loaded image

    // Particle System
    std::vector<Particle> m_Particles;
    float m_Time = 0.0f;
};
