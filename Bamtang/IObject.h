
namespace Bamtang
{
    class IObject
    {
    public:
        virtual ~IObject() {}

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 transform;

        virtual void Start() = 0;
        virtual void Render(Camera& camera, Shader& shader) = 0;
        virtual void Draw(Shader& shader) = 0;
    };
}
