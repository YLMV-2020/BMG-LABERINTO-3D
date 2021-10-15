namespace Bamtang
{
    class TimeManager
    {
    private:

        float deltaTime;
        float lastFrame;

    public:

        static TimeManager* Instance()
        {
            static TimeManager instance;
            return &instance;
        }


        void SetTime(double time)
        {
            float currentFrame = time;

            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
        }

        float &GetDeltaTime()
        {
            return deltaTime;
        }
        
        float &GetLastFrame()
        {
            return lastFrame;
        }

    };
   
}