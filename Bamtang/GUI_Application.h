#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Bamtang
{
    class GUI_Application
    {

    private:

        ImVec2 displayRender;
        ImVec2 display;
        ImVec2 mouse;

    public:

        GUI_Application(GLFWwindow* window, std::string version)
        {
            Start(window, version);
        }

        void Update()
        {
        }

        void Render(Framebuffer* framebuffer)
        {

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            DockSpace();

            Display(framebuffer);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            ImGuiIO& io = ImGui::GetIO(); (void)io;

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

        }

    private:

        void Start(GLFWwindow* window, std::string version)
        {

            std::string glsl_version = "#version " + version + "0";

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.ConfigFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;

            ImGui::StyleColorsDark();

            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }

            ImGuiWindowFlags window_flags = 0;
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version.c_str());

        }

        void Display(Framebuffer* framebuffer)
        {
            ImGui::Begin("Render", NULL);
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            displayRender = ImGui::GetWindowSize();
            display = ImGui::GetCursorScreenPos();

            drawList->AddImage(
                (void*)framebuffer->Display(), display,
                ImVec2(display.x + displayRender.x - 10, display.y + displayRender.y - 30), ImVec2(0, 1), ImVec2(1, 0)
            );

            ImVec2 mousePosition = ImGui::GetMousePos();
            ImVec2 sceenPosition = ImGui::GetCursorScreenPos();
            ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

            mouse = ImVec2(mousePosition.x - sceenPosition.x - scroll.x, mousePosition.y - sceenPosition.y - scroll.y);

            ImGui::End();
        }

        void DockSpace()
        {
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->GetWorkPos());
                ImGui::SetNextWindowSize(viewport->GetWorkSize());
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", NULL, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            else
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
                ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
                ImGui::SameLine(0.0f, 0.0f);
                if (ImGui::SmallButton("click here"))
                    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                    ImGui::MenuItem("Padding", NULL, &opt_padding);
                    //ImGui::Separator();
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }
            ImGui::End();
        }
    };

}