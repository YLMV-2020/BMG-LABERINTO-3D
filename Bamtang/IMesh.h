
namespace Bamtang
{
    class IMesh
    {
    public:

        virtual ~IMesh() {}

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;
        std::vector<VertexBoneData> bones;

        GLuint VAO, EBO; 
        GLuint VBO_vertices;
        GLuint VBO_bones;

        virtual void Setup() = 0;

    };
}
