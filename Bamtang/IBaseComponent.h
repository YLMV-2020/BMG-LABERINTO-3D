#include "IBaseMessage.h"

namespace Bamtang
{
	class IBaseComponent : public IBaseMessage
	{
	public:
		IBaseComponent() {}

		virtual bool SendMessage(IBaseMessage* msg) { return false; }
	};
}