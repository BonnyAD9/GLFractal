#include <iostream>

#include "GLFractal.h"
#include "Parser.h"
#include "Gradient.h"

using namespace std;
using namespace GLFractal;
using namespace Parser;

void printMessage(GLFResult rm);
void printHelp();

int main(int argc, char** args)
{
    GLFConfig config{ };

    while (*++args)
    {
        string arg{ *args };
        // --help -h
        if (arg == "--help" || arg == "-h")
        {
            printHelp();
            return EXIT_SUCCESS;
        }
        // --scale -s
        else if (arg == "--scale" || arg == "-s")
        {
            double scale;
            if (!tryParse(*++args, &scale) || scale <= 0)
            {
                cout << "invalid scale argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.scale = 4 / scale;
        }
        // --center -c
        else if (arg == "--center" || arg == "-c")
        {
            double x;
            double y;
            if (!tryParse(*++args, &x))
            {
                cout << "invalid center x argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            if (!tryParse(*++args, &y))
            {
                cout << "invalid center y argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.center = DVec2{ -x, -y };
        }
        // --iterations -i
        else if (arg == "--iterations" || arg == "-i")
        {
            int iterations;
            if (!tryParse(*++args, &iterations) || iterations <= 0)
            {
                cout << "invalid iterations argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.iterations = iterations;
        }
        // --color -col
        else if (arg == "--color" || arg == "-col")
        {
            unsigned int color;
            if (!tryParseHex(*++args, &color))
            {
                cout << "invalid color argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            unsigned char* argb = (unsigned char*)&color;
            config.color = Vec3{
                argb[2] / 255.0f,
                argb[1] / 255.0f,
                argb[0] / 255.0f,
            };
        }
        // --number -n
        else if (arg == "--number" || arg == "-n")
        {
            double x;
            double y;
            if (!tryParse(*++args, &x))
            {
                cout << "invalid number x argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            if (!tryParse(*++args, &y))
            {
                cout << "invalid number y argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.constant = DVec2{ x, y };
        }
        // --color-count -cc
        else if (arg == "--color-count" || arg == "-cc")
        {
            float colorCount;
            if (!tryParse(*++args, &colorCount) || colorCount <= 0)
            {
                cout << "invalid color count argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.colorCount = colorCount;
        }
        // --sel-scale -ss
        else if (arg == "--sel-scale" || arg == "-ss")
        {
            float selScale;
            if (!tryParse(*++args, &selScale) || selScale <= 0)
            {
                cout << "invalid selector scale argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.selScale = 4 / selScale;
        }
        // --sel-center -sc
        else if (arg == "--sel-center" || arg == "-sc")
        {
            float x;
            float y;
            if (!tryParse(*++args, &x))
            {
                cout << "invalid selector center x argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            if (!tryParse(*++args, &y))
            {
                cout << "invalid selector center y argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.selCenter = Vec2{ -x, -y };
        }
        // --sel-iterations -si
        else if (arg == "--sel-iterations" || arg == "-si")
        {
            int selIterations;
            if (!tryParse(*++args, &selIterations) || selIterations <= 0)
            {
                cout << "invalid selector iterations argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.selIterations = selIterations;
        }
        // --sel-color-count -scc
        else if (arg == "--sel-color-count" || arg == "-scc")
        {
            float selColorCount;
            if (!tryParse(*++args, &selColorCount) || selColorCount <= 0)
            {
                cout << "invalid selector color count argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.selColorCount = selColorCount;
        }
        // --fractal -f
        else if (arg == "--fractal" || arg == "-f")
        {
            string val{ *++args };

            if (val == "mandelbrot" || val == "m")
                config.fractal = Fractal::MANDELBROT;
            else if (val == "julia" || val == "j")
                config.fractal = Fractal::JULIA;
            else if (val == "newton" || val == "n")
                config.fractal = Fractal::NEWTON;
            else if (val == "help" || val == "h")
                config.fractal = Fractal::HELP;
            else
            {
                cout << "invalid fractal argument '" << val << "'" << endl;
                return EXIT_FAILURE;
            }
        }
        // --use-double -d
        else if (arg == "--use-double" || arg == "-d")
        {
            config.useDouble = true;
        }
        // --bg-color -bg
        else if (arg == "--bg-color" || arg == "-bg")
        {
            unsigned int color;
            if (!tryParseHex(*++args, &color))
            {
                cout << "invalid background color argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            unsigned char* argb = (unsigned char*)&color;
            config.backgroundColor = Vec3{
                argb[2] / 255.0f,
                argb[1] / 255.0f,
                argb[0] / 255.0f,
            };
        }
        // --text-color -fg
        else if (arg == "--text-color" || arg == "-fg")
        {
            unsigned int color;
            if (!tryParseHex(*++args, &color))
            {
                cout << "invalid foreground color argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            unsigned char* argb = (unsigned char*)&color;
            config.textColor = Vec3{
                argb[2] / 255.0f,
                argb[1] / 255.0f,
                argb[0] / 255.0f,
            };
        }
        // --gradient -g
        else if (arg == "--gradient" || arg == "-g")
        {
            string val{ *++args };
            if (val == "ultra-fractal" || val == "uf")
                config.gradient = GradientPreset::ULTRA_FRACTAL;
            else if (val == "monokai" || val == "mk")
                config.gradient = GradientPreset::MONOKAI;
            else if (val == "grayscale" || val == "gs")
                config.gradient = GradientPreset::GRAYSCALE;
            else
            {
                cout << "invalid color gradient argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
        }
        // --font-path -fp
        else if (arg == "--font-path" || arg == "-fp")
        {
            config.fontPath = *++args;
        }
        // --font-size -fs
        else if (arg == "--font-size" || arg == "-fs")
        {
            int fontSize;
            if (!tryParse(*++args, &fontSize) || fontSize <= 0)
            {
                cout << "invalid font size argument '" << *args << "'" << endl;
                return EXIT_FAILURE;
            }
            config.fontSize = fontSize;
        }
        else
        {
            cout << "Invalid argument '" << arg << "'" << endl;
            return EXIT_FAILURE;
        }
    }

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
    case GLFResult::UNEXPECTED_ERROR:
        cout << "An unexpected error has occured" << endl;
        break;
    case GLFResult::FREETYPE_LOAD_ERROR:
        cout << "Failed to load FreeType" << endl;
        break;
    case GLFResult::FONT_LOAD_ERROR:
        cout << "Failed to load font" << endl;
        break;
    case GLFResult::SOME_CHARACTERS_MISSING:
        cout << "Some font characters failed to load" << endl;
    default:
        break;
    }
}

void printHelp()
{
    cout << "    Wellcome in GLFractal help!!\n";
    cout << "When running from console, should be executed from the same folder in which other program files are\n";
    cout << "\n";
    cout << "Usage:\n";
    cout << "  GLFractal.exe [flags]\n";
    cout << "\n";
    cout << "Flags: (all flags are optional, and the example shows default value, if not it is a switch)\n";
    cout << "  --help  -h\n";
    cout << "    shows this help page\n";
    cout << "\n";
    cout << "  --scale  -s\n";
    cout << "    sets scale of the main view\n";
    cout << "    glfractal -s 1.0\n";
    cout << "\n";
    cout << "  --center  -c\n";
    cout << "    sets position of center of the screen as a complex number\n";
    cout << "    glfractal -c 0.0 0.0\n";
    cout << "\n";
    cout << "  --iterations  -i\n";
    cout << "    sets the number of iterations for each pixel (newton fractal uses only 1/10)\n";
    cout << "    glfractal -i 1000\n";
    cout << "\n";
    cout << "  --color  -c\n";
    cout << "    sets color (in hex), usually used as max iterations color\n";
    cout << "    glfractal -c 000000\n";
    cout << "\n";
    cout << "  --number  -n\n";
    cout << "    sets special number for fractals (for example julia set uses it)\n";
    cout << "    glfractal -n 0.0 0.0\n";
    cout << "\n";
    cout << "  --color-count  -cc\n";
    cout << "    sets how many colors will be choosed from the gradient until they will repeat\n";
    cout << "    glfractal -cc 256\n";
    cout << "\n";
    cout << "  --sel-scale  -ss\n";
    cout << "    sets scale of the selector\n";
    cout << "    glfractal -ss 1.0\n";
    cout << "\n";
    cout << "  --sel-center  -sc\n";
    cout << "    sets center of the selector\n";
    cout << "    glfractal -sc 0.0 0.0\n";
    cout << "\n";
    cout << "  --sel-iterations  -si\n";
    cout << "    sets the number of iteratins for each pixel in selector\n";
    cout << "    glfractal -si 100\n";
    cout << "\n";
    cout << "  --sel-color-count  -scc\n";
    cout << "    sets the number of colors used by the selector\n";
    cout << "    glfractal -scc 128\n";
    cout << "\n";
    cout << "  --fractal  -f\n";
    cout << "    sets the fractal\n";
    cout << "    Possible values:\n";
    cout << "      mandelbrot | m\n";
    cout << "      julia      | j\n";
    cout << "      newton     | n\n";
    cout << "      help       | h\n";
    cout << "    glfractal -f m\n";
    cout << "\n";
    cout << "  --use-double  -d\n";
    cout << "    determines whether double floating point arithmetic should be used on the gpu (significantly decreases performance)\n";
    cout << "\n";
    cout << "  --bg-color  -bg\n";
    cout << "    sets the clear background color (in hex)\n";
    cout << "    glfractal -bg 191919\n";
    cout << "\n";
    cout << "  --text-colot  -fg\n";
    cout << "    sets the text color\n";
    cout << "    glfractal -fg E5E5E5\n";
    cout << "\n";
    cout << "  --gradient  -g\n";
    cout << "    sets gradient that will be used\n";
    cout << "    Possible values:\n";
    cout << "      ultra-fractal | uf\n";
    cout << "      monokai       | mk\n";
    cout << "      grayscale     | gs\n";
    cout << "    glfractal -g uf\n";
    cout << "\n";
    cout << "  --font-path  -fp\n";
    cout << "    sets the font\n";
    cout << "    glfractal -fp CascadiaCode.ttf\n";
    cout << "\n";
    cout << "  --font-size  -fs\n";
    cout << "    sets the font size\n";
    cout << "    glfractal -fs 40\n";
    cout << "\n";
    cout << "Key bindings:\n";
    cout << "  View (hold space to use it for selector):\n";
    cout << "    move center : LMB\n";
    cout << "    zoom in     : RMB up\n";
    cout << "    zoom out    : RMB down\n";
    cout << "\n";
    cout << "  Fractals:\n";
    cout << "    help           : F1\n";
    cout << "    mandelbrot set : 1\n";
    cout << "    julia set      : 2\n";
    cout << "    newton fractal : 3\n";
    cout << "\n";
    cout << "  Selector:\n";
    cout << "    choose point : LMB\n";
    cout << "\n";
    cout << "  Iterations (hold space to use it for selector):\n";
    cout << "    100    : Ctrl + 1\n";
    cout << "    200    : Ctrl + 2\n";
    cout << "    300    : Ctrl + 3\n";
    cout << "    400    : Ctrl + 4\n";
    cout << "    500    : Ctrl + 5\n";
    cout << "    600    : Ctrl + 6\n";
    cout << "    700    : Ctrl + 7\n";
    cout << "    800    : Ctrl + 8\n";
    cout << "    900    : Ctrl + 9\n";
    cout << "    1000   : Ctrl + Shift + 1\n";
    cout << "    2000   : Ctrl + Shift + 2\n";
    cout << "    3000   : Ctrl + Shift + 3\n";
    cout << "    4000   : Ctrl + Shift + 4\n";
    cout << "    5000   : Ctrl + Shift + 5\n";
    cout << "    6000   : Ctrl + Shift + 6\n";
    cout << "    7000   : Ctrl + Shift + 7\n";
    cout << "    8000   : Ctrl + Shift + 8\n";
    cout << "    9000   : Ctrl + Shift + 9\n";
    cout << "    10000  : Ctrl + 0\n";
    cout << "    20000  : Ctrl + O\n";
    cout << "    100000 : Ctrl + Shift + 0\n";
    cout << "    200000 : Ctrl + Shift + O\n";
    cout << "\n";
    cout << "  Number of colors (hold space to use it for selector):\n";
    cout << "    2    : Alt + 1\n";
    cout << "    4    : Alt + 2\n";
    cout << "    8    : Alt + 3\n";
    cout << "    16   : Alt + 4\n";
    cout << "    32   : Alt + 5\n";
    cout << "    64   : Alt + 6\n";
    cout << "    128  : Alt + 7\n";
    cout << "    256  : Alt + 8\n";
    cout << "    512  : Alt + 9\n";
    cout << "    1024 : Alt + 0\n";
    cout << "    2048 : Alt + O\n";
    cout << "\n";
    cout << "  Render:\n";
    cout << "    toggle double : Tab\n";
    cout << "\n";
    cout << "  Reset (hold space to use it for selector):\n";
    cout << "    zoom and center  : R\n";
    cout << "    iterations       : Ctrl + R\n";
    cout << "    number of colors : Alt + R\n";
    cout << "    all              : Shift + R\n";
    cout << "\n";
    cout << "  Newton fractal:\n";
    cout << "    add/remove point : Shift + RMB\n";
    cout << "    move point       : Shift + LMB\n" << endl;
}