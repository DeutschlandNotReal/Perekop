#pragma once
#include <pkutil/Event.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Perekop { void draw(); bool gui_test_move(); }
namespace pk {
    class GUIObject {
        friend void Perekop::draw();
        friend bool Perekop::gui_test_move();
        public: 
            enum GUIType { frame, button, text, textbutton };
        private:
            enum GUITFlag { entered = 1 };
            int tflags = 0; 
            void* metadata;

            GUIType type;
        public:
            enum GUIProperty { draggable = 1, listenable = 2 };
            int flags = draggable | listenable;
            long long id;
            float Z;
            glm::vec4 colour;
            glm::vec2 size; glm::vec2 position;
            GUIObject(GUIType type, float Z, glm::vec2 position, glm::vec2 size, glm::vec4 colour);
    };
}