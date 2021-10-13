namespace Bamtang
{
    class Ground
    {
    private:

		Shader shader;

		GLuint texture;
		unsigned int VAO, VBO;

		glm::vec3 volumen;
		float repeat;

		float planeVertices[30];

    public:

		Ground(std::string name, glm::vec3 volumen, float repeat)
		{
			this->shader = Shader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");
			this->volumen = volumen;
			this->repeat = repeat;

			std::string path = "assets\\textures\\terrain\\" + name;
			name = std::string(name.begin(), name.end() - 4);

			texture = ResourceManager::Instance()->LoadTexture(name, path.c_str());

			Configuration();
			ConfigurationShader();
		}

		void Render(Camera& camera, glm::vec3 ambient)
		{
			shader.Use();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = camera.GetProjectionMatrix();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);
			shader.SetVec3("ambient", ambient);

			glBindVertexArray(VAO);
			glBindTexture(GL_TEXTURE_2D, texture);
			shader.SetMat4("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

		}

	private:

		void Configuration()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);

			planeVertices[0] = volumen.x;
			planeVertices[1] = volumen.y;
			planeVertices[2] = volumen.z;
			planeVertices[3] = repeat;
			planeVertices[4] = 0.0f;

			planeVertices[5] = -volumen.x;
			planeVertices[6] = volumen.y;
			planeVertices[7] = volumen.z;
			planeVertices[8] = 0.0f;
			planeVertices[9] = 0.0f;

			planeVertices[10] = -volumen.x;
			planeVertices[11] = volumen.y;
			planeVertices[12] = -volumen.z;
			planeVertices[13] = 0.0f;
			planeVertices[14] = repeat;

			planeVertices[15] = volumen.x;
			planeVertices[16] = volumen.y;
			planeVertices[17] = volumen.z;
			planeVertices[18] = repeat;
			planeVertices[19] = 0.0f;

			planeVertices[20] = -volumen.x;
			planeVertices[21] = volumen.y;
			planeVertices[22] = -volumen.z;
			planeVertices[23] = 0.0f;
			planeVertices[24] = repeat;

			planeVertices[25] = volumen.x;
			planeVertices[26] = volumen.y;
			planeVertices[27] = -volumen.z;
			planeVertices[28] = repeat;
			planeVertices[29] = repeat;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glBindVertexArray(0);

		}

		void ConfigurationShader()
		{
			shader.Use();
			shader.SetInt("textureT", 0);
		}
        
    };
}
