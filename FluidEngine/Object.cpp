#include "Object.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

vector<pair<GLuint, string>> Object::vecShaders;
vector<pair<GLuint, string>>::iterator Object::it;

Object::Object(vec3 position)
{
	this->position = position;
}

Object::~Object()
{

}

void Object::load(string shadername)
{
	this->shadername = shadername;
	program = makeProgram(shadername);

	if (this->shadername == "basic")
	{
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
		glBindVertexArray(0);

		locationMVP = glGetUniformLocation(program, "mvp");
	}
}

void Object::update(float dt, Camera* camera)
{
	// Apply object transformations
	matModel = translate(position);
	matMVP = camera->getMatViewProjection() * matModel;

	glUseProgram(program);
	glUniformMatrix4fv(locationMVP, 1, GL_FALSE, value_ptr(matMVP));
	glUseProgram(0);
}

void Object::draw()
{
	if (this->shadername == "basic")
	{
		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}

void Object::setPosition(vec3 position)
{
	this->position = position;
}

vec3 Object::getPosition() const
{
	return position;
}

// Read and return the contents of a file.
string Object::readFile(string filename)
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

// Get program id from string if same shader has already been compiled.
// Load, compile and link shader and return program id.
GLuint Object::makeProgram(string name)
{
	/*it = find_if(Object::vecShaders.begin(), Object::vecShaders.end(),
		[name](const pair<GLuint, string>& element) { return element.second == name; });

	if (it != Object::vecShaders.end())
	{
		printf("using dupe shader: %i %s \n", (*it).first, (*it).second.c_str());
		return (*it).first;
	}*/

	// Read and compile shader files
	string vs = readFile(g_mediaDirectory + name + ".vert");
	string fs = readFile(g_mediaDirectory + name + ".frag");
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

const GLfloat Object::triVertices[] =
{
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

const GLuint Object::triIndices[] =
{
	0, 1, 2
};

// Cube vertices, color, texcoord
const GLfloat Object::cubeVertices[] =
{
	// Front
	-0.5, -0.5, 0.5,
	0.5, -0.5, 0.5,
	0.5, 0.5, 0.5,
	-0.5, 0.5, 0.5,

	// Back
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5, 0.5, -0.5,
	-0.5, 0.5, -0.5
};

const GLuint Object::cubeIndices[] =
{
	// Front
	0, 1, 2,
	2, 3, 0,
	// Top
	3, 2, 6,
	6, 7, 3,
	// Back
	7, 6, 5,
	5, 4, 7,
	// Bottom
	4, 5, 1,
	1, 0, 4,
	// Left
	4, 0, 3,
	3, 7, 4,
	// Right
	1, 5, 6,
	6, 2, 1
};

const GLfloat Object::cubeColors[] =
{
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0,

	1.0, 1.0, 0.0,
	0.0, 1.0, 1.0,
	1.0, 0.0, 1.0,
	1.0, 1.0, 1.0
};