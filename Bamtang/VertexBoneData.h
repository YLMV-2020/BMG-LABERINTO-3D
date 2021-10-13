//namespace Bamtang
//{
//    struct VertexBoneData
//    {
//        const int NUM_BONES_PER_VERTEX = 4;
//        GLuint *id;
//        float *weights;
//
//        VertexBoneData()
//        {
//            id = new GLuint[NUM_BONES_PER_VERTEX];
//            weights = new float[NUM_BONES_PER_VERTEX];
//
//            memset(ids, 0, sizeof(ids));
//            memset(weights, 0, sizeof(weights));
//        }
//
//        void AddBoneData(GLuint boneId, float weight)
//        {
//            for (GLuint i = 0; i < NUM_BONES_PER_VERTEX; i++)
//            {
//                if (weights[i] == 0.0)
//                {
//                    id[i] = boneId;
//                    weights[i] = weight;
//                    return;
//                }
//            }
//        }
//    };
//}
