#pragma once

namespace UI {
    // Forward declare App so we can pass it to render
}
class App;

namespace UI {

    /**
     * @class GuiLayer
     * @brief Manages the ImGui Overlay.
     * 
     * This class handles the Immediate Mode GUI context, specifically for
     * standard debug panels, parameter tuning, and docking support.
     */
    class GuiLayer {
    public:
        GuiLayer();
        ~GuiLayer();

        /**
         * @brief Starts a new ImGui frame.
         * Must be called before any ImGui widgets are defined.
         */
        void begin();

        /**
         * @brief Defines the actual UI layout (Windows, Buttons, etc.).
         * @param app Pointer to the main App instance for interacting with variables.
         */
        void render(App* app);

        /**
         * @brief Ends the frame and issues draw calls to the GPU.
         * Handles multi-viewport platform window rendering if enabled.
         */
        void end();
    };

}
