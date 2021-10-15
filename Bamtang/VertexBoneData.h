#define NUM_BONES_PER_VERTEX 4

namespace Bamtang
{
    struct VertexBoneData
    {
        GLuint ids[NUM_BONES_PER_VERTEX];
        float weights[NUM_BONES_PER_VERTEX];

        VertexBoneData()
        {
            memset(ids, 0, sizeof(ids));
            memset(weights, 0, sizeof(weights));
        }

        void AddBoneData(GLuint bone_id, float weight)
        {
            for (GLuint i = 0; i < NUM_BONES_PER_VERTEX; i++)
            {
                if (weights[i] == 0.0)
                {
                    ids[i] = bone_id;
                    weights[i] = weight;
                    return;
                }
            }
        }
    };
}
