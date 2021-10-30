

namespace Bamtang
{
    class MeshComponent : public IMeshComponent
    {
    private:

        bool hasBone = false;

    public:

        MeshComponent() {}

        MeshComponent(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::vector<VertexBoneData> bones = {})
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;
            this->bones = bones;

            if (!bones.empty()) hasBone = true;

            Setup();
        }

        void Draw(Shader& shader)
        {
            BindTexture(shader);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);

        }

        void DrawInstance(Shader& shader, int amount)
        {
            BindTexture(shader);
            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }

        void ConfigureInstance()
        {
            glBindVertexArray(VAO);
            // vertex attributes
            std::size_t vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }

    private:

        void BindTexture(Shader& shader)
        {
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                std::string number;
                std::string name = textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++);
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++);
                else if (name == "texture_height")
                    number = std::to_string(heightNr++);

                glUniform1i(glGetUniformLocation(shader.GetID(), (name + number).c_str()), i);
                glBindTexture(GL_TEXTURE_2D, textures[i].ID);
            }
        }

        void Setup() override
        {
            //vertices data
            glGenBuffers(1, &VBO_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (hasBone)
            {
                ////bones data
                glGenBuffers(1, &VBO_bones);
                glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
                glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(bones[0]), &bones[0], GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            
            //numbers for sequence indices
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // create VAO and binding data from buffers to shaders
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
            //vertex position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
            glEnableVertexAttribArray(1); // offsetof(Vertex, normal) = returns the byte offset of that variable from the start of the struct
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (hasBone)
            {
                //bones
                glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
                glEnableVertexAttribArray(3);
                glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (GLvoid*)0); // for INT Ipointer
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (GLvoid*)offsetof(VertexBoneData, weights));
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            //indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBindVertexArray(0);
        }
    };
}