#pragma once
#include <string>
#include <vector>
#include <map>

#include "Vectors.h"

using std::string, std::vector, std::map;

enum class FontTextureStatus
{
    OK = 0,
    FREETYPE_NOT_LOADED,
    FONT_NOT_LOADED,
    SOME_CHARACTERS_MISSING,
};

struct Character
{
    IVec2 position;
    IVec2 size;
    IVec2 bearing;
    unsigned int advance;
};

class FontTexture
{
public:
    FontTexture(string path, int charHeight);
    FontTexture();
    FontTexture operator=(FontTexture font);
    unsigned char* image();
    Character operator[](char c);
    int width();
    int height();
    void freeImage();
    FontTextureStatus status();
private:
    vector<unsigned char> _data;
    std::map<char, Character> _characters;
    FontTextureStatus _status;
    unsigned char& _getData(int x, int y);
    int _width;
    int _height;
};
