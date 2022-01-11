#include "FontTexture.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

using std::pair;

FontTexture::FontTexture(string path, const int charHeight) : _characters(), _data(), _status(), _width(charHeight * 16)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        _status = FontTextureStatus::FREETYPE_NOT_LOADED;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, path.data(), 0, &face))
    {
        _status = FontTextureStatus::FONT_NOT_LOADED;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, charHeight);

    const int reserveSize = charHeight * _width;

    int posX = 0;
    int posY = 0;

    _data.reserve(reserveSize);

    // pre-rendering only first 128 ASCII characters
    for (char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            _status = FontTextureStatus::SOME_CHARACTERS_MUSSING;
            continue;
        }

        IVec2 size{ (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows };

        if (size.x + posX > _width)
        {
            _data.reserve(reserveSize);
            posX = 0;
            posY += charHeight;
        }

        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.x; x++)
                _getData(x + posX, y + posY) = face->glyph->bitmap.buffer[x + y * size.y];
        }

        Character ch{
            .position = IVec2{ posX, posY + size.y },
            .size = size,
            .bearing = IVec2{ face->glyph->bitmap_left, face->glyph->bitmap_top },
            .advance = (unsigned int)face->glyph->advance.x
        };

        _characters.insert(pair<char, Character>{c, ch});

        posX += size.x;
    }

    _height = posY + charHeight;

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

unsigned char& FontTexture::_getData(int x, int y)
{
    return _data[x + y * (size_t)_width];
}

unsigned char* FontTexture::image()
{
    return _data.data();
}

Character FontTexture::operator[](char c)
{
    return _characters[c];
}

int FontTexture::width()
{
    return _width;
}

int FontTexture::height()
{
    return _height;
}

FontTextureStatus FontTexture::status()
{
    return _status;
}