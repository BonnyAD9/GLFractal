#pragma once
#include <string>
#include <vector>
#include <map>

#include "Vectors.h"

using std::string, std::vector, std::map;

/// <summary>
/// Represents error codes of this font
/// </summary>
enum class FontTextureStatus
{
    OK = 0,
    FREETYPE_NOT_LOADED,
    FONT_NOT_LOADED,
    SOME_CHARACTERS_MISSING,
};

/// <summary>
/// Holds info about a character (position in texture and parameters to properly draw it)
/// </summary>
struct Character
{
    /// <summary>
    /// Position of topleft corner of this character in the font texture
    /// </summary>
    IVec2 position;
    /// <summary>
    /// Size of the character (width, height) in pixels
    /// </summary>
    IVec2 size;
    /// <summary>
    /// Relative position of the character to the current position in pixels
    /// </summary>
    IVec2 bearing;
    /// <summary>
    /// How much to advance the currrent position in 1/64 pixels
    /// </summary>
    unsigned int advance;
};

/// <summary>
/// Creates font texture and info about each loaded character
/// </summary>
class FontTexture
{
public:
    /// <summary>
    /// Creates valid FontTexture based on font specified by its path, and font size
    /// </summary>
    FontTexture(string path, int charHeight);
    /// <summary>
    /// Creates empty, invalid FontTexture
    /// </summary>
    FontTexture();
    FontTexture operator=(FontTexture font);
    /// <summary>
    /// Gets pointer to texture containing all characters
    /// </summary>
    /// <returns>Grayscale character texture</returns>
    unsigned char* image();
    Character operator[](char c);
    /// <summary>
    /// Gets width of the generated texture
    /// </summary>
    /// <returns>Width of the generated texture in pixels</returns>
    int width();
    /// <summary>
    /// Gets height of the generated texture
    /// </summary>
    /// <returns>Height of the generated texture in pixels</returns>
    int height();
    /// <summar>
    /// Deletes the generated texture, (not info about characters)
    /// After this call behaviour of the image() function is undefined
    /// </summary>
    void freeImage();
    /// <summary>
    /// Gets the status about this FontTexture (0 = OK)
    /// </summary>
    FontTextureStatus status();
private:
    vector<unsigned char> _data;
    std::map<char, Character> _characters;
    FontTextureStatus _status;
    unsigned char& _getData(int x, int y);
    int _width;
    int _height;
};
