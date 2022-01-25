#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLFractal.h"

#include <iostream>

#include "stb_image.h"

#include "Shader.h"
#include "FontTexture.h"
#include "Vectors.h"
#include "Complex.h"

namespace GLFractal
{
    namespace
    {
        using namespace std;

        enum class _Fractal;
        enum class _RenderChange;

        // Sizes of window and render areas
        const int _WIN_WIDTH = 1500;
        const int _WIN_HEIGHT = 1000;
        const int _VIEW_WIDTH = 1000;
        const int _VIEW_HEIGHT = 1000;

        const int _SMALL_WIDTH = _WIN_WIDTH - _VIEW_WIDTH;
        const int _SMALL_HEIGHT_OFFSET = _WIN_HEIGHT - _SMALL_WIDTH;

        const float _NORM_VIEW_WIDTH = (float)_VIEW_WIDTH / _WIN_WIDTH * 2 - 1;
        //const float _NORM_VIEW_HEIGHT = (float)_VIEW_HEIGHT / _WIN_HEIGHT * 2 - 1;

        const float _SMALL_SEC_HEIGHT = 1 - (float)_SMALL_WIDTH * 2 / _WIN_HEIGHT;

        const int _MAX_ROOTS = 10;

        // when rendering text, determines how many characters will be rendered at once
        const int _MAX_STR_LEN = 64;

        GLFConfig _initialSettings;

        double _scale;
        DVec2  _center;
        int    _iterations;
        Vec3   _color;
        DVec2  _constant;
        float  _colorCount;

        float _selScale;
        Vec2  _selCenter;
        int   _selIterations;
        float _selColorCount;

        Fractal _frac;
        bool    _useDouble;

        GLFWwindow* _window;

        FontTexture _font;
        int _fontSize;
        Shader _fontShader;
        Vec3 _textColor;
        Mat4 _fontProjection;

        const struct
        {
            const float scaleL = 1.0f;
            const float scaleM = 0.5f;
            const float scaleS = 4.0f / 9.0f;

            const int full = 40;
            const int extended = 25;
            const int normal = 20;
        } _spacing;

        Mat4 _projection = Mat4::orthographic(0, _WIN_WIDTH, 0, _WIN_HEIGHT, 1, -1);

        static DVec2 _mousePos;

        int _rootCount = 3;
        Vec2 _roots[_MAX_ROOTS] =
        {
            Vec2(1.0f, 0.0f),
            Vec2(-0.5f, -0.86603f),
            Vec2(-0.5f, 0.86603f),
        };

        int _coefCount = 4;
        Vec2 _coefs[_MAX_ROOTS + 1] =
        {
            Vec2(1.0f, 0.0f),
            Vec2(0.0f, 0.0f),
            Vec2(0.0f, 0.0f),
            Vec2(-1.0f, 0.0f),
        };

        _Fractal _fractal();

        struct
        {
            Shader mandelbrotF;
            Shader mandelbrotD;
            Shader mandelbrotSelector;
            Shader juliaF;
            Shader juliaD;
            Shader newtonCoefF;
            Shader newtonCoefD;
        } _fractals;

        struct
        {
            unsigned int mainVAO;
            unsigned int mainVBO;
            unsigned int mainEBO;

            unsigned int selVAO;
            unsigned int selVBO;
            unsigned int selEBO;

            unsigned int textVAO;
            unsigned int textVBO;
            unsigned int textEBO;

            unsigned int gradientTexture;
            unsigned int fontTexture;
        } _buffers;

        // Points on screen used to form triangles for main view
        const float _mainVertices[] =
        {
            -1.0f,             1.0f, 0.0f,		1.0f, 0.0f,
            _NORM_VIEW_WIDTH,  1.0f, 0.0f,		1.0f, 1.0f,
            _NORM_VIEW_WIDTH, -1.0f, 0.0f,		0.0f, 1.0f,
            -1.0f,            -1.0f, 0.0f,		0.0f, 0.0f
        };

        // Points on screen used to form truengles for point selector view
        const float _selectorVertices[] =
        {
            _NORM_VIEW_WIDTH, -_SMALL_SEC_HEIGHT, 0.0f,     1.0f, 0.0f,
            1.0,              -_SMALL_SEC_HEIGHT, 0.0f,     1.0f, 1.0f,
            1.0,              -1.0              , 0.0f,     0.0f, 1.0f,
            _NORM_VIEW_WIDTH, -1.0              , 0.0f,     0.0f, 0.0f
        };

        // Indices of triangles for rectangle (indexes in verices array)
        const unsigned int _rectIndices[] =
        {
            0, 1, 3,
            1, 2, 3
        };

        inline _RenderChange operator|(_RenderChange rc1, _RenderChange rc2);

        _Fractal _fractal();

        GLFResult _init();
        GLFResult _initShaders();
        GLFResult _initBuffers();
        GLFResult _loadTexture(string texturePath);
        GLFResult _loadFont(string fontPath);

