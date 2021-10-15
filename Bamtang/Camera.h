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

        float minVelocity = 0.3f;
        float maxVelocity = 5;

        float speed;

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
            this->up = glm::vec3(0.0f, 1.0f, 0.0f);
            this->front = glm::vec3(0.0f, 0.0f, -1.0f);
            this->position = glm::vec3(0.0f, 10.0f, 0.0f);
            this->projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, near, far);
            this->view = glm::lookAt(position, position + front, up);

            this->speed = this->minVelocity;

            UpdateVectors();
        }

    public:

        void Update()
        {
            this->view = glm::lookAt(position, position + front, up);
        }

        void UpdateVectors()
        {
            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(front);

            this->front = front;
        }

        void ChangeSpeed()
        {
            speed = (speed == minVelocity) ? maxVelocity : minVelocity;
        }

        glm::mat4 &GetViewMatrix()
        {
            //return glm::lookAt(position, position + front, up);
            return view;
        }

        glm::mat4 &GetProjectionMatrix()
        {
            //return glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, near, far);
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

        unsigned int GetWidth()
        {
            return this->WIDTH;
        }

        unsigned int GetHeight()
        {
            return this->HEIGHT;
        }

        float GetFov()
        {
            return this->fov;
        }

        float GetNear()
        {
            return this->near;
        }

        float GetFar()
        {
            return this->far;
        }

        float GetSpeed()
        {
            return this->speed;
        }

    };
}