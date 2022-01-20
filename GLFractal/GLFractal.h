#pragma once

#include "Vectors.h"

namespace GLFractal
{
	enum class Fractal
	{
		MANDELBROT = 0b1,
		JULIA = 0b10,
	};

	enum class GLFResult
	{
		OK = 0,
		WINDOW_INIT_ERROR,
		GLAD_INIT_ERROR,
		SHADER_INIT_ERROR,
		TEXTURE_LOAD_ERROR,
		INVALID_FRACTAL,
		UNEXPECTED_ERROR,
		FREETYPE_LOAD_ERROR,
		FONT_LOAD_ERROR,
		SOME_CHARACTERS_MISSING,
	};

	struct GLFConfig
	{
		double scale{ 4.0 };
		DVec2 center{ 0.0, 0.0 };
		int iterations{ 1000 };
		Vec3 color{ 0.0f, 0.0f, 0.0f };
		DVec2 constant{ 0.0, 0.0 };
		float colorCount{ 256.0f };

		float selScale{ 4.0f };
		Vec2 selCenter{ 0.0f, 0.0f };
		int selIterations{ 100 };
		float selColorCount{ 128.0f };

		Fractal fractal{ Fractal::MANDELBROT };
		bool useDouble{ false };

		Vec3 backgroundColor{ 0.1f, 0.1f, 0.1f };
		Vec3 textColor{ 0.9f, 0.9f, 0.9f };

		string gradientPath{ "yel_blue_1024.png" };
		
		string fontPath{ "CascadiaMono.ttf" };
		int fontSize{ 11 };
	};

	GLFResult init(GLFConfig config);
	GLFResult mainloop();
	GLFResult terminate();
};
