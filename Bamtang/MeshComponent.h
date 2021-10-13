#include "IBaseComponent.h"

namespace Bamtang
{
    class MeshComponent : public IBaseComponent
    {
    public:

        bool SendMessage(IBaseMessage* msg) override
        {
            // Object has a switch for any messages it cares about
            switch (msg->m_messageTypeID)
            {
            case Mesh_Static:
            {
                // Update render mesh position/translation

                //std::cout << "RenderComponent handling SetPosition\n";
            }
            break;
            default:
                return IBaseComponent::SendMessage(msg);
            }

            return true;
        }
    };
}