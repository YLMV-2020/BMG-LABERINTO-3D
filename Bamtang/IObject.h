
namespace Bamtang
{
    class IObject
    {
    public:
        virtual ~IObject() {}

        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Draw() = 0;
    };
}
