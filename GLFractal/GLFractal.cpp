#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLFractal.h"

#include <iostream>

#include "stb_image.h"
#include "Shader.h"

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

        _Fractal _fractal();

        struct
        {
            Shader mandelbrotF;
            Shader mandelbrotD;
            Shader mandelbrotSelector;
            Shader juliaF;
            Shader juliaD;
        } _fractals;

        struct
        {
            unsigned int mainVAO;
            unsigned int mainVBO;
            unsigned int mainEBO;

            unsigned int selVAO;
            unsigned int selVBO;
            unsigned int selEBO;

            unsigned int texture;
        } _buffers;

        // Points on screen used to form triangles for main view
        const float _mainVertices[] =
        {
            -1.0f,             1.0f, 0.0f,		1.0f, 0.0f,
            _NORM_VIEW_WIDTH,  1.0f, 0.0f,		1.0f, 1.0f,
            _NORM_VIEW_WIDTH, -1.0f, 0.0f,		0.0f, 1.0f,
            -1.0f,            -1.0f, 0.0f,		0.0f, 0.0f
        };

        // Indices of triangles for rectangle (indexes in verices array)
        const unsigned int _mainIndices[] =
        {
            0, 1, 3,
            1, 2, 3
        };

        // Indices of triangles for rectangle (indexes in verices array)
        const unsigned int _selectorIndices[] =
        {
            0, 1, 3,
            1, 2, 3
        };

        // Points on screen used to form truengles for point selector view
        const float _selectorVertices[] =
        {
            _NORM_VIEW_WIDTH, -_SMALL_SEC_HEIGHT, 0.0f,     1.0f, 0.0f,
            1.0,              -_SMALL_SEC_HEIGHT, 0.0f,     1.0f, 1.0f,
            1.0,              -1.0              , 0.0f,     0.0f, 1.0f,
            _NORM_VIEW_WIDTH, -1.0              , 0.0f,     0.0f, 0.0f
        };

        _Fractal _fractal();

        GLFResult _init();
        GLFResult _initShaders();
        GLFResult _initBuffers();
        GLFResult _loadTexture(string texturePath);

        void _processInput(GLFWwindow* window);
        _RenderChange _toggleFloatDouble(GLFWwindow* window);
        _RenderChange _changeColorCount(GLFWwindow* window);
        _RenderChange _changeIterations(GLFWwindow* window);
        _RenderChange _changeFractal(GLFWwindow* window);
        _RenderChange _resetRenderParam(GLFWwindow* window);
        _RenderChange _resetMainRenderParam(GLFWwindow* window);
        _RenderChange _resetSelectorRenderParam(GLFWwindow* window);

        void _mouseMoveCallback(GLFWwindow* window, double x, double y);

        enum class _Fractal
        {
            MANDELBROT = 0b1,
            JULIA = 0b10,

            MANDELBROT_F = 0b10,
            MANDELBROT_D = 0b11,
            JULIA_F = 0b100,
            JULIA_D = 0b101
        };

        enum class _RenderChange
        {
            INVALID = -1,
            NONE = 0,
            MAIN,
            SELECTOR
        };

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
                shader.setFloat("scale", (float)_scale);
                shader.setFloat2("center", (Vec2)_center);
                shader.setInt("iter", _iterations);
                shader.setFloat("colorCount", _colorCount);
            });
            if (!_fractals.mandelbrotF.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.mandelbrotF.use();
            _fractals.mandelbrotF.setInt("texture1", 0);
            _fractals.mandelbrotF.setFloat("scale", (float)_scale);
            _fractals.mandelbrotF.setFloat2("center", (Vec2)_center);
            _fractals.mandelbrotF.setInt("iter", _iterations);
            _fractals.mandelbrotF.setFloat("colorCount", _colorCount);
            _fractals.mandelbrotF.setFloat3("color", _color);

            // double mandelbrot shader
            _fractals.mandelbrotD = Shader("shader.vert", "mandelbrot_d.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setDouble("scale", _scale);
                    shader.setDouble2("center", _center);
                    shader.setInt("iter", _iterations);
                    shader.setFloat("colorCount", _colorCount);
                });
            if (!_fractals.mandelbrotD.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.mandelbrotD.use();
            _fractals.mandelbrotD.setInt("texture1", 0);
            _fractals.mandelbrotD.setDouble("scale", _scale);
            _fractals.mandelbrotD.setDouble2("center", _center);
            _fractals.mandelbrotD.setInt("iter", _iterations);
            _fractals.mandelbrotD.setFloat("colorCount", _colorCount);
            _fractals.mandelbrotD.setFloat3("color", _color);

            // selector mandelbrot set
            _fractals.mandelbrotSelector = Shader("shader.vert", "mandelbrot_selector.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setFloat("scale", _selScale);
                    shader.setFloat2("center", _selCenter);
                    shader.setInt("iter", _selIterations);
                    shader.setFloat("colorCount", _selColorCount);
                    shader.setFloat2("constant", (Vec2)_constant);
                });
            if (!_fractals.mandelbrotSelector.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.mandelbrotSelector.use();
            _fractals.mandelbrotSelector.setInt("texture1", 0);
            _fractals.mandelbrotSelector.setFloat("scale", _selScale);
            _fractals.mandelbrotSelector.setFloat2("center", _selCenter);
            _fractals.mandelbrotSelector.setInt("iter", _selIterations);
            _fractals.mandelbrotSelector.setFloat3("color", _color);
            _fractals.mandelbrotSelector.setFloat("colorCount", _selColorCount);
            _fractals.mandelbrotSelector.setFloat2("constant", (Vec2)_constant);

            // float julia shader
            _fractals.juliaF = Shader("shader.vert", "julia_f.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setFloat("scale", (float)_scale);
                    shader.setFloat2("center", (Vec2)_center);
                    shader.setInt("iter", _iterations);
                    shader.setFloat("colorCount", _colorCount);
                    shader.setFloat2("constant", (Vec2)_constant);
                });
            if (!_fractals.juliaF.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.juliaF.use();
            _fractals.juliaF.setInt("texture1", 0);
            _fractals.juliaF.setFloat("scale", (float)_scale);
            _fractals.juliaF.setFloat2("center", (Vec2)_center);
            _fractals.juliaF.setInt("iter", _iterations);
            _fractals.juliaF.setFloat3("color", _color);
            _fractals.juliaF.setFloat("colorCount", _colorCount);
            _fractals.juliaF.setFloat2("constant", (Vec2)_constant);

            // double julia shader
            _fractals.juliaD = Shader("shader.vert", "julia_d.frag", [](Shader& shader)
                {
                    shader.use();
                    shader.setDouble("scale", _scale);
                    shader.setDouble2("center", _center);
                    shader.setInt("iter", _iterations);
                    shader.setFloat("colorCount", _colorCount);
                    shader.setDouble2("constant", _constant);
                });
            if (!_fractals.juliaF.isCreated())
                return GLFResult::SHADER_INIT_ERROR;
            _fractals.juliaD.use();
            _fractals.juliaD.setInt("texture1", 0);
            _fractals.juliaD.setDouble("scale", _scale);
            _fractals.juliaD.setDouble2("center", _center);
            _fractals.juliaD.setInt("iter", _iterations);
            _fractals.juliaD.setFloat3("color", _color);
            _fractals.juliaD.setFloat("colorCount", _colorCount);
            _fractals.juliaD.setDouble2("constant", _constant);

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
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_mainIndices), _mainIndices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // selector view
            glGenVertexArrays(1, &_buffers.selVAO);
            glGenBuffers(1, &_buffers.selVBO);
            glGenBuffers(1, &_buffers.selEBO);

            glBindVertexArray(_buffers.selVAO);

            glBindBuffer(GL_ARRAY_BUFFER, _buffers.selVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_selectorVertices), _selectorVertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers.selEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_selectorIndices), _selectorIndices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            return GLFResult::OK;
        }

        GLFResult _loadTexture(string texturePath)
        {
            int width, height, nrChannels;
            unsigned char* data = stbi_load(texturePath.data(), &width, &height, &nrChannels, 0);
            if (!data)
                return GLFResult::TEXTURE_LOAD_ERROR;

            // setting up texture 'behaviour'
            glGenTextures(1, &_buffers.texture);
            glBindTexture(GL_TEXTURE_2D, _buffers.texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // loading texture to gpu
            glBindTexture(GL_TEXTURE_2D, _buffers.texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            // deleting texture from ram
            stbi_image_free(data);

            return GLFResult::OK;
        }

        void _processInput(GLFWwindow* window)
        {
            // exit on ESC
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            _toggleFloatDouble(window);

            if (_changeColorCount(window) != _RenderChange::NONE) {}
            else if (_changeIterations(window) != _RenderChange::NONE) {}
            else _changeFractal(window);

            _resetRenderParam(window);
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

        _RenderChange _changeColorCount(GLFWwindow* window)
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
                _selColorCount = newColorCount;
                return _RenderChange::SELECTOR;
            }
            _colorCount = newColorCount;
            return _RenderChange::MAIN;
        }

        _RenderChange _changeIterations(GLFWwindow* window)
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
            return _RenderChange::NONE;
        }

        _RenderChange _resetRenderParam(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
                return _RenderChange::NONE;

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                return _resetSelectorRenderParam(window);
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

        void _mouseMoveCallback(GLFWwindow* window, double x, double y)
        {
            // calculating mouse movement
            static DVec2 last;
            DVec2 current = DVec2(x, y);
            DVec2 delta = current - last;
            last = current;

            bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

            if (!spaceDown && current.x > _VIEW_WIDTH && current.y > _SMALL_HEIGHT_OFFSET)
            {
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    DVec2 relCur = current - DVec2(_VIEW_WIDTH, _SMALL_HEIGHT_OFFSET);
                    _constant = DVec2((relCur.x - _SMALL_WIDTH / 2) / _SMALL_WIDTH * _selScale - _selCenter.x, (relCur.y - _SMALL_WIDTH / 2) / _SMALL_WIDTH * -_selScale - _selCenter.y);
                }
            }
            else
            {
                DVec2 newCenterDifference{};
                double newScaleMultiplier = 1.0;

                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {
                    // moving image
                    newCenterDifference = DVec2(delta.x / _VIEW_WIDTH, delta.y / _VIEW_HEIGHT * -1);
                }
                else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
                {
                    // zooming image
                    newScaleMultiplier *= pow(0.99, -delta.y);
                }
                else
                {
                    return;
                }

                if (spaceDown)
                {
                    _selScale *= (float)newScaleMultiplier;
                    _selCenter += (Vec2)(newCenterDifference * ((double)_VIEW_WIDTH / _SMALL_WIDTH) * _selScale);
                }
                else
                {
                    _scale *= newScaleMultiplier;
                    _center += newCenterDifference * _scale;
                }
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

        return GLFResult::OK;
    }

    GLFResult mainloop()
    {
        while (!glfwWindowShouldClose(_window))
        {
            // user input
            _processInput(_window);

            // clearing display
            glClear(GL_COLOR_BUFFER_BIT);

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
            default:
                return GLFResult::INVALID_FRACTAL;
            }

            // rendering image
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

        glDeleteTextures(1, &_buffers.texture);

        _fractals.mandelbrotF.free();
        _fractals.mandelbrotD.free();
        _fractals.mandelbrotSelector.free();
        _fractals.juliaF.free();
        _fractals.juliaD.free();

        glfwTerminate();

        return GLFResult::OK;
    }
}