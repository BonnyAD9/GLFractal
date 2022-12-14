#pragma once

#include "Vectors.hpp"
#include "Gradient.hpp"

/// <summary>
/// Contains functions that run the Fractal window
/// </summary>
namespace GLFractal
{
	/// <summary>
	/// Available fractals
	/// </summary>
	enum class Fractal
	{
		HELP = 0b0,
		MANDELBROT = 0b1,
		JULIA = 0b10,
		NEWTON = 0b11,
		NOVA = 0b100,
	};

	/// <summary>
	/// Error codes
	/// </summary>
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

	/// <summary>
	/// Configuration structure
	/// </summary>
	struct GLFConfig
	{
		/// <summary>
		/// Scale of the main view
		/// default: 4
		/// </summary>
		double scale{ 4.0 };
		/// <summary>
		/// Center of the fractal relative to center of the main view
		/// default: [0, 0]
		/// </summary>
		DVec2 center{ 0.0, 0.0 };
		/// <summary>
		/// How many iterations should be done for each pixel to determine its color (in main view)
		/// default: 1000
		/// </summary>
		int iterations{ 1000 };
		/// <summary>
		/// Special color in fractals (e.g. max reached iterations)
		/// default: [0, 0, 0]
		/// </summary>
		Vec3 color{ 0.0f, 0.0f, 0.0f };
		/// <summary>
		/// Value set by selector
		/// default: [0, 0]
		/// </summary>
		DVec2 constants[10]{ { 0.0, 0.0 }, { 1.0, 0.0 } };
		/// <summary>
		/// Number of colors until the gradient is repeated (in main view)
		/// default: 256
		/// </summary>
		float colorCount{ 256.0f };

		/// <summary>
		/// Scale of the selector view
		/// default: 4
		/// </summary>
		float selScale{ 4.0f };
		/// <summary>
		/// Center of the fractal relative to the center of the selector view
		/// default: [0, 0]
		/// </summary>
		Vec2 selCenter{ 0.0f, 0.0f };
		/// <summary>
		/// How many iterations should be done for each pixel to determine its color (int selector view)
		/// default: 100
		/// </summary>
		int selIterations{ 100 };
		/// <summary>
		/// Number of colors until the gradient is repeated (int selector view)
		/// default: 128
		/// </summary>
		float selColorCount{ 128.0f };

		/// <summary>
		/// Which fractal should be rendered by default
		/// default: MANDELBROT
		/// </summary>
		Fractal fractal{ Fractal::MANDELBROT };
		/// <summary>
		/// Determines whether the main view should use double floating point arithmetic or not (true will very negativly affect performance on most GPUs)
		/// default: false
		/// </summary>
		bool useDouble{ false };

		/// <summary>
		/// Background color of the window
		/// default: [0.1, 0.1, 0.1]
		/// </summary>
		Vec3 backgroundColor{ 0.1f, 0.1f, 0.1f };
		/// <summary>
		/// Color of text
		/// default: [0.9, 0.9, 0.9]
		/// </summary>
		Vec3 textColor{ 0.9f, 0.9f, 0.9f };

		/// <summary>
		/// Path to the gradien that will be used
		/// default: ULTRA_FRACTAL
		/// </summary>
		GradientPreset gradient{ GradientPreset::ULTRA_FRACTAL };
		
		/// <summary>
		/// Path to the font that will be used to render text
		/// default: "CascadiaMiono.ttf"
		/// </summary>
		string fontPath{ "CascadiaMono.ttf" };
		/// <summary>
		/// Base size of font (2 * height in pixels)
		/// default: 40
		/// </summary>
		int fontSize{ 40 };

		/// <summary>
		/// Sets the fps limit
		/// default: 10 000
		/// </summary>
		double fpsLimit = 10000.0;

		/// <summary>
		/// Sets the number of roots negative number leaves the default
		/// default: -1
		/// </summary>
		int rootCount = -1;

		/// <summary>
		/// Contains the roots
		/// default: []
		/// </summary>
		Vec2 roots[10];
	};

	/// <summary>
	/// Initializes the window and resources
	/// </summary>
	/// <param name="config">Default configuration</param>
	/// <returns>Error code (OK = 0)</returns>
	GLFResult init(GLFConfig config);
	/// <summary>
	/// Runs the mainloop of the window (this wil exit when the window exits or on error)
	/// </summary>
	/// <returns>Error code (OK = 0)</returns>
	GLFResult mainloop();
	/// <summary>
	/// Deletes all resources
	/// </summary>
	/// <returns>Error code (OK = 0)</returns>
	GLFResult terminate();
};
