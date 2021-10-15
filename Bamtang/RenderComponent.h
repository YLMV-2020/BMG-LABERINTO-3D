
namespace Bamtang
{
    class RenderComponent : public IBaseComponent
    {
    private:

        std::vector<Texture> textures;
        std::vector<MeshComponent*> meshes;

        std::string directory;
        glm::mat4 transform;

        bool gammaCorrection;
        bool hasBone = false;

        AnimatorComponent* m_animator = NULL;

    public:

        RenderComponent(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
        {
            LoadModel(path);

        }

        ~RenderComponent() {}

        bool SendMessage(IBaseMessage* msg) override { return false; }

        void Render(Shader& shader) override
        {

        }

        void AddAnimator(AnimatorComponent* animator)
        {
            this->m_animator = animator;
        }

        void Draw(Shader& shader) 
        {
            if (hasBone) m_animator->Draw(shader);

            for (unsigned int i = 0; i < meshes.size(); i++)
                meshes[i]->Render(shader);
        }

        void Render(Camera& camera, Shader& shader) 
        {
            shader.Use();

            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = camera.GetProjectionMatrix();

            //shader.SetVec3("viewPos", camera.GetPosition());

            shader.SetMat4("projection", projection);
            shader.SetMat4("view", view);
            shader.SetMat4("model", transform);

            Draw(shader);

        }

        void Update(glm::mat4 transform) override
        {
            this->transform = transform;
        }

        void UpdateTime(float &currentFrame)
        {
            m_animator->UpdateTime(currentFrame);
        }

        void BindBoneID(Shader& shader)
        {
            m_animator->BindBoneID(shader);
        }

    private:
        
        void LoadModel(std::string const& path)
        {

            AddAnimator(new AnimatorComponent());
            
            m_animator->scene[0] = m_animator->import[0].ReadFile(path,
                aiProcess_JoinIdenticalVertices |
                aiProcess_SortByPType |
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs |
                aiProcess_LimitBoneWeights);

            if (!m_animator->scene[0] || m_animator->scene[0]->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_animator->scene[0]->mRootNode)
            {
                std::cout << "ERROR::ASSIMP:: " << m_animator->import[0].GetErrorString() << "\n";
                return;
            }

            this->hasBone = m_animator->scene[0]->mMeshes[0]->mNumBones > 0 ? true : false;
            const aiScene* scene = m_animator->scene[0];
            std::cout << "Has Bone: " << hasBone << "\n";
            
            directory = path.substr(0, path.find_last_of('/'));
            ProccesNode(scene->mRootNode, scene);

            if (hasBone)
            {
                m_animator->matrixInverse = m_animator->scene[0]->mRootNode->mTransformation;
                m_animator->matrixInverse.Inverse();

                m_animator->startFrame = glfwGetTime();
                m_animator->LoadAnimations();
            }
            else
            {
                delete m_animator;
                m_animator = NULL;
                //m_animator->boneID = glGetUniformLocation(shader.GetID(), "bones[0]");
            }

            glBindVertexArray(0);
        }

        void ProccesNode(aiNode* node, const aiScene* scene)
        {
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(ProcessMesh(mesh, scene));
            }
            
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                ProccesNode(node->mChildren[i], scene);
            }
        }

        MeshComponent* ProcessMesh(aiMesh* mesh, const aiScene* scene)
        {
            std::vector<Vertex> vertices;
            std::vector<GLuint> indices;
            std::vector<Texture> textures;
            std::vector<VertexBoneData> bones;

            vertices.reserve(mesh->mNumVertices);
            indices.reserve(mesh->mNumVertices);
            bones.resize(mesh->mNumVertices);

            for (GLuint i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.position = vector;

                if (mesh->mNormals != NULL)
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.normal = vector;
                }
                else
                {
                    vertex.normal = glm::vec3(1.0f);
                }

                if (mesh->mTextureCoords[0])
                {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.texCoords = vec;
                }
                else
                {
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                }
                vertices.push_back(vertex);
            }

            for (GLuint i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            for (GLuint i = 0; i < mesh->mNumBones; i++)
            {
                GLuint bone_index = 0;
                std::string bone_name(mesh->mBones[i]->mName.data);

                if (m_animator->mapBone.find(bone_name) == m_animator->mapBone.end())
                {
                    bone_index = m_animator->numBones;
                    m_animator->numBones++;
                    BoneMatrix bi;
                    m_animator->matrixBone.push_back(bi);
                    m_animator->matrixBone[bone_index].offsetMatrix = mesh->mBones[i]->mOffsetMatrix;
                    m_animator->mapBone[bone_name] = bone_index;
                }
                else
                {
                    bone_index = m_animator->mapBone[bone_name];
                }

                for (GLuint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
                {
                    GLuint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
                    float weight = mesh->mBones[i]->mWeights[j].mWeight;
                    bones[vertex_id].AddBoneData(bone_index, weight);
                }
            }

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // 1. diffuse maps
            std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

            if (hasBone) return new MeshComponent(vertices, indices, textures, bones);

            return new MeshComponent(vertices, indices, textures);
        }

        std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
        {
            std::vector<Texture> texturesLoad;
            for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                
                bool skip = false;
                for (unsigned int j = 0; j < textures.size(); j++)
                {
                    if (std::strcmp(textures[j].path.data(), str.C_Str()) == 0)
                    {
                        texturesLoad.push_back(textures[j]);
                        skip = true; 
                        break;
                    }
                }
                if (!skip)
                {   
                    Texture texture;
                    std::cout << typeName << "\n";
                    texture.ID = ResourceManager::Instance()->LoadTexture(typeName, str.C_Str(), this->directory);
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    texturesLoad.push_back(texture);
                    textures.push_back(texture);  
                }
            }
            return texturesLoad;
        }

    };

}


