#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "IGraphicalUserInterface.h"

#include "GUI_Setting.h"
#include "GUI_Lighting.h"

namespace Bamtang
{
    class GUI_Application
    {

    private:

        ImVec2 displayRender;
        ImVec2 display;
        ImVec2 mouse;

        std::vector<IGraphicalUserInterface*> gui;

        ImGuiWindowFlags window_flags;

    public:

        GUI_Application(GLFWwindow* window, std::string version)
        {
            Start(window, version);
            AddGraphicalUserInterface(new GUI_Setting());
            AddGraphicalUserInterface(new GUI_Lighting());
        }

        void Update()
        {
        }

        void NewFrame()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            DockSpace();
        }

        void Render(const GLuint& texture)
        {
            Display(texture);
            DisplayGraphicalUserInterface();

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

            window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }

            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version.c_str());

        }

        void Display(const GLuint& texture)
        {
            ImGui::Begin("Render", NULL);
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            displayRender = ImGui::GetWindowSize();
            display = ImGui::GetCursorScreenPos();

            drawList->AddImage(
                (void*)texture, display,
                ImVec2(display.x + displayRender.x - 10, display.y + displayRender.y - 30), ImVec2(0, 1), ImVec2(1, 0)
            );

            ImVec2 mousePosition = ImGui::GetMousePos();
            ImVec2 sceenPosition = ImGui::GetCursorScreenPos();
            ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

            mouse = ImVec2(mousePosition.x - sceenPosition.x - scroll.x, mousePosition.y - sceenPosition.y - scroll.y);

            ImGui::End();
        }

        void AddGraphicalUserInterface(IGraphicalUserInterface *gui)
        {
            this->gui.push_back(gui);
        }

        void DisplayGraphicalUserInterface()
        {
            for (IGraphicalUserInterface *&inteface : gui)
            {
                inteface->Display();
            }
        }

        void DockSpace()
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("Dockspace", nullptr, window_flags);
            {
                ImGuiID dockID = ImGui::GetID("Dockspace");
                ImGui::DockSpace(dockID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            }

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("\xef\x85\x9c", "Open Project", "Ctrl + O")) {}
                    if (ImGui::MenuItem("New", "Ctrl+N")) {}
                    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
                    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                    if (ImGui::MenuItem("Save As..")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Editor")) {
                    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                    ImGui::Separator();
                    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Settings")) {
                    if (ImGui::MenuItem("Customize", "Ctrl+T")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::MenuItem("View Help", "Ctrl+H")) {}
                    if (ImGui::MenuItem("About Soft3D")) {}
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::End();
            ImGui::PopStyleVar(3);
        }
    };

}