
namespace Bamtang
{
    class ObjectDynamic
    {
    private:

        static const GLuint MAX_BONES = 100;
        static const GLuint MAX_ANIMATIONS = 20;

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 transform;

        std::vector<Texture> textures;
        std::vector<Texture> textures_loaded;
        std::vector<MeshComponent*> meshes;

        std::string directory;
        bool textureAssimp;

		Assimp::Importer import[MAX_ANIMATIONS];
        const aiScene* scene[MAX_ANIMATIONS];

        std::unordered_map<std::string, GLuint> mapBone;
        GLuint numBones = 0;
        std::vector<BoneMatrix> matrixBone;
        aiMatrix4x4 matrixInverse;

        GLuint boneID;
        float ticksPerSecond = 0.0f;
        float sensitivity = 1.0f;

        float startFrame;
        float animationTime;
        //float frameTime;
        bool pause = false;

        std::unordered_map<std::string, const aiNodeAnim*> mapNodeAnim[MAX_ANIMATIONS];

        std::vector<const aiAnimation*> animations;
        std::vector<aiNode*> root;

        GLuint numAnimations = 0;
        GLuint cantidad;


    public:
        int currentAnimation;

        ObjectDynamic(std::string const& path, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f))
        {
            this->position = position;
            this->scale = scale;
            this->rotation = rotation;
			textureAssimp = true;

            

            loadModel(path);

			//boneID = glGetUniformLocation(shader.GetID(), "bones[0]");

			this->startFrame = glfwGetTime();


        }

        ~ObjectDynamic() {}


		void render(Camera& camera, Shader& shader)
		{
			shader.Use();
			glm::mat4 projection = camera.GetProjectionMatrix();
			glm::mat4 view = camera.GetViewMatrix();

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

			//glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(transform)));

			draw(shader);
		}

