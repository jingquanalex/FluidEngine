#pragma once

#include "globalIncludes.h"
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

// Code: http://learnopengl.com/#!In-Practice/Text-Rendering

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

public:

	Font();
	~Font();

	void load(std::string fontname);

};