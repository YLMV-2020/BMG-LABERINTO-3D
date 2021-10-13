
namespace Bamtang
{
    class Framebuffer
    {

    private:

        GLuint framebuffer;
        GLuint texture;

    public:

        Framebuffer(const int& WIDTH, const int& HEIGHT)
        {
            Bind(WIDTH, HEIGHT);
        }

        void Bind(const int& WIDTH, const int& HEIGHT)
        {
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        }

        const GLuint& Update()
        {
            return framebuffer;
        }

        const GLuint& Display()
        {
            return texture;
        }

    };

    

}