#include "Shader.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

GLuint Shader::uboMatrices;
GLuint Shader::uboLighting;
vector<pair<GLuint, string>> Shader::programList;

void Shader::setupUniformBuffers()
{
	// Setup uniforms that all shaders can use
	if (uboMatrices == 0)
	{
		glGenBuffers(1, &uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
	}

	if (uboLighting == 0)
	{
		glGenBuffers(1, &uboLighting);
		glBindBuffer(GL_UNIFORM_BUFFER, uboLighting);
		glBufferData(GL_UNIFORM_BUFFER, 5 * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboLighting, 0, 5 * sizeof(vec4));
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLighting);
	}
}

Shader::Shader(string shadername)
{
	// Check program list for the shader, if unavailable, compile it.
	vector<pair<GLuint, string>>::iterator it = find_if(programList.begin(), programList.end(),
		[shadername](const pair<GLuint, string>& element){ return element.second == shadername; });

	if (it == programList.end())
	{
		program = makeProgram(shadername);
		if (program) programList.push_back(make_pair(program, shadername));
	}
	else
	{
		program = (*it).first;
	}
}

Shader::~Shader()
{

}

void Shader::update(float dt)
{
	
}

GLuint Shader::getProgram() const
{
	return program;
}

// Read and return the contents of a file.
string Shader::readFile(string filename)
{
	ifstream fileStream(filename, ios::in);
	if (fileStream.is_open())
	{
		stringstream strStream;
		strStream << fileStream.rdbuf();
		fileStream.close();

		return strStream.str();
	}

	return "";
}

// Load, compile and link shader and return program id.
GLuint Shader::makeProgram(string name)
{
	// Read and compile shader files
	string vspath = g_mediaDirectory + "shader/" + name + ".vert";
	string fspath = g_mediaDirectory + "shader/" + name + ".frag";
	string gspath = g_mediaDirectory + "shader/" + name + ".gs";
	string vs = readFile(vspath);
	string fs = readFile(fspath);
	string gs = readFile(gspath);
	const GLchar* vertSrc = vs.c_str();
	const GLchar* fragSrc = fs.c_str();
	const GLchar* geomSrc = gs.c_str();

	if (vs == "") cout << "Cannot open shader: " << vspath << endl;
	if (fs == "") cout << "Cannot open shader: " << fspath << endl;
	if (vs == "" || fs == "") return 0;

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

	// Compile and link geometry shader if available
	GLuint geomShader;

	if (gs != "")
	{
		geomShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geomShader, 1, &geomSrc, NULL);
		glCompileShader(geomShader);

		glGetShaderiv(geomShader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint logLength;
			glGetShaderiv(geomShader, GL_INFO_LOG_LENGTH, &logLength);
			vector<GLchar> infoLog(logLength);
			glGetShaderInfoLog(geomShader, logLength, &logLength, &infoLog[0]);
			cout << "=== GEOMETRY SHADER ERROR ===" << endl;
			cout << &infoLog[0] << endl;
			glDeleteShader(geomShader);
			return 0;
		}

		glAttachShader(program, geomShader);
	}

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
		if (gs != "") glDeleteShader(geomShader);
		glDeleteProgram(program);
		return 0;
	}

	/*cout << "Shader \"" << name << "\" compiled" << endl;
	Object::vecShaders.push_back(make_pair(program, name));*/

	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);
	if (gs != "") glDetachShader(program, geomShader);

	return program;
}