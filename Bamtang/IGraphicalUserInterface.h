
namespace Bamtang
{
    class IGraphicalUserInterface
    {
    public:

        virtual ~IGraphicalUserInterface() {}

        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Display() = 0;
    };
}
