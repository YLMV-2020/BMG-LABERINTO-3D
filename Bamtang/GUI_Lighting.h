namespace Bamtang
{
    class GUI_Lighting : public IGraphicalUserInterface
    {

    private:



    public:

        GUI_Lighting()
        {

        }

        void Start() override
        {

        }

        void Update() override
        {

        }

        void Display() override
        {
			ImGui::Begin("Scene", nullptr);
			{
				/*ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(dark));
				ImGui::BeginChildFrame(ImGui::GetID("toolbar"), ImVec2(0, 32));
				{
					if (Widget::ToolButton::Show(icons.at("move"))) { gizmo.Operation = ImGuizmo::OPERATION::TRANSLATE; }
					ImGui::SameLine();
					ImGui::Dummy(ImVec2(2, 0));

					ImGui::SameLine();
					if (Widget::ToolButton::Show(icons.at("rotate"))) { gizmo.Operation = ImGuizmo::OPERATION::ROTATE; }
					ImGui::SameLine();

					ImGui::SameLine();
					if (Widget::ToolButton::Show(icons.at("scale"))) { gizmo.Operation = ImGuizmo::OPERATION::SCALE; }
					ImGui::SameLine();
					ImGui::Dummy(ImVec2(48, 0));

					ImGui::SameLine();
					if (Widget::ToolButton::Show(icons.at("save"))) { Orbit3D::Serializer.SaveScene("Resource/Scene/scene.o3d"); }
					ImGui::SameLine();
					ImGui::Dummy(ImVec2(2, 0));

					ImGui::SameLine();
					if (Widget::ToolButton::Show(icons.at("build"))) {
						Core.StopGame();
					}
					ImGui::SameLine();
					ImGui::Dummy(ImVec2(2, 0));

					ImGui::SameLine();
					if (Widget::ToolButton::Show(icons.at("play"))) {
						Core.StartGame();
					}
					ImGui::SameLine();
					ImGui::Dummy(ImVec2(2, 0));
				}
				ImGui::EndChildFrame();
				ImGui::PopStyleColor();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::BeginChildFrame(ImGui::GetID("sceneFrame"), ImVec2(0, 0), ImGuiWindowFlags_NoScrollbar);
				{
					const ImVec2& size = ImGui::GetWindowSize();
					ImGui::Image(sceneFrameTexture, size, ImVec2(0, 1), ImVec2(1, 0));
					const ImVec2& pos = ImGui::GetWindowPos();
					viewRect = { pos.x, pos.y, size.x, size.y };
					TransformGizmo();
				}
				ImGui::PopStyleVar();
				ImGui::EndChildFrame();*/
			}
			ImGui::End();
        }

    };

}