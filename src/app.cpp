#include "app.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "core/flow_field.h"

App::App(const std::string& title, int width, int height)
    : m_Title(title), m_Width(width), m_Height(height) {
    // Determine the environment and initialize core systems immediately.
    init();

    // Default to Webcam
    m_InputMode = InputMode::WEBCAM;
    
    // Initialize Canvas (resize handled later ideally, but fixed for now)
    // For now, let's assume a fixed canvas size or window size.
    m_Canvas = std::make_unique<Canvas>(width, height);

    // Try opening webcam
    if (!m_Webcam.open(0)) {
        std::cerr << "Warning: Could not open webcam." << std::endl;
    }
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
    m_TargetPreview = std::make_unique<Texture2D>();
    m_Sorter = std::make_unique<Sorter>();
}

void App::run() {
    // Main Application Loop
    while (!glfwWindowShouldClose(m_Window)) {
        // Poll for inputs (keyboard, mouse, window events)
        glfwPollEvents();

        // Calculate delta time if needed
        // For now, just update state
        update();

        // Perform rendering (Game Logic -> Render Commands)
        render();

        // Swap front and back buffers to display the new frame
        glfwSwapBuffers(m_Window);
    }
}

void App::render() {
    // 1. Clear the screen
    m_Renderer->clear();

    // 2. Render Particles with viewport-aware point sizing
    // Pass current window size and simulation dimensions to calculate proper point size
    m_Renderer->renderParticles(m_Particles, m_Width, m_Height, m_SimulationWidth, m_SimulationHeight);

    // 3. Render UI Layer
    // We wrap this significantly to abstract ImGui frame management.
    m_GuiLayer->begin();
    m_GuiLayer->render(this);
    m_GuiLayer->end();
}

void App::update() {
    // Handle Input Mode updates
    if (m_InputMode == InputMode::WEBCAM) {
        if (m_Webcam.isOpened()) {
            m_Webcam >> m_CurrentFrame;
            
            // Set resolution based on webcam frame (once)
            if (!m_CurrentFrame.empty() && m_SimulationWidth == 256) {
                int maxRes = 600; // Cap for webcam to maintain real-time performance
                float aspectRatio = (float)m_CurrentFrame.cols / (float)m_CurrentFrame.rows;
                m_SimulationWidth = std::min(m_CurrentFrame.cols, maxRes);
                m_SimulationHeight = (int)(m_SimulationWidth / aspectRatio);
                m_SimulationWidth = std::max(m_SimulationWidth, 256);
                m_SimulationHeight = std::max(m_SimulationHeight, 256);
                std::cout << "Webcam resolution: " << m_CurrentFrame.cols << "x" << m_CurrentFrame.rows 
                          << " -> Simulation: " << m_SimulationWidth << "x" << m_SimulationHeight << std::endl;
            }
        }
    } 
    else if (m_InputMode == InputMode::CANVAS) {
        processInput(); // Handle drawing interactions
        
        // Set canvas resolution to window size for full quality
        if (m_SimulationWidth == 256) {
            m_SimulationWidth = m_Width;
            m_SimulationHeight = m_Height;
            std::cout << "Canvas resolution: " << m_SimulationWidth << "x" << m_SimulationHeight << std::endl;
        }
        
        // Read canvas texture back to CPU for sorting
        m_CurrentFrame = m_Canvas->getAsMat();
    }
    else if (m_InputMode == InputMode::IMAGE) {
        if (!m_StaticImage.empty()) {
            m_StaticImage.copyTo(m_CurrentFrame);
        }
    }

    // --- Particle System Update ---
    
    int numParticles = m_SimulationWidth * m_SimulationHeight;
    if (m_Particles.size() != numParticles) {
        m_Particles.clear();
        m_Particles.reserve(numParticles);
        float maxDim = (float)(m_SimulationWidth - 1);
        for (int y = 0; y < m_SimulationHeight; ++y) {
            for (int x = 0; x < m_SimulationWidth; ++x) {
                Particle p;
                p.pos = glm::vec2(x / maxDim, y / maxDim); 
                p.vel = glm::vec2(0.0f);
                p.acc = glm::vec2(0.0f);
                p.target = p.pos;
                p.color = glm::vec4(1.0f);
                m_Particles.push_back(p);
            }
        }
    }

    // Update particle colors from current frame (or frozen frame during transform)
    cv::Mat& colorSource = m_IsTransforming ? m_FrozenFrame : m_CurrentFrame;
    if (!colorSource.empty()) {
        cv::Mat resizedFrame;
        cv::resize(colorSource, resizedFrame, cv::Size(m_SimulationWidth, m_SimulationHeight));
        
        float maxDim = (float)(m_SimulationWidth - 1);
        for (int i = 0; i < m_Particles.size(); ++i) {
            int x = i % m_SimulationWidth;
            int y = i / m_SimulationWidth;
            cv::Vec3b pixel = resizedFrame.at<cv::Vec3b>(y, x);
            m_Particles[i].color = glm::vec4(pixel[2] / 255.0f, pixel[1] / 255.0f, pixel[0] / 255.0f, 1.0f);
        }
    }

    // Only apply physics when transforming
    if (m_IsTransforming) {
        m_Time += 0.01f;

        for (auto& p : m_Particles) {
            glm::vec2 desired = p.target - p.pos;
            float dist = glm::length(desired);
            
            glm::vec2 steer = glm::vec2(0.0f);
            if (dist > 0.0001f) {
                 steer = glm::normalize(desired) * m_ParticleSpeed;
            }

            glm::vec2 flow = FlowField::getForce(p.pos, m_Time, m_NoiseScale) * m_FlowStrength;

            p.acc += steer + flow;
            p.vel += p.acc;
            p.pos += p.vel;
            p.acc = glm::vec2(0.0f);
            p.vel *= 0.90f;
        }
    } else {
        // When not transforming, keep particles at their source grid positions
        float maxDim = (float)(m_SimulationWidth - 1);
        for (int i = 0; i < m_Particles.size(); ++i) {
            int x = i % m_SimulationWidth;
            int y = i / m_SimulationWidth;
            m_Particles[i].pos = glm::vec2(x / maxDim, y / maxDim);
            m_Particles[i].vel = glm::vec2(0.0f);
            m_Particles[i].acc = glm::vec2(0.0f);
        }
    }
}