        void _processInput(GLFWwindow* window);
        _RenderChange _toggleFloatDouble(GLFWwindow* window);
        _RenderChange _changeColorCount(GLFWwindow* window, bool allowSelector);
        _RenderChange _changeIterations(GLFWwindow* window, bool allowSelector);
        _RenderChange _changeFractal(GLFWwindow* window);
        _RenderChange _resetRenderParam(GLFWwindow* window, bool allowSelector);
        _RenderChange _resetMainRenderParam(GLFWwindow* window);
        _RenderChange _resetSelectorRenderParam(GLFWwindow* window);
        _RenderChange _changeCoefs(GLFWwindow* window);

        void _mouseMoveCallback(GLFWwindow* window, double x, double y);
        _RenderChange _moveConstant(GLFWwindow* window);
        _RenderChange _moveRoot(GLFWwindow* window);
        _RenderChange _moveView(GLFWwindow* window, DVec2 mouseDelta, bool allowSelector);
        _RenderChange _scaleView(GLFWwindow* window, DVec2 mouseDelta, bool allowSelector);

        void _renderText(string text, float x, float y, float scale);

        void _renderInfo(double deltaTime);

        void _renderHelp();

        void _updateCoefs();

        enum class _Fractal
        {
            HELP = 0b0,
            MANDELBROT = 0b1,
            JULIA = 0b10,
            NEWTON = 0b11,

            HELP_F = 0b00,
            HELP_D = 0b01,
            MANDELBROT_F = 0b10,
            MANDELBROT_D = 0b11,
            JULIA_F = 0b100,
            JULIA_D = 0b101,
            NEWTON_F = 0b110,
            NEWTON_D = 0b111,
        };

        enum class _RenderChange
        {
            INVALID = 0b1,
            NONE = 0,
            MAIN = 0b10,
            SELECTOR = 0b100,
        };

        inline _RenderChange operator|(_RenderChange rc1, _RenderChange rc2)
        {
            return static_cast<_RenderChange>(static_cast<int>(rc1) | static_cast<int>(rc2));
        }

        _Fractal _fractal()
        {
            return (_Fractal)(((unsigned int)_frac << 1) | (unsigned int)_useDouble);
        }

        GLFResult _init()
        {
            // initialize opengl
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // initialize glfw window
            _window = glfwCreateWindow(_WIN_WIDTH, _WIN_HEIGHT, "Project Fractals", NULL, NULL);
            if (_window == NULL)
                return GLFResult::WINDOW_INIT_ERROR;

            // initialize glad
            glfwMakeContextCurrent(_window);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
                return GLFResult::GLAD_INIT_ERROR;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // setting viewport size
            glViewport(0, 0, _WIN_WIDTH, _WIN_HEIGHT);

            // setting on mouse move handler
            glfwSetCursorPosCallback(_window, _mouseMoveCallback);

            return GLFResult::OK;
        }

        GLFResult _initShaders()
        {
            // float mandelbrot set
            _fractals.mandelbrotF = Shader("shader.vert", "mandelbrot_f.frag", [](Shader& shader)
            {
                shader.use();
                shader.setInt("texture1", 0);
                shader.setFloat("scale", (float)_scale);
                shader.setFloat2("center", (Vec2)_center);
                shader.setInt("iter", _iterations);
                shader.setFloat("colorCount", _colorCount);
                shader.setFloat3("color", _color);
            });
            if (!_fractals.mandelbrotF.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.mandelbrotF.update();

            // double mandelbrot shader
            _fractals.mandelbrotD = Shader("shader.vert", "mandelbrot_d.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setInt("texutre1", 0);
                    shader.setDouble("scale", _scale);
                    shader.setDouble2("center", _center);
                    shader.setInt("iter", _iterations);
                    shader.setFloat("colorCount", _colorCount);
                    shader.setFloat3("color", _color);
                });
            if (!_fractals.mandelbrotD.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.mandelbrotD.update();

            // selector mandelbrot set
            _fractals.mandelbrotSelector = Shader("shader.vert", "mandelbrot_selector.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setInt("texture1", 0);
                    shader.setFloat("scale", _selScale);
                    shader.setFloat2("center", _selCenter);
                    shader.setInt("iter", _selIterations);
                    shader.setFloat3("color", _color);
                    shader.setFloat("colorCount", _selColorCount);
                    shader.setFloat2("constant", (Vec2)_constant);
                });
            if (!_fractals.mandelbrotSelector.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.mandelbrotSelector.update();

            // float julia shader
            _fractals.juliaF = Shader("shader.vert", "julia_f.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setInt("texture1", 0);
                    shader.setFloat("scale", (float)_scale);
                    shader.setFloat2("center", (Vec2)_center);
                    shader.setInt("iter", _iterations);
                    shader.setFloat3("color", _color);
                    shader.setFloat("colorCount", _colorCount);
                    shader.setFloat2("constant", (Vec2)_constant);
                });
            if (!_fractals.juliaF.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.juliaF.update();

            // double julia shader
            _fractals.juliaD = Shader("shader.vert", "julia_d.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setInt("texture1", 0);
                    shader.setDouble("scale", _scale);
                    shader.setDouble2("center", _center);
                    shader.setInt("iter", _iterations);
                    shader.setFloat3("color", _color);
                    shader.setFloat("colorCount", _colorCount);
                    shader.setDouble2("constant", _constant);
                });
            if (!_fractals.juliaF.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.juliaD.update();

            // newton fractal
            _fractals.newtonCoefF = Shader("shader.vert", "newton_coef_f.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setFloat("scale", (float)_scale);
                    shader.setFloat2("center", (Vec2)_center);
                    shader.setInt("iter", _iterations / 10);
                    shader.setFloat3("color", _color);
                    shader.setFloat2Array("roots", _MAX_ROOTS, _roots);
                    shader.setInt("rootCount", _rootCount);
                    shader.setFloat2Array("coefs", _MAX_ROOTS + 1, _coefs);
                    shader.setInt("coefCount", _coefCount);
                });
            if (!_fractals.newtonCoefF.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.newtonCoefF.update();

            // double newton fractal
            _fractals.newtonCoefD = Shader("shader.vert", "newton_coef_d.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setDouble("scale", _scale);
                    shader.setDouble2("center", _center);
                    shader.setInt("iter", _iterations / 10);
                    shader.setFloat3("color", _color);
                    shader.setFloat2Array("roots", _MAX_ROOTS, _roots);
                    shader.setInt("rootCount", _rootCount);
                    shader.setFloat2Array("coefs", _MAX_ROOTS + 1, _coefs);
                    shader.setInt("coefCount", _coefCount);
                });
            if (!_fractals.newtonCoefD.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.newtonCoefD.update();

            return GLFResult::OK;
        }

        GLFResult _initBuffers()
        {
            // main view
            glGenVertexArrays(1, &_buffers.mainVAO);
            glGenBuffers(1, &_buffers.mainVBO);
            glGenBuffers(1, &_buffers.mainEBO);

            glBindVertexArray(_buffers.mainVAO);

            glBindBuffer(GL_ARRAY_BUFFER, _buffers.mainVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_mainVertices), _mainVertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.mainEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_rectIndices), _rectIndices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

            // selector view
            glGenVertexArrays(1, &_buffers.selVAO);
            glGenBuffers(1, &_buffers.selVBO);
            glGenBuffers(1, &_buffers.selEBO);

            glBindVertexArray(_buffers.selVAO);

            glBindBuffer(GL_ARRAY_BUFFER, _buffers.selVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_selectorVertices), _selectorVertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.selEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_rectIndices), _rectIndices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

