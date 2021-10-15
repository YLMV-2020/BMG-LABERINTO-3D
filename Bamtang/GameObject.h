#include "E_ComponentType.h"

namespace Bamtang
{
    class GameObject 
    {
    private:

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 transform;

        std::map<E_ComponentType, IBaseComponent*> m_components;

    public:

        GameObject(std::string const& path, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f), bool gamma = false)
        {
            transform = glm::mat4(1.0f);
            Translate(position);
            Rotation(rotation);
            Scale(scale);

            auto render = new RenderComponent(path, gamma);

            render->Update(transform);

            AddComponent(E_ComponentType::Render, render);
            //AddComponent(new MeshComponent());
        }

        ~GameObject() {}

        void AddComponent(E_ComponentType type, IBaseComponent* comp)
        {
            m_components[type] = comp;
        }

        void Translate(glm::vec3 position)
        {
            transform = glm::translate(transform, position);
        }

        void Rotation(glm::vec3 rotation)
        {
            transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        void Scale(glm::vec3 scale)
        {
            transform = glm::scale(transform, scale);
        }

        void Update()
        {
            std::map<E_ComponentType, IBaseComponent*>::iterator it = m_components.find(E_ComponentType::Render);

            if (it != m_components.end())
            {
                it->second->Update(transform);
            }
        }

        void UpdateTime(float &currentFrame)
        {
            std::map<E_ComponentType, IBaseComponent*>::iterator it = m_components.find(E_ComponentType::Render);

            if (it != m_components.end())
            {
                RenderComponent* render = static_cast<RenderComponent*>(it->second);
                render->UpdateTime(currentFrame);
            }
        }

        void Draw(Shader& shader) 
        {

        }

        void Render(Camera& camera, Shader& shader) 
        {

            //std::list<IBaseComponent*>::iterator compIt = components.begin();
            //for (compIt; compIt != components.end(); ++compIt)
            //{
            //   /* MeshComponent* mesh = static_cast<MeshComponent*>(*compIt);
            //    if (typeid(mesh) == typeid(MeshComponent*))
            //    {

            //        std::cout << "YES" << "\n";
            //    }
            //    else
            //    {
            //        std::cout << typeid(MeshComponent*).name() << "\n";

            //    }
            //    mesh->Debug();*/
            //    (*compIt)->Render(camera, shader);
            //}

            std::map<E_ComponentType, IBaseComponent*>::iterator it = m_components.find(E_ComponentType::Render);

            if (it != m_components.end())
            {
                it->second->Render(camera, shader);
            }

        }



    };

}







