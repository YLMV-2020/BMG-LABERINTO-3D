namespace Bamtang
{
    class AnimatorComponent
    {
        public:

        static const GLuint MAX_BONES = 100;
        static const GLuint MAX_ANIMATIONS = 10;

        const aiScene* scene[MAX_ANIMATIONS];
        Assimp::Importer import[MAX_ANIMATIONS];


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
        int currentAnimation;

        AnimatorComponent()
        {

        }

        void Draw(Shader& shader)
        {
            std::vector<aiMatrix4x4> transforms;

            BoneTransform((double)animationTime, transforms);
            glUniformMatrix4fv(boneID, transforms.size(), GL_TRUE, (const GLfloat*)&transforms[0]);
        }

        void BindBoneID(Shader& shader)
        {
            boneID = glGetUniformLocation(shader.GetID(), "bones[0]");
        }

        void UpdateTime(float currentFrame)
        {
            if (!pause)
                animationTime = currentFrame - startFrame;
        }

        void AddAnimation(std::string const& path)
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

      
        void LoadAnimations()
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

        GLuint FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
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

        GLuint FindRotation(float animationTime, const aiNodeAnim* nodeAnim)
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

        GLuint FindScaling(float animationTime, const aiNodeAnim* nodeAnim)
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

        aiVector3D CalcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim)
        {
            if (nodeAnim->mNumPositionKeys == 1)
                return nodeAnim->mPositionKeys[0].mValue;

            GLuint position_index = FindPosition(animationTime, nodeAnim);
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

        aiQuaternion CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim)
        {
            if (nodeAnim->mNumRotationKeys == 1)
                return nodeAnim->mRotationKeys[0].mValue;

            GLuint rotation_index = FindRotation(animationTime, nodeAnim);
            GLuint next_rotation_index = rotation_index + 1;
            assert(next_rotation_index < nodeAnim->mNumRotationKeys);

            float deltaTime = (float)(nodeAnim->mRotationKeys[next_rotation_index].mTime - nodeAnim->mRotationKeys[rotation_index].mTime);
            float factor = (animationTime - (float)nodeAnim->mRotationKeys[rotation_index].mTime) / deltaTime;

            assert(factor >= 0.0f && factor <= 1.0f);
            aiQuaternion startQuat = nodeAnim->mRotationKeys[rotation_index].mValue;
            aiQuaternion endQuat = nodeAnim->mRotationKeys[next_rotation_index].mValue;

            return ToLerp(startQuat, endQuat, factor);
        }

        aiVector3D CalcInterpolatedScaling(float animationTime, const aiNodeAnim* nodeAnim)
        {
            if (nodeAnim->mNumScalingKeys == 1)
                return nodeAnim->mScalingKeys[0].mValue;

            GLuint scaling_index = FindScaling(animationTime, nodeAnim);
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

        void ReadNodeHierarchy(float animationTime, const aiNode* node, const aiMatrix4x4 parentTransform)
        {
            std::string nodeName(node->mName.data);
            const aiAnimation* animation = animations[currentAnimation];
            aiMatrix4x4 nodeTransform = node->mTransformation;
            const aiNodeAnim* nodeAnim = mapNodeAnim[currentAnimation][nodeName];

            if (nodeAnim)
            {

                aiVector3D scale = CalcInterpolatedScaling(animationTime, nodeAnim);
                aiMatrix4x4 matrixScale;
                aiMatrix4x4::Scaling(scale, matrixScale);

                aiQuaternion rotation = CalcInterpolatedRotation(animationTime, nodeAnim);
                aiMatrix4x4 matrixRotation = aiMatrix4x4(rotation.GetMatrix());

                aiVector3D translate = CalcInterpolatedPosition(animationTime, nodeAnim);
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
                ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform);

        }

        void BoneTransform(double second, std::vector<aiMatrix4x4>& transforms)
        {
            aiMatrix4x4 identityMatrix;

            const aiAnimation* animation = animations[currentAnimation];

            if (animation->mTicksPerSecond != 0.0)
                ticksPerSecond = animation->mTicksPerSecond;
            else
                ticksPerSecond = 25.0f;

            double timeInTicks = second * ticksPerSecond * sensitivity;
            float animationTime = fmod(timeInTicks, animation->mDuration);

            ReadNodeHierarchy(animationTime, root[currentAnimation], identityMatrix);

            transforms.resize(numBones);

            for (GLuint i = 0; i < numBones; i++)
                transforms[i] = matrixBone[i].finalWorldTransform;

        }

        glm::mat4 AiToGlm(aiMatrix4x4 matrix)
        {
            glm::mat4 result;
            result[0].x = matrix.a1; result[0].y = matrix.b1; result[0].z = matrix.c1; result[0].w = matrix.d1;
            result[1].x = matrix.a2; result[1].y = matrix.b2; result[1].z = matrix.c2; result[1].w = matrix.d2;
            result[2].x = matrix.a3; result[2].y = matrix.b3; result[2].z = matrix.c3; result[2].w = matrix.d3;
            result[3].x = matrix.a4; result[3].y = matrix.b4; result[3].z = matrix.c4; result[3].w = matrix.d4;

            return result;
        }

        aiQuaternion ToLerp(aiQuaternion a, aiQuaternion b, float blend)
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