#include "Font.h"

using namespace glm;
using namespace std;

extern string g_mediaDirectory;

Font::Font()
{

}

Font::~Font()
{

}

void Font::load(string fontname)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, (g_mediaDirectory + fontname).c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);


}