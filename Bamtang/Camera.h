#include <GL/glew.h>

namespace Bamtang
{
    class Camera
    {

    private:

        unsigned int WIDTH;
        unsigned int HEIGHT;

        glm::mat4 projection;
        glm::mat4 view;

        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;

        float fov = 45.0f;
        float near = 0.1f;
        float far = 1000.0f;

        float yaw;
        float pitch;

    public:

        static Camera* Instance(unsigned int WIDTH, unsigned int HEIGHT)
        {
            static Camera camera(WIDTH, HEIGHT);
            return &camera;
        }
        
    private:

        Camera(unsigned int WIDTH, unsigned int HEIGHT)
        {
            this->WIDTH = WIDTH;
            this->HEIGHT = HEIGHT;

            Start();
        }

        void Start()
        {
            this->up = glm::vec3(0, 1, 0);
            this->front = glm::vec3(0.0f, 0.0f, -1.0f);

            this->position = glm::vec3(5.0f, 10.0f, 10.75f);

            this->projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, near, far);
            this->view = glm::lookAt(position, position + front, up);
        }

    public:

        glm::mat4 GetViewMatrix()
        {
            return view;
        }

        glm::mat4 GetProjectionMatrix()
        {
            return projection;
        }

        void SetYaw(float yaw)
        {
            this->yaw = yaw;
        }

        void SetPitch(float pitch)
        {
            this->pitch = pitch;
        }

        void SetPosition(glm::vec3 position)
        {
            this->position = position;
        }

        void SetFront(glm::vec3 front)
        {
            this->front = front;
        }

        void SetUp(glm::vec3 up)
        {
            this->up = up;
        }

        float GetYaw()
        {
            return yaw;
        }

        float GetPitch()
        {
            return pitch;
        }

        glm::vec3 GetPosition()
        {
            return this->position;
        }

        glm::vec3 GetFront()
        {
            return this->front;
        }

        glm::vec3 GetUp()
        {
            return this->up;
        }

    };



}