#include <glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;

};
static ShaderProgramSource parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::string line;
	std::stringstream ss[2];

	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos) 
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
		
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int compileShader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == 0)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* query = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, query);
		std::cout << query << std::endl;
		glDeleteShader(id);
		return 0;
	}
	
	return id;

}
static unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader)
{
	unsigned int prog = glCreateProgram();
	unsigned int vS = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fS = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(prog, vS);
	glAttachShader(prog, fS);
	glLinkProgram(prog);
	glValidateProgram(prog);
	glDeleteShader(vS);
	glDeleteShader(fS);
 	return prog;
 }
 

int main(void)
{ 
	//glfwInit();
	GLFWwindow* window;
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	

	

	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "oops" << std::endl;
		glfwTerminate();
	}
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		const GLfloat positions[] = {
		  -0.5f, -0.5f,  //0
		   0.5f, -0.5f,  //1
		   0.5f, 0.5f,   //2
		  -0.5f, 0.5f,   //3
		};

		unsigned int indices[] = {
			  0, 1, 2,
			  2, 3, 0
		};
		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexBuffer vb(positions, sizeof(positions));

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		IndexBuffer ib(indices, 6);

		ShaderProgramSource source = parseShader("res/shaders/basic.shader");
		unsigned int shader = createShader(source.vertexSource, source.fragmentSource);
		GLCall(glUseProgram(shader));

		int location = glGetUniformLocation(shader, "u_Color");
		ASSERT(location != -1);
		GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		float r = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(shader);
			glUniform4f(location, r, 0.03f, 0.8f, 1.0f);
			glBindVertexArray(vao);

			ib.Bind();
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;

			r += increment;


			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
		GLCall(glDeleteProgram(shader));
	}
	
	glfwTerminate();
	return 0;
};