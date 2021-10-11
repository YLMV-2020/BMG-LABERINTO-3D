namespace Bamtang
{
    class Input
    {
    private:

        GLFWwindow* window;
        Camera* camera;

        int lastX;
        int lastY;

        Input(GLFWwindow* _window, Camera* _camera) : window(_window), camera(_camera) {}

    public:

        static Input* Instance(GLFWwindow* window, Camera* camera)
        {
            static Input input(window, camera);
            return &input;
        }


        void Keyboard()
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                std::cout << "TECLA\n";
        }

        void ProcessMouse(double xpos, double ypos)
        {

            /*if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }*/

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            //std::cout << "last x: " << xpos << "\n";

            /*if (!mouseCursorDisabled)
                processMouseMovement(xoffset, yoffset);*/
            ProcessMouseMovement(xoffset, yoffset);
        }

        void ProcessMouseMovement(float xoffset, float yoffset)
        {
            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            camera->SetYaw(camera->GetYaw() + xoffset);
            camera->SetPitch(camera->GetPitch() + yoffset);

            if (camera->GetPitch() > 89.0f)
                camera->SetPitch(89.0f);
            if (camera->GetPitch() < -89.0f)
                camera->SetPitch(-89.0f);

            glm::vec3 front;
            front.x = cos(glm::radians(camera->GetYaw())) * cos(glm::radians(camera->GetPitch()));
            front.y = sin(glm::radians(camera->GetPitch()));
            front.z = sin(glm::radians(camera->GetYaw())) * cos(glm::radians(camera->GetPitch()));
            front = glm::normalize(front);

            camera->SetFront(front);
        }

    };
}