#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <functional>
#include <queue>
#include <map>
#include <fstream>
#include <sstream>

#include "Maze.h"

#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"

#include "ResourceManager.h"
#include "TimeManager.h"

#include "Camera.h"
#include "Skybox.h"
#include "Ground.h"

#include "InputManager.h"

#include "GUI_Application.h"

#include "Event_Application.h"

namespace Bamtang
{
	class Application
	{

	private:

		GLFWwindow* window;

        unsigned int WIDTH;
        unsigned int HEIGHT;

        // GUI
        GUI_Application* gui_app;

        // UTIL
		Framebuffer* framebuffer;

        // ENGINE
        Camera* camera;
        Skybox* skybox;
        Ground* ground;

        //MANAGERS
        InputManager* inputManager;
        TimeManager* timeManager;
        ResourceManager* resourceManager;

        // EVENTS
        Event_Application* event_app;

	public:

        Application(std::string name, std::string version, unsigned int WIDTH, unsigned int HEIGHT)
        {
            this->WIDTH = WIDTH;
            this->HEIGHT = HEIGHT;

            Start(name, version);

            // --------- UTIL ----------- //
            this->framebuffer = new Framebuffer(WIDTH, HEIGHT);
            // --------- ENGINE ----------- //
            this->camera = Camera::Instance(WIDTH, HEIGHT);
            this->skybox = new Skybox("bosque", "png");
            this->ground = new Ground("marble.jpg", glm::vec3(100, -0.01f, 100), 50.0f);
            // --------- MANAGERS ----------- //
            this->inputManager = InputManager::Instance(window, camera);
            this->timeManager = TimeManager::Instance();
            this->resourceManager = ResourceManager::Instance();
            // --------- EVENTS ----------- //
            this->event_app = Event_Application::Instance(window, camera);
            // --------- GUI ----------- //
            this->gui_app = new GUI_Application(window, version);
            
        }

        ~Application()
        {
            Clear();
        }

        void Start(std::string name, std::string version)
        {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version[0] - '0');
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version[1] - '0');
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            //glfwWindowHint(GLFW_RESIZABLE, false);

            window = glfwCreateWindow(WIDTH, HEIGHT, name.c_str(), nullptr, nullptr);
            glfwMakeContextCurrent(window);

            if (GLenum err = glewInit()) return;

            //glfwSwapInterval(0);

            glViewport(0, 0, WIDTH, HEIGHT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        }

        void Update()
        {
            inputManager->ProccessKeyboard();
            inputManager->ProcessCameraMovement(timeManager->GetDeltaTime());
            gui_app->Update();
            camera->Update();
        }

        void Render()
        {
            skybox->Render(*camera, glm::vec3(1.0f));
            ground->Render(*camera, glm::vec3(1.0f));
        }

        void Render3D()
        {
            while (!glfwWindowShouldClose(window))
            {
                timeManager->SetTime(glfwGetTime());
                
                glfwPollEvents();

                glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
                glm::mat4 ViewMatrix = camera->GetViewMatrix();

                Update();

                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->Update());
                {
                    glClearColor(0, 1, 1, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    Render();
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                gui_app->Render(framebuffer);

                glfwSwapBuffers(window);
            }
        }

        void Clear()
        {
            glfwTerminate();
        }

	};


}