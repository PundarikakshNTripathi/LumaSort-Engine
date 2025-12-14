#include "app.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

App::App(const std::string& title, int width, int height)
    : m_Title(title), m_Width(width), m_Height(height) {
    // Determine the environment and initialize core systems immediately.
    init();
}

App::~App() {
    // Release resources.
    shutdown();
}

void App::init() {
    // 1. Setup GLFW Error Callback
    // This allows us to catch window creation errors or context issues early.
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });

    // 2. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(1);
    }

    // 3. Configure Window Hints for OpenGL 3.3 Core Profile
    // We target 3.3 to ensure wide compatibility while having access to modern shaders.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 4. Create the Window
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }

    // Make the window's context current on this thread
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // Enable vsync to prevent tearing

    // 5. Initialize GLAD
    // Important: Must be done after making the context current.
    // GLAD loads the actual OpenGL function pointers from the driver.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    // 6. Initialize Sub-systems
    // Renderer needs OpenGL context, so it comes after GLAD.
    m_Renderer = std::make_unique<Graphics::Renderer>();

    // 7. Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Enable features we need for a desktop-like experience
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking (Crucial for our layout)
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    m_GuiLayer = std::make_unique<UI::GuiLayer>();
}

void App::run() {
    // Main Application Loop
    while (!glfwWindowShouldClose(m_Window)) {
        // Poll for inputs (keyboard, mouse, window events)
        glfwPollEvents();

        // Perform rendering (Game Logic -> Render Commands)
        render();

        // Swap front and back buffers to display the new frame
        glfwSwapBuffers(m_Window);
    }
}

void App::render() {
    // 1. Clear the screen
    m_Renderer->clear();

    // 2. Render UI Layer
    // We wrap this significantly to abstract ImGui frame management.
    m_GuiLayer->begin();
    m_GuiLayer->render();
    m_GuiLayer->end();
}

void App::shutdown() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup GLFW
    if (m_Window) {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}