            return GLFResult::OK;
        }

        GLFResult _loadTexture(string texturePath)
        {
            int width, height, nrChannels;
            unsigned char* data = stbi_load(texturePath.data(), &width, &height, &nrChannels, 0);
            if (!data)
                return GLFResult::TEXTURE_LOAD_ERROR;

            // setting up texture
            glGenTextures(1, &_buffers.gradientTexture);
            glBindTexture(GL_TEXTURE_2D, _buffers.gradientTexture);

            // loading texture to gpu
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _buffers.gradientTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            // setting texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // deleting texture from ram
            stbi_image_free(data);



            return GLFResult::OK;
        }

        GLFResult _loadFont(string fontPath)
        {
            // loading font
            _font = FontTexture(fontPath, _fontSize);
            switch (_font.status())
            {
            case FontTextureStatus::FREETYPE_NOT_LOADED:
                return GLFResult::FREETYPE_LOAD_ERROR;
            case FontTextureStatus::FONT_NOT_LOADED:
                return GLFResult::FONT_LOAD_ERROR;
            case FontTextureStatus::SOME_CHARACTERS_MISSING:
                break;
            case FontTextureStatus::OK:
                break;
            default:
                return GLFResult::UNEXPECTED_ERROR;
            }

            _fontShader = Shader("text.vert", "text.frag", [](Shader& s)
            {
                s.use();
                s.setMatrix4("projection", _projection);
                s.setFloat3("textColor", _textColor);
                _fontShader.setInt("font", 1);

            });

            if (!_fontShader.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fontShader.use();
            
            glGenTextures(1, &_buffers.fontTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _buffers.fontTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _font.width(), _font.height(), 0, GL_RED, GL_UNSIGNED_BYTE, _font.image());

            _font.freeImage();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // prepare buffers for rendering text
            glGenVertexArrays(1, &_buffers.textVAO);
            glGenBuffers(1, &_buffers.textVBO);
            glGenBuffers(1, &_buffers.textEBO);

            glBindVertexArray(_buffers.textVAO);
            glBindBuffer(GL_ARRAY_BUFFER, _buffers.textVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * _MAX_STR_LEN, NULL, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.textEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * _MAX_STR_LEN, NULL, GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            return GLFResult::OK;
        }

        void _processInput(GLFWwindow* window)
        {
            // exit on ESC
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            
            switch (_frac)
            {
            case Fractal::HELP:
                _changeFractal(window);
                break;
            case Fractal::MANDELBROT:
                _toggleFloatDouble(window);

                if (_changeColorCount(window, false) != _RenderChange::NONE) {}
                else if (_changeIterations(window, false) != _RenderChange::NONE) {}
                else _changeFractal(window);

                _resetRenderParam(window, false);
                break;
            case Fractal::JULIA:
                _toggleFloatDouble(window);

                if (_changeColorCount(window, true) != _RenderChange::NONE) {}
                else if (_changeIterations(window, true) != _RenderChange::NONE) {}
                else _changeFractal(window);

                _resetRenderParam(window, true);
                break;
            case Fractal::NEWTON:
                _toggleFloatDouble(window);

                if (_changeIterations(window, false) != _RenderChange::NONE) {}
                else _changeFractal(window);

                _resetRenderParam(window, false);
                
                if (_changeCoefs(window) == _RenderChange::MAIN)
                    _updateCoefs();
                break;
            }
        }

        _RenderChange _toggleFloatDouble(GLFWwindow* window)
        {
            static int lastTab = GLFW_RELEASE;

            int tab = glfwGetKey(window, GLFW_KEY_TAB);

            if (tab != lastTab && tab == GLFW_PRESS)
            {
                _useDouble = !_useDouble;
                lastTab = tab;
                return _RenderChange::MAIN;
            }
            
            lastTab = tab;
            return _RenderChange::NONE;
        }

        _RenderChange _changeColorCount(GLFWwindow* window, bool allowSelector)
        {
            float newColorCount = 0;
            if (glfwGetKey(window, GLFW_KEY_LEFT_ALT))
            {
                if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
                    newColorCount = 2.0f;
                else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
                    newColorCount = 4.0f;
                else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
                    newColorCount = 8.0f;
                else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
                    newColorCount = 16.0f;
                else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
                    newColorCount = 32.0f;
                else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
                    newColorCount = 64.0f;
                else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
                    newColorCount = 128.0f;
                else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
                    newColorCount = 256.0f;
                else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
                    newColorCount = 512.0f;
                else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
                    newColorCount = 1024.0f;
                else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
                    newColorCount = 2048.0f;
                else
                    return _RenderChange::INVALID;
            }
            else
                return _RenderChange::NONE;

            if (glfwGetKey(window, GLFW_KEY_SPACE))
            {
                if (!allowSelector)
                    return _RenderChange::INVALID;
                _selColorCount = newColorCount;
                return _RenderChange::SELECTOR;
            }
            _colorCount = newColorCount;
            return _RenderChange::MAIN;
        }

        _RenderChange _changeIterations(GLFWwindow* window, bool allowSelector)
        {
            int newIterations = 0;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            {
                if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
                    newIterations = 100;
                else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
                    newIterations = 200;
                else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
                    newIterations = 300;
                else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
                    newIterations = 400;
                else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
                    newIterations = 500;
                else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
                    newIterations = 600;
                else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
                    newIterations = 700;
                else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
                    newIterations = 800;
                else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
                    newIterations = 900;
                else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
                    newIterations = 10000;
                else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
                    newIterations = 20000;
                else
                    return _RenderChange::INVALID;

                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    newIterations *= 10;
            }
            else
                return _RenderChange::NONE;

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                if (!allowSelector)
                    return _RenderChange::INVALID;
                _selIterations = newIterations;
                return _RenderChange::SELECTOR;
            }
            _iterations = newIterations;
            return _RenderChange::MAIN;
        }

        _RenderChange _changeFractal(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            {
                _frac = Fractal::MANDELBROT;
                return _RenderChange::MAIN;
            }
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            {
                _frac = Fractal::JULIA;
                return _RenderChange::MAIN;
            }
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            {
                _frac = Fractal::NEWTON;
                return _RenderChange::MAIN;
            }
            if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
            {
                _frac = Fractal::HELP;
                return _RenderChange::MAIN;
            }
            return _RenderChange::NONE;
        }

        _RenderChange _resetRenderParam(GLFWwindow* window, bool allowSelector)
        {
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
                return _RenderChange::NONE;

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                return allowSelector ? _resetSelectorRenderParam(window) : _RenderChange::INVALID;
            return _resetMainRenderParam(window);
        }

        _RenderChange _resetMainRenderParam(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                _iterations = _initialSettings.iterations;
            else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
                _colorCount = _initialSettings.colorCount;
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                _iterations = _initialSettings.iterations;
                _colorCount = _initialSettings.colorCount;
                _center     = _initialSettings.center;
                _scale      = _initialSettings.scale;
            }
            else
            {
                _center = _initialSettings.center;
                _scale  = _initialSettings.scale;
            }

            return _RenderChange::MAIN;
        }

