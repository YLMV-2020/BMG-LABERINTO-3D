
namespace Bamtang
{
    class IMesh
    {
    public:

        virtual ~IMesh() {}

        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        GLuint VAO, VBO, EBO;

        virtual void Setup() = 0;
        virtual void Draw(Shader*& shader) = 0;
    };
}
