#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "stb_image.h"
#include "Shader.h"

using std::cout, std::endl;

// Sizes of window and render areas
const int WIN_WIDTH = 1500;
const int WIN_HEIGHT = 1000;
const int VIEW_WIDTH = 1000;
const int VIEW_HEIGHT = 1000;

const int SMALL_WIDTH = WIN_WIDTH - VIEW_WIDTH;
const int SMALL_HEIGHT_OFFSET = WIN_HEIGHT - SMALL_WIDTH;

const float NORM_VIEW_WIDTH = (float)VIEW_WIDTH / WIN_WIDTH * 2 - 1;
//const float NORM_VIEW_HEIGHT = (float)VIEW_HEIGHT / WIN_HEIGHT * 2 - 1;

const float SMALL_SEC_HEIGHT = 1 - (float)SMALL_WIDTH * 2 / WIN_HEIGHT;

void processInput(GLFWwindow* window);
void mouseMoveCallback(GLFWwindow* window, double x, double y);

enum class fractal
{
	MANDELBROT   = 0b1,
	JULIA		 = 0b10,

	MANDELBROT_F = 0b10,
	MANDELBROT_D = 0b11,
	JULIA_F      = 0b100,
	JULIA_D      = 0b101
};

// Points on screen used to form triangles for main view
float mainVertices[] =
{
	-1.0f,             1.0f, 0.0f,		1.0f, 0.0f,
	 NORM_VIEW_WIDTH,  1.0f, 0.0f,		1.0f, 1.0f,
	 NORM_VIEW_WIDTH, -1.0f, 0.0f,		0.0f, 1.0f,
	-1.0f,            -1.0f, 0.0f,		0.0f, 0.0f
};

// Indices of triangles for rectangle (indexes in verices array)
unsigned int mainIndices[] =
{
	0, 1, 3,
	1, 2, 3
};

// Indices of triangles for rectangle (indexes in verices array)
unsigned int selectorIndices[] =
{
	0, 1, 3,
	1, 2, 3
};

// Points on screen used to form truengles for point selector view
float selectorVertices[] =
{
	NORM_VIEW_WIDTH, -SMALL_SEC_HEIGHT, 0.0f,	1.0f, 0.0f,
	1.0,             -SMALL_SEC_HEIGHT, 0.0f,   1.0f, 1.0f,
	1.0,             -1.0             , 0.0f,   0.0f, 1.0f,
	NORM_VIEW_WIDTH, -1.0             , 0.0f,	0.0f, 0.0f
};

// control variables
double scale = 4.0;
DVec2 center(0.0, 0.0);
int iterations = 1000;
Vec3 color(0.0f, 0.0f, 0.0f);
DVec2 constant(0, 0);
float colorCount = 256.0f;

float selScale = 4.0;
Vec2 selCenter(0.0f, 0.0f);
int selIterations = 100;
float selColorCount = 128.0f;

fractal frac = fractal::MANDELBROT;
bool useDouble = false;

fractal getFrac()
{
	return (fractal)(((unsigned int)frac << 1) | (unsigned int)useDouble);
}

