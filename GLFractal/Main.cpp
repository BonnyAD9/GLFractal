#include <iostream>

#include "GLFractal.h"

using namespace std;
using namespace GLFractal;

void printMessage(GLFResult rm);

int main(int argc, char** args)
{
    GLFConfig config{ };

    GLFResult rm = GLFResult::OK;
    
    if ((rm = init(config)) != GLFResult::OK)
    {
        printMessage(rm);
        goto exit;
    }

    if ((rm = GLFractal::mainloop()) != GLFResult::OK)
    {
        printMessage(rm);
        goto exit;
    }

    exit:
    GLFractal::terminate();
}

void printMessage(GLFResult rm)
{
    switch (rm) 
    {
    case GLFResult::WINDOW_INIT_ERROR:
        cout << "Couldn't initialize window" << endl;
        break;
    case GLFResult::GLAD_INIT_ERROR:
        cout << "Couldn't initialize glad" << endl;
        break;
    case GLFResult::SHADER_INIT_ERROR:
        cout << "Failed to create shader" << endl;
        break;
    case GLFResult::TEXTURE_LOAD_ERROR:
        cout << "Failed to load texture" << endl;
        break;
    case GLFResult::INVALID_FRACTAL:
        cout << "Selected invalid fractal" << endl;
        break;
    default:
        break;
    }
}