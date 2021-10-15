namespace Bamtang
{
    class ResourceManager
    {

    private:

        std::map<std::string, GLuint> Textures;

    public:

        static ResourceManager* Instance()
        {
            static ResourceManager instance;
            return &instance;
        }

        GLuint LoadTexture(const std::string &name, const char* path, bool gammaCorrection = false)
        {
            Textures[name] = LoadTextureFromFile(path, gammaCorrection);
            return Textures[name];
        }

        GLuint LoadTexture(const std::string& name, const char* path, const std::string& directory, bool gammaCorrection = false)
        {
            Textures[name] = LoadTextureFromFile(path, directory, gammaCorrection);
            return Textures[name];
        }
        
        GLuint GetTexture(std::string name)
        {
            return Textures[name];
        }

        void Clear()
        {
            for (auto iter : Textures)
                glDeleteProgram(iter.second);
        }

    private:

        ResourceManager() { }

        GLuint LoadTextureFromFile(char const* path, bool gammaCorrection = false)
        {
            GLuint ID;
            glGenTextures(1, &ID);

            int width, height, nrChannels;
            unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
            if (data)
            {
                GLenum internalFormat;
                GLenum dataFormat;
                if (nrChannels == 1)
                    internalFormat = dataFormat = GL_RED;
                else if (nrChannels == 3)
                {
                    internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
                    dataFormat = GL_RGB;
                }
                else if (nrChannels == 4)
                {
                    internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
                    dataFormat = GL_RGBA;
                }

                glBindTexture(GL_TEXTURE_2D, ID);
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Failed to load texture: " << path << std::endl;
                stbi_image_free(data);
            }

            return ID;
        }

        GLuint LoadTextureFromFile(const char* path, const std::string& directory, bool gammaCorrection = false)
        {
            std::string filename = std::string(path);
            filename = directory + '/' + filename;

            unsigned int textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Texture failed to load at path: " << path << std::endl;
                stbi_image_free(data);
            }

            return textureID;
        }

    };
   
}