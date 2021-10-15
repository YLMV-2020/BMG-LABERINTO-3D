
namespace Bamtang
{
    class Event_Application
    {

    private:

        GLFWwindow* window;

        Camera* camera;
        InputManager* inputManager;

        std::function<void(int width, int height)> resize;
        std::function<void(int key, int scancode, int action, int mods)> keyboard;
        std::function<void(double xpos, double ypos)> mousePosition;
        std::function<void(double xoffset, double yoffset)> offset;
        std::function<void(int button, int action, int mods)> mouseButton;

    public:

        static Event_Application* Instance(GLFWwindow* window, Camera* camera)
        {
            static Event_Application event(window, camera);
            return &event;
        }

    private:

        Event_Application(GLFWwindow* _window, Camera* _camera)
        {
            this->window = _window;
            this->camera = _camera;
            this->inputManager = InputManager::Instance(this->window, this->camera);
            Start();
        }

        void Start()
        {
            Init();
            glfwSetWindowUserPointer(window, this);
            EventPointer();
        }

        void Init()
        {
            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
                auto w = (Event_Application*)glfwGetWindowUserPointer(window); if (w->resize) w->resize(width, height);
                });

            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                auto w = (Event_Application*)glfwGetWindowUserPointer(window); if (w->keyboard) w->keyboard(key, scancode, action, mods);
                });

            glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
                auto w = (Event_Application*)glfwGetWindowUserPointer(window); if (w->mousePosition) w->mousePosition(xpos, ypos);
                });

            glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
                auto w = (Event_Application*)glfwGetWindowUserPointer(window); if (w->offset) w->offset(xoffset, yoffset);
                });

            glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
                auto w = (Event_Application*)glfwGetWindowUserPointer(window); if (w->mouseButton) w->mouseButton(button, action, mods);
                });

        }

        void EventPointer()
        {
            resize = [&](int width, int height)
            {
                glViewport(0, 0, width, height);
            };

            keyboard = [&](int key, int sancode, int action, int mods)
            {
                if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
                {
                    camera->ChangeSpeed();
                }
                else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
                {
                    camera->ChangeSpeed();
                }
            };

            mousePosition = [&](double xpos, double ypos)
            {
                //input->processMouse(xpos, ypos);
                inputManager->ProcessMouse(xpos, ypos);
            };

            offset = [&](float xoffset, float yoffset)
            {
               
            };

            mouseButton = [&](int button, int action, int mods)
            {
                if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
                {
                    inputManager->ChangeCursor();
                }
            };

        }
    };
}