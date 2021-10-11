namespace Bamtang
{
    class Input
    {
    private:

        GLFWwindow* window;

        int lastX;
        int lastY;

        Input(GLFWwindow* _window) : window(_window) {}

    public:

        static Input* Instance(GLFWwindow* window)
        {
            static Input input(window);
            return &input;
        }


        void Keyboard()
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                std::cout << "TECLA\n";
        }

    };
}