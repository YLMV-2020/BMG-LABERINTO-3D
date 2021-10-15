#include "IBaseMessage.h"

namespace Bamtang
{
	class IBaseComponent : public IBaseMessage
	{
	public:
		IBaseComponent() {}

		virtual bool SendMessage(IBaseMessage* msg) { return false; }

		virtual void Render(Shader& shader) = 0;
		virtual void Render(Camera& camera, Shader& shader) = 0;
		virtual void Update(glm::mat4 transform) = 0;
	};
}