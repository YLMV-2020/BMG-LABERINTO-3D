#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <functional>
#include <queue>

#include "Maze.h"

#include "Framebuffer.h"

#include "Camera.h"
#include "Input.h"

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
        Input *input;

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
            this->input = Input::Instance(window, camera);
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
        }

        void Render3D()
        {
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();

                input->Keyboard();
                gui_app->Update();

                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->Update());
                {
                    glClearColor(0, 1, 1, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    //object

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