int main()
{
	//==<<initialization>>==//

	// initialize opengl
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// initialize glfw window
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Project Fractals", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	
	// initialize glad
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return EXIT_FAILURE;
	}

	// setting viewport size
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	// setting on mouse move handler
	glfwSetCursorPosCallback(window, mouseMoveCallback);

	//==<<Creating shaders>>==//

	// float mandelbrot set
	Shader mandelbrotF("shader.vert", "mandelbrot_f.frag", [](Shader& shader)
		{
			shader.use();
			shader.setFloat("scale", (float)scale);
			shader.setFloat2("center", (Vec2)center);
			shader.setInt("iter", iterations);
			shader.setFloat("colorCount", colorCount);
		});
	if (!mandelbrotF.isCreated())
		return EXIT_FAILURE;
	// default values
	mandelbrotF.use();
	mandelbrotF.setInt("texture1", 0);
	mandelbrotF.setFloat("scale", (float)scale);
	mandelbrotF.setFloat2("center", (Vec2)center);
	mandelbrotF.setInt("iter", iterations);
	mandelbrotF.setFloat("colorCount", colorCount);
	mandelbrotF.setFloat3("color", color);

	// double mandelbrot shader
	Shader mandelbrotD("shader.vert", "mandelbrot_d.frag", [](Shader& shader)
		{
			shader.use();
			shader.setDouble("scale", scale);
			shader.setDouble2("center", center);
			shader.setInt("iter", iterations);
			shader.setFloat("colorCount", colorCount);
		});
	if (!mandelbrotD.isCreated())
		return EXIT_FAILURE;
	// default values
	mandelbrotD.use();
	mandelbrotD.setInt("texture1", 0);
	mandelbrotD.setDouble("scale", scale);
	mandelbrotD.setDouble2("center", center);
	mandelbrotD.setInt("iter", iterations);
	mandelbrotD.setFloat("colorCount", colorCount);
	mandelbrotD.setFloat3("color", color);

	// selector mandelbrot set
	Shader mandelbrotSelector("shader.vert", "mandelbrot_selector.frag", [](Shader& shader)
		{
			shader.use();
			shader.setFloat("scale", selScale);
			shader.setFloat2("center", selCenter);
			shader.setInt("iter", selIterations);
			shader.setFloat("colorCount", selColorCount);
			shader.setFloat2("constant", (Vec2)constant);
		});
	if (!mandelbrotSelector.isCreated())
		return EXIT_FAILURE;
	// default values
	mandelbrotSelector.use();
	mandelbrotSelector.setInt("texture1", 0);
	mandelbrotSelector.setFloat("scale", selScale);
	mandelbrotSelector.setFloat2("center", selCenter);
	mandelbrotSelector.setInt("iter", selIterations);
	mandelbrotSelector.setFloat3("color", color);
	mandelbrotSelector.setFloat("colorCount", selColorCount);
	mandelbrotSelector.setFloat2("constant", (Vec2)constant);

	// float julia shader
	Shader juliaF("shader.vert", "julia_f.frag", [](Shader& shader)
		{
			shader.use();
			shader.setFloat("scale", (float)scale);
			shader.setFloat2("center", (Vec2)center);
			shader.setInt("iter", iterations);
			shader.setFloat("colorCount", colorCount);
			shader.setFloat2("constant", (Vec2)constant);
		});
	if (!juliaF.isCreated())
		return EXIT_FAILURE;
	// default values
	juliaF.use();
	juliaF.setInt("texture1", 0);
	juliaF.setFloat("scale", (float)scale);
	juliaF.setFloat2("center", (Vec2)center);
	juliaF.setInt("iter", iterations);
	juliaF.setFloat3("color", color);
	juliaF.setFloat("colorCount", colorCount);
	juliaF.setFloat2("constant", (Vec2)constant);

	// double julia shader
	Shader juliaD("shader.vert", "julia_d.frag", [](Shader& shader)
		{
			shader.use();
			shader.setDouble("scale", scale);
			shader.setDouble2("center", center);
			shader.setInt("iter", iterations);
			shader.setFloat("colorCount", colorCount);
			shader.setDouble2("constant", constant);
		});
	if (!juliaF.isCreated())
		return EXIT_FAILURE;
	// default values
	juliaD.use();
	juliaD.setInt("texture1", 0);
	juliaD.setDouble("scale", scale);
	juliaD.setDouble2("center", center);
	juliaD.setInt("iter", iterations);
	juliaD.setFloat3("color", color);
	juliaD.setFloat("colorCount", colorCount);
	juliaD.setDouble2("constant", constant);


	//==<<setting up buffers>==//

	// main view
	unsigned int mainVAO, mainVBO, mainEBO;
	glGenVertexArrays(1, &mainVAO);
	glGenBuffers(1, &mainVBO);
	glGenBuffers(1, &mainEBO);

	glBindVertexArray(mainVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mainVertices), mainVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mainIndices), mainIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// selector view
	unsigned int selVAO, selVBO, selEBO;
	glGenVertexArrays(1, &selVAO);
	glGenBuffers(1, &selVBO);
	glGenBuffers(1, &selEBO);

	glBindVertexArray(selVAO);

	glBindBuffer(GL_ARRAY_BUFFER, selVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(selectorVertices), selectorVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(selectorIndices), selectorIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//==<<adding texture (gradient)>>==//

	// loading texture from file
	int width, height, nrChannels;
	unsigned char* data = stbi_load("yel_blue_1024.png", &width, &height, &nrChannels, 0);
	if (!data)
	{
		cout << "Failed to load texture" << endl;
		return EXIT_FAILURE;
	}
	
	// setting up texture 'behaviour'
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// loading texture to gpu
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// deleting texture from ram
	stbi_image_free(data);


	// setting clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		// user input
		processInput(window);

		// clearing display
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(mainVAO);

		// choosing fractal to render
		switch (getFrac())
		{
		case fractal::MANDELBROT_F:
			mandelbrotF.update();
			break;
		case fractal::MANDELBROT_D:
			mandelbrotD.update();
			break;
		case fractal::JULIA_F:
			juliaF.update();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(selVAO);
			mandelbrotSelector.update();
			break;
		case fractal::JULIA_D:
			juliaD.update();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(selVAO);
			mandelbrotSelector.update();
			break;
		default:
			cout << "Invalid fractal " << (unsigned int)frac << endl;
			break;
		}

		// rendering image
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// showing image
		glfwSwapBuffers(window);
		
		// updating user input
		glfwPollEvents();
	}

	// cleaning
	glDeleteVertexArrays(1, &mainVAO);
	glDeleteBuffers(1, &mainVBO);
	glDeleteBuffers(1, &mainEBO);
	glDeleteProgram(mandelbrotF.getId());
	glDeleteProgram(mandelbrotD.getId());
	glDeleteProgram(mandelbrotSelector.getId());
	glDeleteProgram(juliaF.getId());

	glfwTerminate();

	return EXIT_SUCCESS;
}

