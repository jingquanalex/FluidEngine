#pragma once

#include "Includes.h"
#include "Shader.h"
#include "Quad.h"
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

// Code Credit: http://learnopengl.com/#!In-Practice/Text-Rendering

class Font
{

	struct Character 
	{
		GLuint     TextureID;  // ID handle of the glyph texture
		glm::ivec2 Size;       // Size of glyph
		glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
		GLuint     Advance;    // Offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;
	Shader* shader;
	GLuint VAO, VBO;
	glm::mat4 projection;

	void loadChar(FT_Face face, char c);

public:

	Font();
	~Font();

	void load(std::string fontname);
	void updateProjection();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

};