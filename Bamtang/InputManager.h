namespace Bamtang
{
    class InputManager
    {
    private:

        GLFWwindow* window;
        Camera* camera;

        int lastX, lastY;
        bool firstMouse = true;
        bool cursorEnabled = true;

        InputManager(GLFWwindow* _window, Camera* _camera) : window(_window), camera(_camera)
        {
            glfwGetWindowSize(window, &lastX, &lastY);
            lastX = lastX / 2;
            lastY = lastY / 2;
        }

    public:

        static InputManager* Instance(GLFWwindow* window, Camera* camera)
        {
            static InputManager input(window, camera);
            return &input;
        }

        void ProccessKeyboard(ObjectDynamic*d)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {

                if (d->currentAnimation == 1)
                {
                    d->currentAnimation = 0;
               }
                
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            {
                
            }
                
        }

        void ProcessMouse(double xpos, double ypos)
        {
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            if (!cursorEnabled)
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

            camera->UpdateVectors();
        }

        void ProcessCameraMovement(float deltaTime)
        {
            float cameraSpeed = camera->GetSpeed() * deltaTime * 20.0f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera->SetPosition(camera->GetPosition() + cameraSpeed * camera->GetFront());
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera->SetPosition(camera->GetPosition() - cameraSpeed * camera->GetFront());
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera->SetPosition(camera->GetPosition() - glm::normalize(glm::cross(camera->GetFront(), camera->GetUp())) * cameraSpeed);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera->SetPosition(camera->GetPosition() + glm::normalize(glm::cross(camera->GetFront(), camera->GetUp())) * cameraSpeed);
        }

        void ChangeCursor()
        {
            if (!cursorEnabled)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cursorEnabled = !cursorEnabled;
        }

    };
}