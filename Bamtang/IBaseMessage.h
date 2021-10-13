#include "E_MessageType.h"

namespace Bamtang
{
    class IBaseMessage
    {
    protected: 
        IBaseMessage(int destinationObjectID, E_MessageType messageTypeID)
            : m_destObjectID(destinationObjectID)
            , m_messageTypeID(messageTypeID)
        {}

    public: // Normally this isn't public, just doing it to keep code small
        int m_destObjectID;
        E_MessageType m_messageTypeID;
    };
}