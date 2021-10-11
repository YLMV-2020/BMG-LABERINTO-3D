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
            ImGui::Begin("Lighting", NULL);

            ImGui::Text("F");

            ImGui::End();
        }

    };

}