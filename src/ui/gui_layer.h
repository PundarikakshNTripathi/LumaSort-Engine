#pragma once

namespace UI {

    class GuiLayer {
    public:
        GuiLayer();
        ~GuiLayer();

        void begin();
        void render();
        void end();
    };

}