void mouseMoveCallback(GLFWwindow* window, double x, double y)
{
	// calculating mouse movement
	static DVec2 last;
	DVec2 current = DVec2(x, y);
	DVec2 delta = current - last;
	last = current;

	bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

	if (!spaceDown && current.x > VIEW_WIDTH && current.y > SMALL_HEIGHT_OFFSET)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			DVec2 relCur = current - DVec2(VIEW_WIDTH, SMALL_HEIGHT_OFFSET);
			constant = DVec2((relCur.x - SMALL_WIDTH / 2) / SMALL_WIDTH * selScale - selCenter.x, (relCur.y - SMALL_WIDTH / 2) / SMALL_WIDTH * -selScale - selCenter.y);
		}
	}
	else
	{
		DVec2 newCenterDifference{};
		double newScaleMultiplier = 1.0;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			// moving image
			newCenterDifference = DVec2(delta.x / VIEW_WIDTH, delta.y / VIEW_HEIGHT * -1);
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
			selScale *= (float)newScaleMultiplier;
			selCenter += (Vec2)(newCenterDifference * ((double)VIEW_WIDTH / SMALL_WIDTH) * selScale);
		}
		else
		{
			scale *= newScaleMultiplier;
			center += newCenterDifference * scale;
		}
	}
}

void processInput(GLFWwindow* window)
{
	// exit on ESC
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	// switching between float and double
	int tab = glfwGetKey(window, GLFW_KEY_TAB);
	static int lastTab = GLFW_RELEASE;
	if (tab != lastTab && tab == GLFW_PRESS)
	{
		useDouble = !useDouble;// (unsigned int)frac & 1 ? (fractal)((unsigned int)frac & ~1) : (fractal)((unsigned int)frac | 1);
	}
	lastTab = tab;

	float newColorCount = (float)colorCount;
	int newIterations = iterations;

	// changing number of iterations
	int base = 1;
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT))
	{
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			newColorCount = 2;
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			newColorCount = 4;
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			newColorCount = 8;
		else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			newColorCount = 16;
		else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
			newColorCount = 32;
		else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
			newColorCount = 64;
		else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
			newColorCount = 128;
		else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
			newColorCount = 256;
		else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
			newColorCount = 512;
		else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
			newColorCount = 1024;
		else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
			newColorCount = 2048;
		else
			return;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			base = 10;
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			newIterations = base * 100;
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			newIterations = base * 200;
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			newIterations = base * 300;
		else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			newIterations = base * 400;
		else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
			newIterations = base * 500;
		else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
			newIterations = base * 600;
		else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
			newIterations = base * 700;
		else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
			newIterations = base * 800;
		else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
			newIterations = base * 900;
		else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
			newIterations = base * 10000;
		else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
			newIterations = base * 20000;
		else
			return;
	}
	else
	{
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			frac = fractal::MANDELBROT;
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			frac = fractal::JULIA;
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		selColorCount = newColorCount;
		selIterations = newIterations;
	}
	else
	{
		colorCount = newColorCount;
		iterations = newIterations;
	}
}