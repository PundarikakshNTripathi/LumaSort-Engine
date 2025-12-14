#pragma once

namespace Graphics {

    /**
     * @class Renderer
     * @brief Wraps OpenGL Rendering Commands.
     * 
     * This class abstracts raw OpenGL calls, providing a higher-level interface for
     * drawing the pixel sorting visualization. It handles context-specific operations.
     */
    class Renderer {
    public:
        /**
         * @brief Initializes the renderer.
         * Note: Requires an active OpenGL context to function.
         */
        Renderer();
        ~Renderer();

        /**
         * @brief Clears the screen with the designated background color.
         * Should be called at the start of every frame.
         */
        void clear();
    };

}
