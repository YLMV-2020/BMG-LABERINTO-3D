namespace Bamtang
{
    class GameObject 
    {
    private:

        std::list<IBaseComponent*> components;

    public:

        GameObject(std::string const& path, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), bool gamma = false) /*: gammaCorrection(gamma)*/
        {

        }

        ~GameObject() {}

        void AddComponent(IBaseComponent* comp)
        {
            components.push_back(comp);
        }

        // draws the model, and thus all its meshes
        void Draw(Shader& shader) 
        {

        }

        void Render(Camera& camera, Shader& shader) 
        {
            //shader.Use();

            //glm::mat4 view = camera.GetViewMatrix();
            //glm::mat4 projection = camera.GetProjectionMatrix();

            //transform = glm::mat4(1.0f);

            //transform = glm::translate(transform, position);

            //transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            //transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            //transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

            //transform = glm::scale(transform, scale);

            ////shader.SetVec3("viewPos", camera.GetPosition());

            //shader.SetMat4("model", transform);
            //shader.SetMat4("view", view);
            //shader.SetMat4("projection", projection);

            std::list<IBaseComponent*>::iterator compIt = components.begin();
            for (compIt; compIt != components.end(); ++compIt)
            {
                MeshComponent* mesh = static_cast<MeshComponent*>(*compIt);
                if (typeid(mesh) == typeid(MeshComponent*))
                {

                    std::cout << "YES" << "\n";
                }
                else
                {
                    std::cout << typeid(MeshComponent*).name() << "\n";

                }
                mesh->Debug();
                (*compIt)->Render(shader);
            }

            //for (unsigned int i = 0; i < components.size(); i++)
            //    components[i]->Render(shader);
        }




       

    };

}