		void draw(Shader& shader)
		{
			std::vector<aiMatrix4x4> transforms;

			boneTransform((double)animationTime, transforms);
			glUniformMatrix4fv(boneID, transforms.size(), GL_TRUE, (const GLfloat*)&transforms[0]);

			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i]->Render(shader);
		}
		

		virtual void update()
		{

		}

		void updateTime(float currentFrame)
		{
			if (!pause)
				animationTime = currentFrame - startFrame;
		}

		void addAnimation(std::string const& path)
		{
			numAnimations++;
			const std::string ruta =/* directory + "/" +*/ path;

			scene[numAnimations] = import[numAnimations].ReadFile(ruta,
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_LimitBoneWeights);

			if (!scene[numAnimations] || scene[numAnimations]->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
			{
				std::cout << "error assimp : " << import[numAnimations].GetErrorString() << "\n";
				return;
			}


			for (GLuint i = 0; i < scene[numAnimations]->mNumAnimations; i++)
			{
				const aiAnimation* animation = scene[numAnimations]->mAnimations[i];
				animations.push_back(animation);
				root.push_back(scene[numAnimations]->mRootNode);
				int index = animations.size() - 1;
				for (GLuint j = 0; j < animation->mNumChannels; j++)
				{
					const aiNodeAnim* node_anim = animation->mChannels[j];
					mapNodeAnim[index][std::string(node_anim->mNodeName.data)] = node_anim;
				}
			}
		}



        

		void loadModel(std::string const& path)
		{

			scene[0] = import[0].ReadFile(path,
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_LimitBoneWeights);

			if (!scene[0] || scene[0]->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene[0]->mRootNode)
			{
				std::cout << "error assimp : " << import[0].GetErrorString() << "\n";
				return;
			}

			

			directory = path.substr(0, path.find_last_of('/'));
			processNode(scene[0]->mRootNode, scene[0]);

			matrixInverse = scene[0]->mRootNode->mTransformation;
			matrixInverse.Inverse();

			loadAnimations();

			glBindVertexArray(0);
		}

		void loadAnimations()
		{
			for (GLuint i = 0; i < scene[0]->mNumAnimations; i++)
			{
				numAnimations++;
				const aiAnimation* animation = scene[0]->mAnimations[i];
				animations.push_back(animation);
				int index = animations.size() - 1;
				root.push_back(scene[0]->mRootNode);
				for (GLuint j = 0; j < animation->mNumChannels; j++)
				{
					const aiNodeAnim* node_anim = animation->mChannels[j];
					mapNodeAnim[index][std::string(node_anim->mNodeName.data)] = node_anim;
				}
			}
		}

		void processNode(aiNode* node, const aiScene* scene)
		{
			//MeshComponent mesh;
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene. 
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		MeshComponent* processMesh(aiMesh* mesh, const aiScene* scene)
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

				if (mapBone.find(bone_name) == mapBone.end())
				{
					bone_index = numBones;
					numBones++;
					BoneMatrix bi;
					matrixBone.push_back(bi);
					matrixBone[bone_index].offsetMatrix = mesh->mBones[i]->mOffsetMatrix;
					mapBone[bone_name] = bone_index;
				}
				else
				{
					bone_index = mapBone[bone_name];
				}

				for (GLuint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
				{
					GLuint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
					float weight = mesh->mBones[i]->mWeights[j].mWeight;
					bones[vertex_id].AddBoneData(bone_index, weight);
				}
			}

			if (textureAssimp)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
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

				return new MeshComponent(vertices, indices, textures, bones);
			}

			return new MeshComponent(vertices, indices, this->textures, bones);
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

		GLuint findPosition(float animationTime, const aiNodeAnim* nodeAnim)
		{
			for (GLuint i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
			{
				if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
				{
					return i;
				}
			}
			assert(0);
			return 0;
		}

		GLuint findRotation(float animationTime, const aiNodeAnim* nodeAnim)
		{
			assert(nodeAnim->mNumRotationKeys > 0);
			for (GLuint i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
			{
				if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
				{
					return i;
				}
			}
			assert(0);
			return 0;
		}

		GLuint findScaling(float animationTime, const aiNodeAnim* nodeAnim)
		{
			for (GLuint i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
			{
				if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
				{
					return i;
				}
			}
			assert(0);
			return 0;
		}

		aiVector3D calcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim)
		{
			if (nodeAnim->mNumPositionKeys == 1)
				return nodeAnim->mPositionKeys[0].mValue;

			GLuint position_index = findPosition(animationTime, nodeAnim);
			GLuint next_position_index = position_index + 1;
			assert(next_position_index < nodeAnim->mNumPositionKeys);

			float deltaTime = (float)(nodeAnim->mPositionKeys[next_position_index].mTime - nodeAnim->mPositionKeys[position_index].mTime);
			float factor = (animationTime - (float)nodeAnim->mPositionKeys[position_index].mTime) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = nodeAnim->mPositionKeys[position_index].mValue;
			aiVector3D end = nodeAnim->mPositionKeys[next_position_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}

		aiQuaternion calcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim)
		{
			if (nodeAnim->mNumRotationKeys == 1)
				return nodeAnim->mRotationKeys[0].mValue;

			GLuint rotation_index = findRotation(animationTime, nodeAnim);
			GLuint next_rotation_index = rotation_index + 1;
			assert(next_rotation_index < nodeAnim->mNumRotationKeys);

			float deltaTime = (float)(nodeAnim->mRotationKeys[next_rotation_index].mTime - nodeAnim->mRotationKeys[rotation_index].mTime);
			float factor = (animationTime - (float)nodeAnim->mRotationKeys[rotation_index].mTime) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiQuaternion startQuat = nodeAnim->mRotationKeys[rotation_index].mValue;
			aiQuaternion endQuat = nodeAnim->mRotationKeys[next_rotation_index].mValue;

			return nlerp(startQuat, endQuat, factor);
		}

		aiVector3D calcInterpolatedScaling(float animationTime, const aiNodeAnim* nodeAnim)
		{
			if (nodeAnim->mNumScalingKeys == 1)
				return nodeAnim->mScalingKeys[0].mValue;

			GLuint scaling_index = findScaling(animationTime, nodeAnim);
			GLuint next_scaling_index = scaling_index + 1;
			assert(next_scaling_index < nodeAnim->mNumScalingKeys);

			float deltaTime = (float)(nodeAnim->mScalingKeys[next_scaling_index].mTime - nodeAnim->mScalingKeys[scaling_index].mTime);
			float  factor = (animationTime - (float)nodeAnim->mScalingKeys[scaling_index].mTime) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);
			aiVector3D start = nodeAnim->mScalingKeys[scaling_index].mValue;
			aiVector3D end = nodeAnim->mScalingKeys[next_scaling_index].mValue;
			aiVector3D delta = end - start;

			return start + factor * delta;
		}

		void readNodeHierarchy(float animationTime, const aiNode* node, const aiMatrix4x4 parentTransform)
		{
			std::string nodeName(node->mName.data);
			const aiAnimation* animation = animations[currentAnimation];
			aiMatrix4x4 nodeTransform = node->mTransformation;
			const aiNodeAnim* nodeAnim = mapNodeAnim[currentAnimation][nodeName];

			if (nodeAnim)
			{

				aiVector3D scale = calcInterpolatedScaling(animationTime, nodeAnim);
				aiMatrix4x4 matrixScale;
				aiMatrix4x4::Scaling(scale, matrixScale);

				aiQuaternion rotation = calcInterpolatedRotation(animationTime, nodeAnim);
				aiMatrix4x4 matrixRotation = aiMatrix4x4(rotation.GetMatrix());

				aiVector3D translate = calcInterpolatedPosition(animationTime, nodeAnim);
				aiMatrix4x4 matrixTranslate;
				aiMatrix4x4::Translation(translate, matrixTranslate);

				nodeTransform = matrixTranslate * matrixRotation * matrixScale;

			}

			aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

			if (mapBone.find(nodeName) != mapBone.end())
			{
				GLuint boneIndex = mapBone[nodeName];
				matrixBone[boneIndex].finalWorldTransform = matrixInverse * globalTransform * matrixBone[boneIndex].offsetMatrix;
			}

			for (GLuint i = 0; i < node->mNumChildren; i++)
				readNodeHierarchy(animationTime, node->mChildren[i], globalTransform);

		}

		void boneTransform(double second, std::vector<aiMatrix4x4>& transforms)
		{
			aiMatrix4x4 identityMatrix;

			const aiAnimation* animation = animations[currentAnimation];

			if (animation->mTicksPerSecond != 0.0)
				ticksPerSecond = animation->mTicksPerSecond;
			else
				ticksPerSecond = 25.0f;

			double timeInTicks = second * ticksPerSecond * sensitivity;
			float animationTime = fmod(timeInTicks, animation->mDuration);

			readNodeHierarchy(animationTime, root[currentAnimation], identityMatrix);

			transforms.resize(numBones);

			for (GLuint i = 0; i < numBones; i++)
				transforms[i] = matrixBone[i].finalWorldTransform;

		}

		glm::mat4 aiToGlm(aiMatrix4x4 matrix)
		{
			glm::mat4 result;
			result[0].x = matrix.a1; result[0].y = matrix.b1; result[0].z = matrix.c1; result[0].w = matrix.d1;
			result[1].x = matrix.a2; result[1].y = matrix.b2; result[1].z = matrix.c2; result[1].w = matrix.d2;
			result[2].x = matrix.a3; result[2].y = matrix.b3; result[2].z = matrix.c3; result[2].w = matrix.d3;
			result[3].x = matrix.a4; result[3].y = matrix.b4; result[3].z = matrix.c4; result[3].w = matrix.d4;

			return result;
		}

		aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend)
		{
			a.Normalize();
			b.Normalize();

			aiQuaternion result;
			float dotProduct = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
			float oneMinusBlend = 1.0f - blend;

			if (dotProduct < 0.0f)
			{
				result.x = a.x * oneMinusBlend + blend * -b.x;
				result.y = a.y * oneMinusBlend + blend * -b.y;
				result.z = a.z * oneMinusBlend + blend * -b.z;
				result.w = a.w * oneMinusBlend + blend * -b.w;
			}
			else
			{
				result.x = a.x * oneMinusBlend + blend * b.x;
				result.y = a.y * oneMinusBlend + blend * b.y;
				result.z = a.z * oneMinusBlend + blend * b.z;
				result.w = a.w * oneMinusBlend + blend * b.w;
			}

			return result.Normalize();
		}

    };

}


