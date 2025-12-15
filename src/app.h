#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "graphics/renderer.h"
#include "graphics/canvas.h"
#include "graphics/texture.h"
#include "ui/gui_layer.h"
#include "core/particle.h"
#include "core/sorter.h"
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
    // --- UI & Interaction Hooks ---
    // Allow GuiLayer to access private members for tuning
    friend class UI::GuiLayer;

public:
    /**
     * @brief Loads a source image from disk.
     * @param path File path to image.
     */
    void loadSourceImage(const std::string& path);

    /**
     * @brief Loads a target image from disk.
     * @param path File path to image.
     */
    void loadTargetImage(const std::string& path);

    /**
     * @brief Clears the drawing canvas.
     */
    void clearCanvas();

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
    std::unique_ptr<Graphics::Renderer> m_Renderer;
    std::unique_ptr<UI::GuiLayer> m_GuiLayer;
    std::unique_ptr<Canvas> m_Canvas;

    // Input State
    InputMode m_InputMode = InputMode::WEBCAM;
    cv::VideoCapture m_Webcam;
    cv::Mat m_CurrentFrame;
    cv::Mat m_StaticImage; // Loaded source image
    cv::Mat m_FrozenFrame; // Captured frame when transform starts
    
    // Target State
    cv::Mat m_TargetImage; // Loaded target image
    std::unique_ptr<Texture2D> m_TargetPreview; // GPU texture for GUI preview

    // Physics Parameters (Tunable via GUI)
    float m_ParticleSpeed = 0.005f;
    float m_FlowStrength = 0.0002f;
    float m_NoiseScale = 5.0f;

    // Particle System
    std::vector<Particle> m_Particles;
    float m_Time = 0.0f;
    
    // Drawing State (for Canvas mode)
    enum class DrawTool { PEN, ERASER, FILL };
    DrawTool m_DrawTool = DrawTool::PEN;
    glm::vec3 m_DrawColor = glm::vec3(1.0f, 0.0f, 0.0f); // Default red
    float m_BrushSize = 4.0f;
    bool m_IsDrawing = false;
    glm::vec2 m_LastMousePos = glm::vec2(0.0f);
    
    // Core Logic
    std::unique_ptr<Sorter> m_Sorter;
    bool m_IsTransforming = false;
    int m_SimulationWidth = 256;
    int m_SimulationHeight = 256;
    
    /**
     * @brief Recalculates particle targets based on current source and target images.
     */
    void recalculateTargets();
    
public:
    /**
     * @brief Starts the transform animation - particles begin moving to targets.
     */
    void startTransform();
    
    /**
     * @brief Stops the transform animation and resets particles to source positions.
     */
    void stopTransform();
    
    /**
     * @brief Returns whether transform is currently active.
     */
    bool isTransforming() const { return m_IsTransforming; }
};
