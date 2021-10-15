#include "E_MessageType.h"

namespace Bamtang
{
    class IBaseMessage
    {
    public:

        int m_destObjectID;
        E_MessageType m_messageTypeID;

        IBaseMessage() {}

    protected: 
        IBaseMessage(int destinationObjectID, E_MessageType messageTypeID)
            : m_destObjectID(destinationObjectID)
            , m_messageTypeID(messageTypeID)
        {}
    };
}