#include "Shader.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

bool Shader::hasUboMatrices;

Shader::Shader(string shadername)
{
	program = makeProgram(shadername);

	if (Shader::hasUboMatrices != true)
	{
		// Predefined set of uniforms to pass to all shaders
		glGenBuffers(1, &uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(mat4));

		Shader::hasUboMatrices = true;
	}
}

Shader::~Shader()
{

}

void Shader::update(float dt, Camera* camera)
{
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(camera->getMatProjection()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(camera->getMatView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

GLuint Shader::getProgram() const
{
	return program;
}

// Read and return the contents of a file.
string Shader::readFile(string filename)
{
	ifstream fileStream(filename, ios::in);
	if (!fileStream.is_open())
	{
		cout << "Could not open file: " << filename << endl;
		return "";
	}

	stringstream strStream;
	strStream << fileStream.rdbuf();
	fileStream.close();

	return strStream.str();
}

// Load, compile and link shader and return program id.
GLuint Shader::makeProgram(string name)
{
	// Read and compile shader files
	string vs = readFile(g_mediaDirectory + "shader/" + name + ".vert");
	string fs = readFile(g_mediaDirectory + "shader/" + name + ".frag");
	const GLchar* vertSrc = vs.c_str();
	const GLchar* fragSrc = fs.c_str();
	if (vertSrc == "" || fragSrc == "") return 0;

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertShader, 1, &vertSrc, NULL);
	glShaderSource(fragShader, 1, &fragSrc, NULL);
	glCompileShader(vertShader);
	glCompileShader(fragShader);

	// Error checking
	GLint success;

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logLength;
		glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
		vector<GLchar> infoLog(logLength);
		glGetShaderInfoLog(vertShader, logLength, &logLength, &infoLog[0]);
		cout << "=== VERTEX SHADER ERROR ===" << endl;
		cout << &infoLog[0] << endl;
		glDeleteShader(vertShader);
		return 0;
	}

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logLength;
		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
		vector<GLchar> infoLog(logLength);
		glGetShaderInfoLog(fragShader, logLength, &logLength, &infoLog[0]);
		cout << "=== FRAGMENT SHADER ERROR ===" << endl;
		cout << &infoLog[0] << endl;
		glDeleteShader(fragShader);
		return 0;
	}

	// Link shaders
	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	// Linking error checking
	glGetShaderiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		vector<GLchar> infoLog(logLength);
		glGetProgramInfoLog(program, logLength, &logLength, &infoLog[0]);
		cout << "=== LINKING SHADER ERROR ===" << endl;
		cout << &infoLog[0] << endl;
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(program);
		return 0;
	}

	/*cout << "Shader \"" << name << "\" compiled" << endl;
	Object::vecShaders.push_back(make_pair(program, name));*/

	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);

	return program;
}