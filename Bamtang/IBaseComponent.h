#include "IBaseMessage.h"

namespace Bamtang
{
	class IBaseComponent : public IBaseMessage
	{
	public:
		virtual bool SendMessage(IBaseMessage* msg) { return false; }
	};
}