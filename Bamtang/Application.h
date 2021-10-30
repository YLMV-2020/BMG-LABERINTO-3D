#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <functional>
#include <queue>
#include <map>
#include <unordered_map>
#include <list>
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

#include "Vertex.h"
#include "VertexBoneData.h"
#include "BoneMatrix.h"



#include "IObject.h"

#include "IBaseComponent.h"

#include "IMeshComponent.h"
#include "MeshComponent.h"

#include "AnimatorComponent.h"
#include "RenderComponent.h"

#include "GameObject.h"

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

        Shader shaderObject;
        Shader shaderObjectInstance;
        Shader shaderObjectDynamic;
        Shader shaderObjectDynamicInstance;
        GameObject* object;
        GameObject* object2;
        //ObjectDynamic* dynamic2;

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
            this->skybox = new Skybox("skyrender", "tga");
            this->ground = new Ground("marble.jpg", glm::vec3(200, -0.01f, 200), 50.0f);
            // --------- MANAGERS ----------- //
            this->inputManager = InputManager::Instance(window, camera);
            this->timeManager = TimeManager::Instance();
            this->resourceManager = ResourceManager::Instance();
            // --------- EVENTS ----------- //
            this->event_app = Event_Application::Instance(window, camera);
            // --------- GUI ----------- //
            this->gui_app = new GUI_Application(window, version);




            this->shaderObject = Shader("assets/shaders/object.vert", "assets/shaders/object.frag");
            this->shaderObjectInstance = Shader("assets/shaders/instanceModel.vert", "assets/shaders/instanceModel.frag");
            this->shaderObjectDynamic = Shader("assets/shaders/animation.vert", "assets/shaders/animation.frag");
            this->shaderObjectDynamicInstance = Shader("assets/shaders/animation.vert", "assets/shaders/animation.frag");

            Maze* maze = new Maze(51);
            int nCount = maze->GetBlockCount();
            glm::mat4 *matrix = maze->GetMatrix();

            this->object = new GameObject("assets/animations/character/character.dae", glm::vec3(200.0f, 1.0f, 200.0f));
            this->object2 = new GameObject("assets/objects/block/block.obj", glm::vec3(0.f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f), false, nCount, matrix);
            //this->dynamic =new ObjectDynamic("assets/animations/bruja/bruja.dae" , glm::vec3(1.0f, 1.0f, 0.0f));
            //this->dynamic->addAnimation("assets/animations/bruja/Samba Dancing.dae");
            //this->dynamic->currentAnimation = 1;
            //this->dynamic2 =new ObjectDynamic("assets/animations/character/character.dae" , glm::vec3(12.0f, 1.0f, 0.0f));
        }

        ~Application()
        {
            Clear();
        }

        void Start(std::string name, std::string version)
        {
            if (glfwInit() == GL_FALSE) return;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version[0] - '0');
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version[1] - '0');
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            auto& monitor = *glfwGetVideoMode(glfwGetPrimaryMonitor());

            glfwWindowHint(GLFW_RED_BITS, monitor.redBits);
            glfwWindowHint(GLFW_BLUE_BITS, monitor.blueBits);
            glfwWindowHint(GLFW_GREEN_BITS, monitor.greenBits);
            glfwWindowHint(GLFW_REFRESH_RATE, monitor.refreshRate);

            //glfwWindowHint(GLFW_RESIZABLE, false);

            window = glfwCreateWindow(WIDTH, HEIGHT, name.c_str(), NULL, NULL);
            glfwMakeContextCurrent(window);

            if (GLenum err = glewInit()) return;

            //glfwSwapInterval(0);

            glEnable(GL_DEPTH_TEST);

        }

        void Update()
        {

            inputManager->ProccessKeyboard();
            inputManager->ProcessCameraMovement(timeManager->GetDeltaTime());
            gui_app->Update();
            camera->Update();

          /*  object2->Update();
            object->Update();*/

        }

        void Render()
        {
            skybox->Render(*camera, glm::vec3(1.0f));
            ground->Render(*camera, glm::vec3(1.0f));

            object2->RenderInstance(*camera, shaderObjectInstance);

            object->UpdateTime(timeManager->GetLastFrame());

            object->Render(*camera, shaderObjectDynamic);
        }

        void Render3D()
        {
            
            while (!glfwWindowShouldClose(window))
            {
                timeManager->SetTime(glfwGetTime());
                
                gui_app->NewFrame();

                glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
                glm::mat4 ViewMatrix = camera->GetViewMatrix();

                Update();

                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->Update());
                {
                    glClearColor(0, 0, 0, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    Render();
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                gui_app->Render(framebuffer->Display());

                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }

        void Clear()
        {
            glfwTerminate();
        }

	};


}