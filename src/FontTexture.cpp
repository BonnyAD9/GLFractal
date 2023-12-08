#include "FontTexture.hpp"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

using std::pair;

FontTexture::FontTexture() : _data(), _characters(), _status(), _width(), _height() {}

FontTexture::FontTexture(string path, int charHeight) : _characters(), _data(), _status(), _width(charHeight * 16), _height(0)
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

    const int reserveSize = (charHeight + 1) * _width;

    int posX = 0;
    int posY = 0;

    _data.resize(reserveSize);

    // pre-rendering only first 128 ASCII characters
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Missing character '" << c << "'" << std::endl;
            _status = FontTextureStatus::SOME_CHARACTERS_MISSING;
            continue;
        }

        IVec2 size{ (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows};

        if (size.x + posX >= _width)
        {
            _data.resize(_data.size() + reserveSize);
            posX = 0;
            posY += charHeight;
        }

        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.x; x++)
                _getData(x + posX, y + posY) = face->glyph->bitmap.buffer[x + ((size.y - y - 1) * size.x)];
        }

        Character ch{
            .position = IVec2{ posX, posY },
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

void FontTexture::freeImage()
{
    _data.clear();
}

FontTexture FontTexture::operator=(FontTexture font)
{
    _data = font._data;
    _characters = font._characters;
    _status = font._status;
    _width = font._width;
    _height = font._height;
    return *this;
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
