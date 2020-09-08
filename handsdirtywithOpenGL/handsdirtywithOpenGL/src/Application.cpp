#include <glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char *function, const char *file, int line)
{
	while (GLenum error = glGetError()) 
	{
		std::cout << "[OpenGL error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}
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
	/*glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/
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

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "oops" << std::endl;
		glfwTerminate();
		
	}
	
	std::cout << glGetString(GL_VERSION) << std::endl;

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

	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	ShaderProgramSource source = parseShader("res/shaders/basic.shader");
	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_Color");
	ASSERT(location != -1);
	glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);

	float r = 0.0f;
	float increment = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform4f(location, r, 0.03f, 0.8f, 1.0f);
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
	//glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}