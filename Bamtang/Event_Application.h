
namespace Bamtang
{
    class Event_Application
    {

    private:

        GLFWwindow* window;

        std::function<void(int width, int height)> resize;
        std::function<void(int key, int scancode, int action, int mods)> keyboard;
        std::function<void(double xpos, double ypos)> mousePosition;
        std::function<void(double xoffset, double yoffset)> displace;
        std::function<void(int button, int action, int mods)> mouseButton;

        Event_Application(GLFWwindow* _window) 
        {
            this->window = _window;
            Start();
        }

    public:

        static Event_Application* Instance(GLFWwindow* window)
        {
            static Event_Application event(window);
            return &event;
        }

    private:

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
        }

        void EventPointer()
        {
            resize = [&](int width, int height)
            {
                std::cout << "W: " << width << "\n";
                std::cout << "H: " << height << "\n";
            };
        }
    };
}