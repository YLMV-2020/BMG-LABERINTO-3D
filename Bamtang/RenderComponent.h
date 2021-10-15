
namespace Bamtang
{
    class RennderComponent : public IObject
    {
    private:

        std::vector<Texture> textures;
        std::vector<MeshComponent*> meshes;
        std::string directory;
        bool gammaCorrection;
        bool hasBone = false;

        AnimatorComponent* m_animator = NULL;

    public:


        RennderComponent(std::string const& path, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f), bool gamma = false) : gammaCorrection(gamma)
        {
            this->position = position;
            this->rotation = rotation;
            this->scale = scale;

            LoadModel(path);
        }



        ~RennderComponent() {}

        void AddAnimator(AnimatorComponent* animator)
        {
            this->m_animator = animator;
        }

        void Draw(Shader& shader) override
        {
            if (hasBone) m_animator->Draw(shader);

            for (unsigned int i = 0; i < meshes.size(); i++)
                meshes[i]->Render(shader);
        }

        void Render(Camera& camera, Shader& shader) override
        {
            shader.Use();

            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = camera.GetProjectionMatrix();

            transform = glm::mat4(1.0f);

            transform = glm::translate(transform, position);

            transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

            transform = glm::scale(transform, scale);

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

        void UpdateTime(float currentFrame)
        {
            m_animator->UpdateTime(currentFrame);
        }

        void BindBoneID(Shader& shader)
        {
            m_animator->BindBoneID(shader);
        }

    private:
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
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



        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void ProccesNode(aiNode* node, const aiScene* scene)
        {
            // process each mesh located at the current node
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                // the node object only contains indices to index the actual objects in the scene. 
                // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(ProcessMesh(mesh, scene));
            }
            // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
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
                // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                bool skip = false;
                for (unsigned int j = 0; j < textures.size(); j++)
                {
                    if (std::strcmp(textures[j].path.data(), str.C_Str()) == 0)
                    {
                        texturesLoad.push_back(textures[j]);
                        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                        break;
                    }
                }
                if (!skip)
                {   // if texture hasn't been loaded already, load it
                    Texture texture;
                    std::cout << typeName << "\n";
                    texture.ID = ResourceManager::Instance()->LoadTexture(typeName, str.C_Str(), this->directory);
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    texturesLoad.push_back(texture);
                    textures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                }
            }
            return texturesLoad;
        }



        void Start() override
        {

        }

    };

}