void App::processInput() {
    if (m_InputMode == InputMode::CANVAS) {
        double xpos, ypos;
        glfwGetCursorPos(m_Window, &xpos, &ypos);
        
        bool mouseDown = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        
        if (mouseDown && !ImGui::GetIO().WantCaptureMouse) {
            glm::vec2 currentPos = glm::vec2(xpos, ypos);
            
            if (!m_IsDrawing) {
                // Just started drawing - draw a point at current position
                m_IsDrawing = true;
                m_LastMousePos = currentPos;
                
                // Draw initial point
                if (m_DrawTool == DrawTool::PEN) {
                    m_Canvas->drawLine(currentPos, currentPos, m_DrawColor, m_BrushSize);
                } else if (m_DrawTool == DrawTool::ERASER) {
                    m_Canvas->drawLine(currentPos, currentPos, glm::vec3(1.0f), m_BrushSize * 2.0f);
                }
            } else {
                // Continue drawing
                if (m_DrawTool == DrawTool::PEN) {
                    m_Canvas->drawLine(m_LastMousePos, currentPos, m_DrawColor, m_BrushSize);
                } else if (m_DrawTool == DrawTool::ERASER) {
                    m_Canvas->drawLine(m_LastMousePos, currentPos, glm::vec3(1.0f), m_BrushSize * 2.0f);
                }
                
                m_LastMousePos = currentPos;
            }
        } else {
            m_IsDrawing = false;
        }
    }
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

void App::loadSourceImage(const std::string& path) {
    cv::Mat img = cv::imread(path);
    if (!img.empty()) {
        m_StaticImage = img;
        
        // Adapt simulation resolution based on image size
        // Cap at 800x800 to maintain good performance on most systems
        int maxRes = 800;
        int newWidth = std::min(img.cols, maxRes);
        int newHeight = std::min(img.rows, maxRes);
        
        // Maintain aspect ratio if one dimension exceeds max
        float aspectRatio = (float)img.cols / (float)img.rows;
        if (aspectRatio > 1.0f) {
            newHeight = (int)(newWidth / aspectRatio);
        } else {
            newWidth = (int)(newHeight * aspectRatio);
        }
        
        // Ensure minimum resolution
        newWidth = std::max(newWidth, 128);
        newHeight = std::max(newHeight, 128);
        
        m_SimulationWidth = newWidth;
        m_SimulationHeight = newHeight;
        
        std::cout << "Loaded Source Image: " << path << std::endl;
        std::cout << "  Resolution: " << img.cols << "x" << img.rows << std::endl;
        std::cout << "  Simulation: " << m_SimulationWidth << "x" << m_SimulationHeight 
                  << " (" << (m_SimulationWidth * m_SimulationHeight) << " particles)" << std::endl;
    } else {
        std::cerr << "Failed to load source image: " << path << std::endl;
    }
}

void App::loadTargetImage(const std::string& path) {
    cv::Mat img = cv::imread(path);
    if (!img.empty()) {
        m_TargetImage = img;
        // Upload to GPU for preview
        if (m_TargetPreview) {
            m_TargetPreview->uploadFromOpenCV(m_TargetImage);
        }
        std::cout << "Loaded Target Image: " << path << std::endl;
    } else {
        std::cerr << "Failed to load target image: " << path << std::endl;
    }
}

void App::clearCanvas() {
    if (m_Canvas) {
        m_Canvas->clear();
    }
}

void App::recalculateTargets() {
    // Need both source and target to calculate
    if (m_FrozenFrame.empty() || m_TargetImage.empty()) {
        return;
    }
    
    // Use Sorter to get mapping
    std::vector<glm::vec2> mapping = m_Sorter->sortImage(m_FrozenFrame, m_TargetImage, m_SimulationWidth, m_SimulationHeight);
    
    if (mapping.empty()) {
        return;
    }
    
    // Update particle targets based on mapping
    // mapping[i] gives the target position for particle i (in pixel coordinates)
    float normX = (float)(m_SimulationWidth - 1);
    float normY = (float)(m_SimulationHeight - 1);
    for (size_t i = 0; i < m_Particles.size() && i < mapping.size(); ++i) {
        // Normalize to 0..1 range using proper dimension for each axis
        m_Particles[i].target = glm::vec2(mapping[i].x / normX, mapping[i].y / normY);
    }
}

void App::startTransform() {
    if (m_TargetImage.empty()) {
        std::cerr << "Cannot start transform: No target image loaded!" << std::endl;
        return;
    }
    if (m_CurrentFrame.empty()) {
        std::cerr << "Cannot start transform: No source image available!" << std::endl;
        return;
    }
    
    // Freeze the current frame for transformation
    m_CurrentFrame.copyTo(m_FrozenFrame);
    
    // Calculate targets once based on frozen frame
    recalculateTargets();
    
    m_IsTransforming = true;
    std::cout << "Transform started" << std::endl;
}

void App::stopTransform() {
    m_IsTransforming = false;
    m_Time = 0.0f;
    std::cout << "Transform stopped" << std::endl;
}