        _RenderChange _resetSelectorRenderParam(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                _selIterations = _initialSettings.selIterations;
            else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
                _selColorCount = _initialSettings.selColorCount;
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                _selIterations = _initialSettings.selIterations;
                _selColorCount = _initialSettings.selColorCount;
                _selCenter     = _initialSettings.selCenter;
                _selScale      = _initialSettings.selScale;
            }
            else
            {
                _selCenter = _initialSettings.selCenter;
                _selScale  = _initialSettings.selScale;
            }

            return _RenderChange::SELECTOR;
        }

        _RenderChange _changeCoefs(GLFWwindow* window)
        {
            bool shiftDown = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            bool rmb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

            static bool lastRMB = false;
            bool curRS = rmb && shiftDown;

            if (rmb != lastRMB && curRS)
            {
                lastRMB = rmb;
                Vec2 fracPos = Vec2(
                    (float)((_mousePos.x - _VIEW_WIDTH / 2) / _VIEW_WIDTH * _scale - _center.x),
                    (float)((_mousePos.y - _VIEW_HEIGHT / 2) / _VIEW_HEIGHT * -_scale - _center.y)
                );

                if (_rootCount != 0)
                {
                    int closest = 0;
                    float distance = (fracPos - _roots[0]).length();
                    for (int i = 1; i < _rootCount; i++)
                    {
                        float newDist = (fracPos - _roots[i]).length();
                        if (newDist < distance)
                        {
                            distance = newDist;
                            closest = i;
                        }
                    }
                    if (distance < 0.007 * _scale)
                    {
                        _rootCount--;
                        for (int i = closest; i < _rootCount; i++)
                            _roots[i] = _roots[i + 1];
                        return _RenderChange::MAIN;
                    }
                }
                if (_rootCount < _MAX_ROOTS)
                {
                    _roots[_rootCount] = fracPos;
                    _rootCount++;
                    return _RenderChange::MAIN;
                }
                return _RenderChange::INVALID;
            }

            lastRMB = rmb;
            return _RenderChange::NONE;
        }

        void _mouseMoveCallback(GLFWwindow* window, double x, double y)
        {
            // calculating mouse movement
            DVec2 current = DVec2(x, y);
            DVec2 delta = current - _mousePos;
            _mousePos = current;

            switch (_frac)
            {
            case Fractal::HELP:
                break;
            case Fractal::MANDELBROT:
                if (_moveView(window, delta, false) != _RenderChange::NONE) break;
                if (_scaleView(window, delta, false) != _RenderChange::NONE) break;
                break;
            case Fractal::JULIA:
                if (_moveConstant(window) != _RenderChange::NONE) break;
                if (_moveView(window, delta, true) != _RenderChange::NONE) break;
                if (_scaleView(window, delta, true) != _RenderChange::NONE) break;
                break;
            case Fractal::NEWTON:
                bool cont = true;
                switch (_moveRoot(window))
                {
                case _RenderChange::NONE:
                    break;
                case _RenderChange::INVALID:
                    cont = false;
                    break;
                default:
                    _updateCoefs();
                    cont = false;
                    break;
                }
                if (!cont)
                    break;

                if (_moveView(window, delta, false) != _RenderChange::NONE) break;
                if (_scaleView(window, delta, false) != _RenderChange::NONE) break;

                break;
            }
        }

        _RenderChange _moveConstant(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && _mousePos.x > _VIEW_WIDTH && _mousePos.y > _SMALL_HEIGHT_OFFSET)
            {
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    DVec2 relCur = _mousePos - DVec2(_VIEW_WIDTH, _SMALL_HEIGHT_OFFSET);
                    _constant = DVec2(
                        (relCur.x - _SMALL_WIDTH / 2) / _SMALL_WIDTH * _selScale - _selCenter.x,
                        (relCur.y - _SMALL_WIDTH / 2) / _SMALL_WIDTH * -_selScale - _selCenter.y
                    );
                    return _RenderChange::SELECTOR;
                }
                return _RenderChange::INVALID;
            }
            return _RenderChange::NONE;
        }

        _RenderChange _moveRoot(GLFWwindow* window)
        {
            static int rootHold = -1;
            bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && shift)
            {
                Vec2 fracPos = Vec2(
                    (float)((_mousePos.x - _VIEW_WIDTH / 2) / _VIEW_WIDTH * _scale - _center.x),
                    (float)((_mousePos.y - _VIEW_HEIGHT / 2) / _VIEW_HEIGHT * -_scale - _center.y)
                );

                if (rootHold < 0)
                {
                    if (_rootCount == 0)
                        return _RenderChange::INVALID;
                    int closest = 0;
                    float distance = (fracPos - _roots[0]).length();
                    for (int i = 1; i < _rootCount; i++)
                    {
                        float newDist = (fracPos - _roots[i]).length();
                        if (newDist < distance)
                        {
                            distance = newDist;
                            closest = i;
                        }
                    }
                    if (distance < 0.007 * _scale)
                        rootHold = closest;
                }
                _roots[rootHold] = fracPos;
                return _RenderChange::MAIN;
            }
            rootHold = -1;
            return shift ? _RenderChange::INVALID : _RenderChange::NONE;
        }

        _RenderChange _moveView(GLFWwindow* window, DVec2 mouseDelta, bool allowSelector)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
                return _RenderChange::NONE;

            DVec2 newCenterDifference{
                mouseDelta.x / _VIEW_WIDTH,
                mouseDelta.y / _VIEW_HEIGHT * -1
            };

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                if (!allowSelector)
                    return _RenderChange::INVALID;
                _selCenter += (Vec2)(newCenterDifference * ((double)_VIEW_WIDTH / _SMALL_WIDTH) * _selScale);
                return _RenderChange::SELECTOR;
            }
            
            _center += newCenterDifference * _scale;
            return _RenderChange::MAIN;
        }

        _RenderChange _scaleView(GLFWwindow* window, DVec2 mouseDelta, bool allowSelector)
        {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
            {
                return _RenderChange::NONE;
            }

            double newScaleMultiplier = pow(0.99, -mouseDelta.y);

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                if (!allowSelector)
                    return _RenderChange::INVALID;
                _selScale *= (float)newScaleMultiplier;
                return _RenderChange::SELECTOR;
            }

            _scale *= newScaleMultiplier;
            return _RenderChange::MAIN;
        }

        void _renderText(string text, float x, float y, float scale)
        {
            int vertLength = 16 * text.length();
            int indLength = 6 * text.length();
            if (text.length() > _MAX_STR_LEN)
            {
                vertLength = 16 * _MAX_STR_LEN;
                indLength = 6 * _MAX_STR_LEN;
            }

            unique_ptr<float> vertPtr{ new float[vertLength] };
            float* vertices = vertPtr.get();
            
            unique_ptr<unsigned int> indPtr{ new unsigned int[indLength] };
            unsigned int* indices = indPtr.get();

            int indIndex = 0;
            int i = 0;
            for (string::const_iterator c = text.begin(); c != text.end() && i < _MAX_STR_LEN; c++, indIndex += 4, i++)
            {
                Character ch = _font[*c];

                float xPos = x + ch.bearing.x * scale;
                float yPos = y - (ch.size.y - ch.bearing.y) * scale;

                float w = ch.size.x * scale;
                float h = ch.size.y * scale;

                *vertices++ = xPos;     *vertices++ = yPos + h; *vertices++ = (float)ch.position.x / _font.width();               *vertices++ = (float)(ch.position.y + ch.size.y) / _font.height();
                *vertices++ = xPos;     *vertices++ = yPos;     *vertices++ = (float)ch.position.x / _font.width();               *vertices++ = (float)ch.position.y / _font.height();
                *vertices++ = xPos + w; *vertices++ = yPos;     *vertices++ = (float)(ch.position.x + ch.size.x) / _font.width(); *vertices++ = (float)ch.position.y / _font.height();
                *vertices++ = xPos + w; *vertices++ = yPos + h; *vertices++ = (float)(ch.position.x + ch.size.x) / _font.width(); *vertices++ = (float)(ch.position.y + ch.size.y) / _font.height();

                *indices++ = indIndex + 0; *indices++ = indIndex + 1; *indices++ = indIndex + 3;
                *indices++ = indIndex + 1; *indices++ = indIndex + 2; *indices++ = indIndex + 3;

                x += (ch.advance >> 6) * scale;
            }

            _fontShader.update();
            glBindVertexArray(_buffers.textVAO);
            glBindBuffer(GL_ARRAY_BUFFER, _buffers.textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertLength, vertPtr.get());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.textEBO);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indLength, indPtr.get());
            glVertexAttribPointer(0, vertLength, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glDrawElements(GL_TRIANGLES, indLength, GL_UNSIGNED_INT, 0);

            if (text.length() > _MAX_STR_LEN)
                _renderText(text.substr(_MAX_STR_LEN, text.length() - _MAX_STR_LEN), x, y, scale);
        }

        void _renderInfo(double deltaTime)
        {
            const float lm = _VIEW_WIDTH + 10;
            const float ls = lm + 10;
            float t = _VIEW_HEIGHT - 20;

            string useDouble = _useDouble ? "yes" : "no";

            switch (_frac)
            {
            case Fractal::HELP:
                _renderText("Fps: " + to_string((int)(1 / deltaTime)), lm, t, _spacing.scaleM);

                _renderText("Fractal: Help page", lm, t -= _spacing.full, _spacing.scaleM);
                break;
            case Fractal::MANDELBROT:
                _renderText("Fps: " + to_string((int)(1 / deltaTime)), lm, t, _spacing.scaleM);

                _renderText("Fractal: Mandelbrot set", lm, t -= _spacing.full, _spacing.scaleM);

                _renderText("Main:", lm, t -= _spacing.full, _spacing.scaleM);
                _renderText("Iterations: " + (_frac == Fractal::NEWTON ? to_string(_iterations / 10) : to_string(_iterations)), ls, t -= _spacing.extended, _spacing.scaleS);
                _renderText("Color count: " + to_string((int)_colorCount), ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Use double: " + useDouble, ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Scale: " + to_string(1 / _scale), ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Center: " + to_string(-_center.x) + " + " + to_string(-_center.y) + "i", ls, t -= _spacing.normal, _spacing.scaleS);
                break;
            case Fractal::JULIA:
                _renderText("Fps: " + to_string((int)(1 / deltaTime)), lm, t, _spacing.scaleM);

                _renderText("Fractal: Julia set", lm, t -= _spacing.full, _spacing.scaleM);

                _renderText("Main:", lm, t -= _spacing.full, _spacing.scaleM);
                _renderText("Iterations: " + (_frac == Fractal::NEWTON ? to_string(_iterations / 10) : to_string(_iterations)), ls, t -= _spacing.extended, _spacing.scaleS);
                _renderText("Color count: " + to_string((int)_colorCount), ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Use double: " + useDouble, ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Scale: " + to_string(1 / _scale), ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Center: " + to_string(-_center.x) + " + " + to_string(-_center.y) + "i", ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Number: " + to_string(_constant.x) + " + " + to_string(_constant.y) + "i", ls, t -= _spacing.normal, _spacing.scaleS);

                _renderText("Selector:", lm, t -= _spacing.full, _spacing.scaleM);
                _renderText("Iterations: " + to_string(_selIterations), ls, t -= _spacing.extended, _spacing.scaleS);
                _renderText("Color count: " + to_string((int)_selColorCount), ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Scale: " + to_string(1 / _selScale), ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Center: " + to_string(-_selCenter.x) + " + " + to_string(-_selCenter.y) + "i", ls, t -= _spacing.normal, _spacing.scaleS);
                break;
            case Fractal::NEWTON:
                _renderText("Fps: " + to_string((int)(1 / deltaTime)), lm, t, _spacing.scaleM);

                _renderText("Fractal: Newton fractal", lm, t -= _spacing.full, _spacing.scaleM);

                _renderText("Main:", lm, t -= _spacing.full, _spacing.scaleM);
                _renderText("Iterations: " + (_frac == Fractal::NEWTON ? to_string(_iterations / 10) : to_string(_iterations)), ls, t -= _spacing.extended, _spacing.scaleS);
                _renderText("Use double: " + useDouble, ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Scale: " + to_string(1 / _scale), ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Center: " + to_string(-_center.x) + " + " + to_string(-_center.y) + "i", ls, t -= _spacing.normal, _spacing.scaleS);
                _renderText("Number of roots: " + to_string(_rootCount), ls, t -= _spacing.normal, _spacing.scaleS);
                break;
            default:
                _renderText("Fps: " + to_string((int)(1 / deltaTime)), lm, t, _spacing.scaleM);

                _renderText("Fractal: Unknown", lm, t -= _spacing.full, _spacing.scaleM);
                break;
            }
        }

        void _renderHelp()
        {
            const float c1m = 10;
            const float c1s = c1m + 10;
            const float c2m = _VIEW_WIDTH / 2 + 10;
            const float c2s = c2m + 10;
            float t = _VIEW_HEIGHT - 50;

            _renderText("Key bindings", c1m - 5, t, _spacing.scaleL);

            _renderText("View (space for selector):", c1m, t -= _spacing.full, _spacing.scaleM);
            _renderText("move center : LMB", c1s, t -= _spacing.extended, _spacing.scaleS);
            _renderText("zoom in     : RMB up", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("zoom out    : RMB down", c1s, t -= _spacing.normal, _spacing.scaleS);

            _renderText("Fractals:", c1m, t -= _spacing.full, _spacing.scaleM);
            _renderText("help           : F1", c1s, t -= _spacing.extended, _spacing.scaleS);
            _renderText("mandelbrot set : 1", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("julia set      : 2", c1s, t -= _spacing.normal, _spacing.scaleS);
            
            _renderText("Selector:", c1m, t -= _spacing.full, _spacing.scaleM);
            _renderText("choose point : LMB", c1s, t -= _spacing.extended, _spacing.scaleS);

            _renderText("Iterations (space for selector):", c1m, t -= _spacing.full, _spacing.scaleM);
            _renderText("100    : Ctrl + 1", c1s, t -= _spacing.extended, _spacing.scaleS);
            _renderText("200    : Ctrl + 2", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("300    : Ctrl + 3", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("400    : Ctrl + 4", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("500    : Ctrl + 5", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("600    : Ctrl + 6", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("700    : Ctrl + 7", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("800    : Ctrl + 8", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("900    : Ctrl + 9", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("1000   : Ctrl + Shift + 1", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("2000   : Ctrl + Shift + 2", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("3000   : Ctrl + Shift + 3", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("4000   : Ctrl + Shift + 4", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("5000   : Ctrl + Shift + 5", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("6000   : Ctrl + Shift + 6", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("7000   : Ctrl + Shift + 7", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("8000   : Ctrl + Shift + 8", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("9000   : Ctrl + Shift + 9", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("10000  : Ctrl + 0", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("20000  : Ctrl + O", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("100000 : Ctrl + Shift + 0", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("200000 : Ctrl + Shift + O", c1s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("newton fractal uses only 1/10 of iterations", c1s, t -= _spacing.normal, _spacing.scaleS);

            t = 950;

            _renderText("Number of colors (space for selector):", c2m, t -= _spacing.full, _spacing.scaleM);
            _renderText("2    : Alt + 1", c2s, t -= _spacing.extended, _spacing.scaleS);
            _renderText("4    : Alt + 2", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("8    : Alt + 3", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("16   : Alt + 4", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("32   : Alt + 5", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("64   : Alt + 6", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("128  : Alt + 7", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("256  : Alt + 8", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("512  : Alt + 9", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("1024 : Alt + 0", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("2048 : Alt + O", c2s, t -= _spacing.normal, _spacing.scaleS);

            _renderText("Render:", c2m, t -= _spacing.full, _spacing.scaleM);
            _renderText("toggle double : Tab", c2s, t -= _spacing.extended, _spacing.scaleS);

            _renderText("Reset (space for selector):", c2m, t -= _spacing.full, _spacing.scaleM);
            _renderText("zoom and center  : R", c2s, t -= _spacing.extended, _spacing.scaleS);
            _renderText("iterations       : Ctrl + R", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("number of colors : Alt + R", c2s, t -= _spacing.normal, _spacing.scaleS);
            _renderText("all              : Shift + R", c2s, t -= _spacing.normal, _spacing.scaleS);

            _renderText("Newton fractal:", c2m, t -= _spacing.full, _spacing.scaleM);
            _renderText("add/remove point : Shift + RMB", c2s, t -= _spacing.extended, _spacing.scaleS);
            _renderText("move point       : Shift + LMB", c2s, t -= _spacing.normal, _spacing.scaleS);
        }

        void _updateCoefs()
        {
            _coefCount = _rootCount + 1;

            // set the _coefs to 1
            for (int i = 0; i < _rootCount; i++)
                _coefs[i] = Vec2(0, 0);
            _coefs[_rootCount] = Vec2(1, 0);

            // calculate coefs as multiplication of all degree 2 polynomials in _roots
            for (int i = _rootCount; i > 0; i--)
            {
                for (int j = i - 1; j < _rootCount; j++)
                    _coefs[j] = Complex::cMul(_coefs[j], _roots[i - 1] * -1) + _coefs[j + 1];
                _coefs[_rootCount] = Complex::cMul(_coefs[_rootCount], _roots[i - 1] * -1);
            }
        }
    }

    GLFResult init(GLFConfig config)
    {
        _initialSettings = config;

        _scale      = config.scale;
        _center     = config.center;
        _iterations = config.iterations;
        _color      = config.color;
        _constant   = config.constant;
        _colorCount = config.colorCount;

        _selScale      = config.selScale;
        _selCenter     = config.selCenter;
        _selIterations = config.selIterations;
        _selColorCount = config.selColorCount;

        _frac      = config.fractal;
        _useDouble = config.useDouble;

        _fontSize = config.fontSize;
        _textColor = config.textColor;

        GLFResult result{ GLFResult::OK };
        if ((result = _init()) != GLFResult::OK)
            return result;

        glClearColor(config.backgroundColor.x, config.backgroundColor.y, config.backgroundColor.z, 1.0f);

        if ((result = _initShaders()) != GLFResult::OK)
            return result;
        if ((result = _initBuffers()) != GLFResult::OK)
            return result;
        if ((result = _loadTexture(config.gradientPath)) != GLFResult::OK)
            return result;
        if ((result = _loadFont(config.fontPath)) != GLFResult::OK)
            return result;

        return GLFResult::OK;
    }

    GLFResult mainloop()
    {
        double lastTime = glfwGetTime();
        while (!glfwWindowShouldClose(_window))
        {
            // user input
            _processInput(_window);

            // clearing display
            glClear(GL_COLOR_BUFFER_BIT);

            //glBindTexture(GL_TEXTURE_2D, _buffers.gradientTexture);
            glBindVertexArray(_buffers.mainVAO);

            // choosing fractal to render
            switch (_fractal())
            {
            case _Fractal::MANDELBROT_F:
                _fractals.mandelbrotF.update();
                break;
            case _Fractal::MANDELBROT_D:
                _fractals.mandelbrotD.update();
                break;
            case _Fractal::JULIA_F:
                _fractals.juliaF.update();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(_buffers.selVAO);
                _fractals.mandelbrotSelector.update();
                break;
            case _Fractal::JULIA_D:
                _fractals.juliaD.update();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(_buffers.selVAO);
                _fractals.mandelbrotSelector.update();
                break;
            case _Fractal::HELP_F:
                _renderHelp();
                break;
            case _Fractal::HELP_D:
                _renderHelp();
                break;
            case _Fractal::NEWTON_F:
                _fractals.newtonCoefF.update();
                break;
            case _Fractal::NEWTON_D:
                _fractals.newtonCoefD.update();
                break;
            default:
                return GLFResult::INVALID_FRACTAL;
            }

            // rendering image
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            double newTime = glfwGetTime();

            _renderInfo(newTime - lastTime);

            lastTime = newTime;

            // showing image
            glfwSwapBuffers(_window);

            // updating user input
            glfwPollEvents();
        }

        return GLFResult::OK;
    }

    GLFResult terminate()
    {
        glDeleteVertexArrays(1, &_buffers.mainVAO);
        glDeleteBuffers(1, &_buffers.mainVBO);
        glDeleteBuffers(1, &_buffers.mainEBO);

        glDeleteVertexArrays(1, &_buffers.selVAO);
        glDeleteBuffers(1, &_buffers.selVBO);
        glDeleteBuffers(1, &_buffers.selEBO);

        glDeleteVertexArrays(1, &_buffers.textVAO);
        glDeleteBuffers(1, &_buffers.textVBO);
        glDeleteBuffers(1, &_buffers.textEBO);

        glDeleteTextures(1, &_buffers.gradientTexture);
        glDeleteTextures(1, &_buffers.fontTexture);

        _fractals.mandelbrotF.free();
        _fractals.mandelbrotD.free();
        _fractals.mandelbrotSelector.free();
        _fractals.juliaF.free();
        _fractals.juliaD.free();
        _fractals.newtonCoefF.free();
        _fractals.newtonCoefD.free();

        _fontShader.free();

        glfwTerminate();

        return GLFResult::OK;
    }
}