#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <filesystem>
#include <vector>

#include "shader.h"
#include "../Source/OBJloader.h"  //For loading .obj files
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Image loading library from https://github.com/nothings/stb
#include "irrKlang/irrKlang.h" // Sound Engine
using namespace irrklang;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float gridUnitLength = 0.01953125f;
float shapeMovement = 0.0f;
float wallMovement = 0.0f;
double totalTime = 120.0; //Change this to change time of game play (in seconds).
glm::vec3 direction;
glm::vec3 lightPosition(0.0f, 0.0f, -10.0f);
// Camera parameters
glm::mat4 view;
glm::mat4 projection;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.07f * 6.0f, 1.0f + shapeMovement * 0.07f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 2.0f;
float sensitivity = 0.03f;
float lastX = 512.0f;
float lastY = 384.0f;
float yaw = -90.0f;
float pitch = -10.0f;
float fov = 45.0f;
double mousePosX, mousePosY;
double lastMousePosX, lastMousePosY;
double mouseXChange;
double mouseYChange;
bool startGame = true;
bool playGame = false;
bool endGame = false;
bool startScreen = true;
bool gameScreen = false;
bool endScreen = false;
double currentTime;
double deltaTime2;
double timeLeft;


int rotations[1000];
float rotationHelp[1000];
int numberOfRotations = 0;
int minChecked = 0;
int verticesCount = 100;

// Stores rendering modes, default is triangles
GLenum renderModeShape = GL_TRIANGLES;

// For glow
bool useGlow = false;
bool kPressed = false;

// Rotations
bool qPressed = false;
bool ePressed = false;
bool wPressed = false;
bool sPressed = false;
bool aPressed = false;
bool dPressed = false;

// To randomize on release on only
bool rPressed = false;

// Textures and shadows toggle
bool bPressed = false;
bool xPressed = false;
bool useTextures = false;
bool useShadows = false;

// Texture things
GLuint brickTextureID;
GLuint metalTextureID;
GLuint tileTextureID;
GLuint skyboxBack;
GLuint skyboxBottom;
GLuint skyboxFront;
GLuint skyboxLeft;
GLuint skyboxRight;
GLuint skyboxTop;
GLuint font;

// Shape transformations
float translateShapeX = 0.0f;
float translateShapeY = 0.0f;
float translateShapeZ = 0.0f;
float scaleShape = 1.0f;
float rotateRandomX = 0.0f;
float rotateRandomY = 0.0f;
float rotateRandomZ = 0.0f;
std::vector<glm::vec3> shapePositions;
std::vector<glm::vec3> wallPositions;
std::vector<glm::vec3> wallBounds;


// Levels parameters
int numberOfCubesInWall = 5;
int level = 0;
float shapeSpeed = 0.04f;
bool levelBeaten = false;
// To only rotate once 
bool rotateShape = false;
bool glowingObjects = false;

GLuint createCubeVao() // Taken from lab and modified
{
	float vertices[] =
	{
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,//BACK
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,//FRONT
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//LEFT
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//RIGHT
		 0.5f, -0.5f, -0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,//BOTTOM
		 0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f, //TOP
		 0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,

		// Skybox Cube
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,//a//BACK
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,//b
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,//c
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,//c
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,//d
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,//a

		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,//a //FRONT
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,//b
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f,//c
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,//b
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,//a
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f,//d

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//a //LEFT
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,//b
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,//c
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,//b
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//a
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,//d

		 0.5f,  0.5f,  0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//a //RIGHT
		 0.5f,  0.5f, -0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,//b
		 0.5f, -0.5f, -0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,//c
		 0.5f, -0.5f, -0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,//c
		 0.5f, -0.5f,  0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,//d
		 0.5f,  0.5f,  0.5f,  0.8f, 0.4f, 0.2f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//a

		-0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,//a //BOTTOM
		 0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,//b
		 0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,//c
		 0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,//b
		-0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,//a
		-0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.8f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,//d

		-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,//a //TOP
		 0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,//b
		 0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,//c
		 0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,//c
		-0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,//d
		-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f//a
	};

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer
	(
		0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		11 * sizeof(float), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer
	(
		1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		11 * sizeof(float),
		(void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer
	(
		2,                            // attribute 2 matches aNormal in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		11 * sizeof(float),
		(void*)(sizeof(glm::vec3) * 2)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer
	(
		3,                            // attribute 2 matches aNormal in Vertex Shader
		2,
		GL_FLOAT,
		GL_FALSE,
		11 * sizeof(float),
		(void*)(sizeof(glm::vec3) * 3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // VAO already stored the state we just defined, safe to unbind buffer
	glBindVertexArray(0); // Unbind to not modify the VAO

	return vertexArrayObject;
}

void buildMatrices(Shader shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(fov), 1024.0f / 768.0f, 0.1f, 100.0f);

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
}

bool boundsAreSymmetric(std::vector<glm::vec3> wallBounds)
{
	std::vector<glm::vec3> translatedBounds;
	glm::vec3 selectedBound = wallBounds[0];
	for (int i = 0; i < wallBounds.size(); i++)
	{
		if (wallBounds[i].x < selectedBound.x || wallBounds[i].x == selectedBound.x && wallBounds[i].y < selectedBound.y)
			selectedBound = wallBounds[i];
	}

	for (int i = 0; i < wallBounds.size(); i++)
	{
		translatedBounds.push_back(wallBounds[i] - selectedBound);
	}

	std::vector<std::vector<glm::vec3>> symmetricBoundsSizeFive =
	{
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(1.0f, -2.0f, 0.0f),
			glm::vec3(2.0f, -2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -2.0f, 0.0f)
		}
	};
	std::vector<std::vector<glm::vec3>> symmetricBoundsSizeSix =
	{
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(3.0f, 1.0f, 0.0f),
			glm::vec3(3.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 3.0f, 0.0f),
			glm::vec3(2.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(1.0f, -2.0f, 0.0f),
			glm::vec3(1.0f, -3.0f, 0.0f),
			glm::vec3(2.0f, -3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 3.0f, 0.0f),
			glm::vec3(0.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 3.0f, 0.0f),
			glm::vec3(1.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(3.0f, 1.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 3.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f),
			glm::vec3(3.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -2.0f, 0.0f),
			glm::vec3(3.0f, -2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(1.0f, -2.0f, 0.0f),
			glm::vec3(2.0f, -2.0f, 0.0f),
			glm::vec3(2.0f, -3.0f, 0.0f)
		}
	};
	std::vector<std::vector<glm::vec3>> symmetricBoundsSizeSeven =
	{
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 1.0f, 0.0f),
			glm::vec3(3.0f, 2.0f, 0.0f),
			glm::vec3(3.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 3.0f, 0.0f),
			glm::vec3(1.0f, 3.0f, 0.0f),
			glm::vec3(2.0f, 3.0f, 0.0f),
			glm::vec3(3.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, -1.0f, 0.0f),
			glm::vec3(3.0f, -2.0f, 0.0f),
			glm::vec3(3.0f, -3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f),
			glm::vec3(3.0f, -1.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(3.0f, 1.0f, 0.0f),
			glm::vec3(3.0f, 2.0f, 0.0f),
			glm::vec3(3.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(1.0f, 3.0f, 0.0f),
			glm::vec3(2.0f, 3.0f, 0.0f),
			glm::vec3(3.0f, 3.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 2.0f, 0.0f),
			glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 0.0f)
		},
		{
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, -1.0f, 0.0f)
		}
	};

	std::vector<std::vector<glm::vec3>> symmetricBounds;
	switch (wallBounds.size())
	{
	case 5:
		symmetricBounds = symmetricBoundsSizeFive;
		break;
	case 6:
		symmetricBounds = symmetricBoundsSizeSix;
		break;
	case 7:
		symmetricBounds = symmetricBoundsSizeSeven;
		break;
	}

	for (int i = 0; i < symmetricBounds.size(); i++)
	{
		std::sort(symmetricBounds[i].begin(), symmetricBounds[i].end(),
			[](const glm::vec3& lhs, const glm::vec3& rhs)
		{
			if (lhs.x == rhs.x)
			{
				if (lhs.y == rhs.y)
					return lhs.z < rhs.z;

				return lhs.y < rhs.y;
			}

			return lhs.x < rhs.x;
		});
	}

	std::sort(translatedBounds.begin(), translatedBounds.end(),
		[](const glm::vec3& lhs, const glm::vec3& rhs)
	{
		if (lhs.x == rhs.x)
		{
			if (lhs.y == rhs.y)
				return lhs.z < rhs.z;

			return lhs.y < rhs.y;
		}

		return lhs.x < rhs.x;
	});

	bool symmetryFound = false;
	for (int i = 0; i < symmetricBounds.size(); i++)
	{
		if (symmetricBounds[i] == translatedBounds)
			symmetryFound = true;
	}

	return symmetryFound;
}

glm::vec3 determineNextPosition(glm::vec3 currentPosition, std::vector<glm::vec3> processedPositions, std::vector<glm::vec3> bounds, bool forMainShape)
{
	glm::vec3 nextPosition;
	bool timeToBreak = false;
	int count = 0;

	while (true)
	{
		std::vector<glm::vec3> relativePositions =
		{
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, -1.0f, 0.0f)
		};

		if (forMainShape)
		{
			relativePositions.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
			relativePositions.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		}

		int randomIndex = rand() % relativePositions.size();
		glm::vec3 relativePosition = relativePositions[randomIndex];

		for (int i = 0; i < bounds.size(); i++)
		{
			if (currentPosition.x + relativePosition.x == bounds[i].x && currentPosition.y + relativePosition.y == bounds[i].y)
			{
				glm::vec3 possibleNextPosition = currentPosition + relativePosition;

				if (std::find(processedPositions.begin(), processedPositions.end(), possibleNextPosition) == processedPositions.end())
				{
					nextPosition = possibleNextPosition;
					timeToBreak = true;

					break;
				}
			}
		}

		count += 1;
		if (count > 100)
		{
			nextPosition = glm::vec3(99, 99, 99);
			timeToBreak = true;
		}

		if (timeToBreak)
			break;
	}

	return nextPosition;
}

std::vector<glm::vec3> randomizeShape()
{
	std::vector<glm::vec3> randomPos;
	bool coveringAllBounds;

	do
	{
		coveringAllBounds = true;
		randomPos.clear();

		int randomIndex = rand() % wallBounds.size();
		glm::vec3 currentPosition = wallBounds[randomIndex];
		randomPos.push_back(currentPosition);

		for (int i = 0; i < 8; i++)
		{
			glm::vec3 nextPosition = determineNextPosition(currentPosition, randomPos, wallBounds, true);
			if (nextPosition == glm::vec3(99, 99, 99))
			{
				randomPos.clear();
				return randomPos;
			}
			randomPos.push_back(nextPosition);
			currentPosition = nextPosition;
		}

		for (int i = 0; i < wallBounds.size(); i++)
		{
			bool boundaryFound = false;

			for (int j = 0; j < randomPos.size(); j++)
			{
				if (wallBounds[i].x == randomPos[j].x && wallBounds[i].y == randomPos[j].y)
					boundaryFound = true;

				if (boundaryFound)
					break;
			}

			if (boundaryFound == false)
			{
				coveringAllBounds = false;
				break;
			}
		}
	} while (coveringAllBounds == false);

	return randomPos;
}

std::vector<glm::vec3> randomizeWall(int numberOfCubes)
{
	std::vector<glm::vec3> outerPositions =
	{
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(2.0f, -1.0f, 0.0f),
		glm::vec3(3.0f, -1.0f, 0.0f),
		glm::vec3(4.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 4.0f, 0.0f),
		glm::vec3(0.0f, 4.0f, 0.0f),
		glm::vec3(1.0f, 4.0f, 0.0f),
		glm::vec3(2.0f, 4.0f, 0.0f),
		glm::vec3(3.0f, 4.0f, 0.0f),
		glm::vec3(4.0f, 4.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(-1.0f, 2.0f, 0.0f),
		glm::vec3(-1.0f, 3.0f, 0.0f),
		glm::vec3(-1.0f, 4.0f, 0.0f),
		glm::vec3(4.0f, 0.0f, 0.0f),
		glm::vec3(4.0f, 1.0f, 0.0f),
		glm::vec3(4.0f, 2.0f, 0.0f),
		glm::vec3(4.0f, 3.0f, 0.0f),
		glm::vec3(4.0f, 4.0f, 0.0f)
	};

	std::vector<glm::vec3> innerPositions =
	{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		glm::vec3(0.0f, 3.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 2.0f, 0.0f),
		glm::vec3(1.0f, 3.0f, 0.0f),
		glm::vec3(2.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 1.0f, 0.0f),
		glm::vec3(2.0f, 2.0f, 0.0f),
		glm::vec3(2.0f, 3.0f, 0.0f),
		glm::vec3(3.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 1.0f, 0.0f),
		glm::vec3(3.0f, 2.0f, 0.0f),
		glm::vec3(3.0f, 3.0f, 0.0f)
	};

	std::vector<glm::vec3> randomPos;
	bool isSymmetric = false;

	do
	{
		int randomIndex = rand() % innerPositions.size();
		glm::vec3 currentPosition = innerPositions[randomIndex];
		randomPos.push_back(currentPosition);

		for (int i = 0; i < numberOfCubes - 1; i++)
		{
			glm::vec3 nextPosition = determineNextPosition(currentPosition, randomPos, innerPositions, false);
			if (nextPosition == glm::vec3(99, 99, 99))
			{
				randomPos.clear();
				return randomPos;
			}
			randomPos.push_back(nextPosition);
			currentPosition = nextPosition;
		}

		if (boundsAreSymmetric(randomPos))
			isSymmetric = true;

	} while (isSymmetric);

	wallBounds = randomPos;

	std::vector<glm::vec3> wallPos;
	std::vector<glm::vec3> posDiff;

	for (int i = 0; i < innerPositions.size(); i++)
	{
		if (std::find(randomPos.begin(), randomPos.end(), innerPositions[i]) == randomPos.end())
			posDiff.push_back(innerPositions[i]);
	}

	wallPos.reserve(posDiff.size() + outerPositions.size());
	wallPos.insert(wallPos.end(), posDiff.begin(), posDiff.end());
	wallPos.insert(wallPos.end(), outerPositions.begin(), outerPositions.end());

	return wallPos;
}

void displayShape(Shader shader)
{
	float minX = 100.0f;
	float maxX = -100.0f;
	float minY = 100.0f;
	float maxY = -100.0f;
	float minZ = 100.0f;
	float maxZ = -100.0f;

	for (glm::vec3 shape : shapePositions) 
	{
		if (shape.x < minX) minX = shape.x;
		if (shape.x > maxX) maxX = shape.x;
		if (shape.y < minY) minY = shape.y;
		if (shape.y > maxY) maxY = shape.y;
		if (shape.z < minZ) minZ = shape.z;
		if (shape.z > maxZ) maxZ = shape.z;
	}

	float centerX = (maxX - minX) / 2 + minX;
	float centerY = (maxY - minY) / 2 + minY;
	float centerZ = (maxZ - minZ) / 2 + minZ;

	for (int i = 0; i < 9; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3((0.0f + translateShapeX) * 0.07f, (0.0f + translateShapeY) * 0.07f, (1.0f + translateShapeZ - shapeMovement - minZ) * 0.07f));
		model = glm::scale(model, glm::vec3(0.07f * scaleShape, 0.07f * scaleShape, 0.07f * scaleShape));
		model = glm::translate(model, shapePositions[i]);
		model = glm::translate(model, glm::vec3((-shapePositions[i].x) + centerX, -shapePositions[i].y + centerY, -shapePositions[i].z + centerZ));

		for (int i = numberOfRotations - 1; i >= 0; i--) {
			switch (rotations[i]) {
			case 0: model = glm::rotate(model, glm::radians(rotationHelp[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 1:model = glm::rotate(model, glm::radians(-rotationHelp[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 2:model = glm::rotate(model, glm::radians(rotationHelp[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case 3:model = glm::rotate(model, glm::radians(-rotationHelp[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case 4:model = glm::rotate(model, glm::radians(rotationHelp[i]), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case 5:model = glm::rotate(model, glm::radians(-rotationHelp[i]), glm::vec3(0.0f, 0.0f, 1.0f));
				break;




			}
		}
		model = glm::rotate(model, glm::radians(rotateRandomY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotateRandomZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(rotateRandomX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3((shapePositions[i].x - centerX), shapePositions[i].y - centerY, shapePositions[i].z - centerZ));

		shader.setMat4("model", model);

		glDrawArrays(renderModeShape, 0, 36);
	}
}

void displayModel(Shader shader, int verticesCount)
{
	glm::vec3 positions[] =
	{
		glm::vec3(15.0f, 8.0f, -45.5f),
		glm::vec3(-10.0f, 4.0f, -32.5f),
		glm::vec3(2.0f, -8.0f, -30.5f),
		glm::vec3(0.0f, 10.0f, -40.5f),
		glm::vec3(-3.0f, 9.0f, -12.5f),
		glm::vec3(-12.0f, -5.0f, -22.0f),
		glm::vec3(11.0f, -5.0f, -23.5f)
	};

	for (int i = 0; i < 8; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
		model = glm::translate(model, positions[i]);
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, verticesCount);
	}
}

void drawSkybox(Shader shader)
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));

	shader.setMat4("model", model);

	// Draw back
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, skyboxBack);
	glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 2);
	glDrawArrays(GL_TRIANGLES, 36, 6);

	// Draw Front
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, skyboxFront);
	glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 2);
	glDrawArrays(GL_TRIANGLES, 42, 6);

	// Draw Left
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, skyboxLeft);
	glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 2);
	glDrawArrays(GL_TRIANGLES, 48, 6);

	// Draw Right
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, skyboxRight);
	glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 2);
	glDrawArrays(GL_TRIANGLES, 54, 6);

	// Draw Bottom
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, skyboxBottom);
	glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 2);
	glDrawArrays(GL_TRIANGLES, 60, 6);

	// Draw Top
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, skyboxTop);
	glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 2);
	glDrawArrays(GL_TRIANGLES, 66, 6);
}

void displayWall(Shader shader)
{
	for (int i = 0; i < wallPositions.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -45.0f));
		model = glm::translate(model, wallPositions[i]);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

GLuint buildDepthMapFrameBuffer()
{
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);

	return depthMapFBO;
}

void calculateShadows(glm::vec3 lightPosition,Shader shadowShader, Shader mainShader, GLuint depthMapFBO, GLuint vao, GLFWwindow* window)
{
	
	/*glm::vec3 lightFocus(0.0, 0.0, -50.0);
	glm::vec3 lightDirection = glm::normalize(lightFocus - lightPosition);

	glm::mat4 lightProjectionMatrix = glm::perspective(65.0f, 1.0f, 0.01f, 100.0f);
	glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, lightPosition + lightDirection, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;*/

	glm::mat4 lightProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f,0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix; 


	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));




	mainShader.use();
	mainShader.setMat4("lightMatrix", lightSpaceMatrix);
	shadowShader.use();
	shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shadowShader.setMat4("model", model);
	shadowShader.setInt("shadowMap", 0);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);

	displayWall(shadowShader);
	displayShape(shadowShader);
	displayModel(mainShader, verticesCount);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void rotateShapes(float x) {

	for (int i = minChecked; i < numberOfRotations; i++) {
		if (rotationHelp[i] < 89) rotationHelp[i] += x;
		else minChecked++;
	}

}

void processInput(GLFWwindow* window)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Camera Controls
	const float actualSpeed = cameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += actualSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= actualSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * actualSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * actualSpeed;
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = -90.0f;
		pitch = 0.0f;
		fov = 45.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		if (startScreen == true) 
		{
			startScreen = false;
			gameScreen = true;
			startGame = true;
		}
		if (endScreen == true) 
		{
			endScreen = false;
			startGame = true;
			gameScreen = true;
		}
	}

	// Shape Reset
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		renderModeShape = GL_TRIANGLES;
		translateShapeX = 0.0;
		translateShapeY = 0.0;
		translateShapeZ = 0.0;
	}

	if (!levelBeaten && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		aPressed = true;
	}


	if (aPressed && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
	{
		aPressed = false;
		rotations[numberOfRotations] = 2;
		numberOfRotations++;
	}

	if (!levelBeaten && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		dPressed = true;

	if (dPressed && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
	{
		dPressed = false;
		rotations[numberOfRotations] = 3;
		numberOfRotations++;
	}

	if (!levelBeaten && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		wPressed = true;

	if (wPressed && glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
	{
		wPressed = false;
		rotations[numberOfRotations] = 1;
		numberOfRotations++;
	}

	if (!levelBeaten && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		sPressed = true;

	if (sPressed && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
	{
		sPressed = false;
		rotations[numberOfRotations] = 0;
		numberOfRotations++;
	}

	if (!levelBeaten && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		qPressed = true;

	if (qPressed && glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
	{
		qPressed = false;
		rotations[numberOfRotations] = 4;
		numberOfRotations++;
	}

	if (!levelBeaten && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		ePressed = true;

	if (ePressed && glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
	{
		ePressed = false;
		rotations[numberOfRotations] = 5;
		numberOfRotations++;
	}


	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Reset Shapes
	{
		renderModeShape = GL_TRIANGLES;
		scaleShape = 1.0;
		translateShapeX = 0.0;
		translateShapeY = 0.0;
		translateShapeZ = 0.0;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // texture controls
		xPressed = true;

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE && xPressed)
	{
		xPressed = false;
		if (useTextures == true) useTextures = false;
		else useTextures = true;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) // glow controls
		kPressed = true;

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE && kPressed)
	{
		kPressed = false;
		if (useGlow == true) useGlow = false;
		else useGlow = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) //shadow controls
		bPressed = true;

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE && bPressed)
	{
		bPressed = false;
		if (useShadows == true) useShadows = false;
		else useShadows = true;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		mouseYChange *= sensitivity;
		pitch -= mouseYChange;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		mouseXChange *= sensitivity;
		yaw += mouseXChange;
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);

	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		fov += (float)mouseYChange;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 45.0f)
			fov = 45.0f;
	}
}

void calculateCameraViewVector()
{
	pitch -= mouseYChange;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	mouseXChange *= sensitivity;
	yaw += mouseXChange;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	cameraPos = glm::vec3(0.0f, 0.07f * 6.0f, 1.0f - shapeMovement * 0.07f);
}

void setLevelParameters()
{
	switch (level % 3)
	{
	case 1:
		numberOfCubesInWall = 5;
		break;
	case 2:
		numberOfCubesInWall = 6;
		break;
	case 0:
		numberOfCubesInWall = 7;
		break;
	default:
		break;
	}

	shapeSpeed = 0.04f + ((level / 3) + 1) * 0.02f;
}

void randomizeRotations() 
{
	int randZ = rand() % 3;
	int randX = rand() % 3;
	int randY = rand() & 3;
	switch (randZ) 
	{
	case 0: rotateRandomZ = 90.0f;
		break;
	case 1: rotateRandomZ = 180.0f;
		break;
	case 2: rotateRandomZ = 270.0f;
		break;
	}

	switch (randX) 
	{
	case 0: rotateRandomX = 90.0f;
		break;
	case 1: rotateRandomX = 180.0f;
		break;
	case 2: rotateRandomX = 270.0f;
		break;
	}

	switch (randY) 
	{
	case 0: rotateRandomY = 90.0f;
		break;
	case 1: rotateRandomY = 180.0f;
		break;
	case 2: rotateRandomY = 270.0f;
		break;
	}
}

GLuint loadTexture(const char* filename) // From lab
{
	// Step1 Create and bind textures
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	assert(textureId != 0);

	glBindTexture(GL_TEXTURE_2D, textureId);

	// Step2 Set filter parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Step3 Load Textures with dimension data
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
		return 0;
	}
	// Step4 Upload the texture to the PU
	GLenum format = 0;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
		0, format, GL_UNSIGNED_BYTE, data);

	// Step5 Free resources
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

GLuint setupModelVBO(std::string path, int& vertexCount) 
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;

	//read the vertex data from the model's OBJ file
	loadOBJ(path.c_str(), vertices, normals, UVs);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); //Becomes active VAO
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

	//Vertex VBO setup
	GLuint vertices_VBO;
	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Normals VBO setup
	GLuint normals_VBO;
	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	//UVs VBO setup
	GLuint uvs_VBO;
	glGenBuffers(1, &uvs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
	vertexCount = vertices.size();
	return VAO;
}

GLuint createTextVao(float x1, float y1, float x2, float y2, float s)
{
	// A vertex is a point on a polygon, it contains positions and other data (eg: colors)
	float vertexArray[] = {
		-0.1f * s, -0.1f * s,  0.0f,  1.0f,  0.0f,  1.0f,  x1,  y2,
		 0.1f * s, -0.1f * s,  0.0f,  1.0f,  0.0f,  1.0f,  x2,  y2,
		-0.1f * s,  0.1f * s,  0.0f,  1.0f,  0.0f,  1.0f,  x1,  y1,
		 0.1f * s, -0.1f * s,  0.0f,  1.0f,  0.0f,  1.0f,  x2,  y2,
		 0.1f * s,  0.1f * s,  0.0f,  1.0f,  0.0f,  1.0f,  x2,  y1,
		-0.1f * s,  0.1f * s,  0.0f,  1.0f,  0.0f,  1.0f,  x1,  y1,
	};

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		2,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		8 * sizeof(float), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		4,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(void*)(sizeof(float) * 2)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,                            // attribute 1 matches aColor in Vertex Shader
		2,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(void*)(sizeof(float) * 6)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vertexArrayObject;
}

void displayText(Shader shader, std::string text, glm::vec2 loc, float s)
{

	//Function to display text. the vec2 parameter is the location on the screen to start displaying the string

	float x1, x2, y1, y2;

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, font);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(shader.ID, "font"), 2);

	for (int i = 0; i < text.length(); i++) {

		switch (text[i]) {
		case 'A':
			x1 = 0.0; x2 = 0.1; y1 = 0.0; y2 = 0.1;
			break;
		case 'B':
			x1 = 0.1; x2 = 0.2; y1 = 0.0; y2 = 0.1;
			break;
		case 'C':
			x1 = 0.2; x2 = 0.3; y1 = 0.0; y2 = 0.1;
			break;
		case 'D':
			x1 = 0.3; x2 = 0.4; y1 = 0.0; y2 = 0.1;
			break;
		case 'E':
			x1 = 0.4; x2 = 0.5; y1 = 0.0; y2 = 0.1;
			break;
		case 'F':
			x1 = 0.5; x2 = 0.6; y1 = 0.0; y2 = 0.1;
			break;
		case 'G':
			x1 = 0.6; x2 = 0.7; y1 = 0.0; y2 = 0.1;
			break;
		case 'H':
			x1 = 0.7; x2 = 0.8; y1 = 0.0; y2 = 0.1;
			break;
		case 'I':
			x1 = 0.8; x2 = 0.9; y1 = 0.0; y2 = 0.1;
			break;
		case 'J':
			x1 = 0.9; x2 = 1.0; y1 = 0.0; y2 = 0.1;
			break;
		case 'K':
			x1 = 0.0; x2 = 0.1; y1 = 0.1; y2 = 0.2;
			break;
		case 'L':
			x1 = 0.1; x2 = 0.2; y1 = 0.1; y2 = 0.2;
			break;
		case 'M':
			x1 = 0.2; x2 = 0.3; y1 = 0.1; y2 = 0.2;
			break;
		case 'N':
			x1 = 0.3; x2 = 0.4; y1 = 0.1; y2 = 0.2;
			break;
		case 'O':
			x1 = 0.4; x2 = 0.5; y1 = 0.1; y2 = 0.2;
			break;
		case 'P':
			x1 = 0.5; x2 = 0.6; y1 = 0.1; y2 = 0.2;
			break;
		case 'Q':
			x1 = 0.6; x2 = 0.7; y1 = 0.1; y2 = 0.2;
			break;
		case 'R':
			x1 = 0.7; x2 = 0.8; y1 = 0.1; y2 = 0.2;
			break;
		case 'S':
			x1 = 0.8; x2 = 0.9; y1 = 0.1; y2 = 0.2;
			break;
		case 'T':
			x1 = 0.9; x2 = 1.0; y1 = 0.1; y2 = 0.2;
			break;
		case 'U':
			x1 = 0.0; x2 = 0.1; y1 = 0.2; y2 = 0.3;
			break;
		case 'V':
			x1 = 0.1; x2 = 0.2; y1 = 0.2; y2 = 0.3;
			break;
		case 'W':
			x1 = 0.2; x2 = 0.3; y1 = 0.2; y2 = 0.3;
			break;
		case 'X':
			x1 = 0.3; x2 = 0.4; y1 = 0.2; y2 = 0.3;
			break;
		case 'Y':
			x1 = 0.4; x2 = 0.5; y1 = 0.2; y2 = 0.3;
			break;
		case 'Z':
			x1 = 0.5; x2 = 0.6; y1 = 0.2; y2 = 0.3;
			break;
		case 'a':
			x1 = 0.6; x2 = 0.7; y1 = 0.2; y2 = 0.3;
			break;
		case 'b':
			x1 = 0.7; x2 = 0.8; y1 = 0.2; y2 = 0.3;
			break;
		case 'c':
			x1 = 0.8; x2 = 0.9; y1 = 0.2; y2 = 0.3;
			break;
		case 'd':
			x1 = 0.9; x2 = 1.0; y1 = 0.2; y2 = 0.3;
			break;
		case 'e':
			x1 = 0.0; x2 = 0.1; y1 = 0.3; y2 = 0.4;
			break;
		case 'f':
			x1 = 0.1; x2 = 0.2; y1 = 0.3; y2 = 0.4;
			break;
		case 'g':
			x1 = 0.2; x2 = 0.3; y1 = 0.3; y2 = 0.4;
			break;
		case 'h':
			x1 = 0.3; x2 = 0.4; y1 = 0.3; y2 = 0.4;
			break;
		case 'i':
			x1 = 0.4; x2 = 0.5; y1 = 0.3; y2 = 0.4;
			break;
		case 'j':
			x1 = 0.5; x2 = 0.6; y1 = 0.3; y2 = 0.4;
			break;
		case 'k':
			x1 = 0.6; x2 = 0.7; y1 = 0.3; y2 = 0.4;
			break;
		case 'l':
			x1 = 0.7; x2 = 0.8; y1 = 0.3; y2 = 0.4;
			break;
		case 'm':
			x1 = 0.8; x2 = 0.9; y1 = 0.3; y2 = 0.4;
			break;
		case 'n':
			x1 = 0.9; x2 = 1.0; y1 = 0.3; y2 = 0.4;
			break;
		case 'o':
			x1 = 0.0; x2 = 0.1; y1 = 0.4; y2 = 0.5;
			break;
		case 'p':
			x1 = 0.1; x2 = 0.2; y1 = 0.4; y2 = 0.5;
			break;
		case 'q':
			x1 = 0.2; x2 = 0.3; y1 = 0.4; y2 = 0.5;
			break;
		case 'r':
			x1 = 0.3; x2 = 0.4; y1 = 0.4; y2 = 0.5;
			break;
		case 's':
			x1 = 0.4; x2 = 0.5; y1 = 0.4; y2 = 0.5;
			break;
		case 't':
			x1 = 0.5; x2 = 0.6; y1 = 0.4; y2 = 0.5;
			break;
		case 'u':
			x1 = 0.6; x2 = 0.7; y1 = 0.4; y2 = 0.5;
			break;
		case 'v':
			x1 = 0.7; x2 = 0.8; y1 = 0.4; y2 = 0.5;
			break;
		case 'w':
			x1 = 0.8; x2 = 0.9; y1 = 0.4; y2 = 0.5;
			break;
		case 'x':
			x1 = 0.9; x2 = 1.0; y1 = 0.4; y2 = 0.5;
			break;
		case 'y':
			x1 = 0.0; x2 = 0.1; y1 = 0.5; y2 = 0.6;
			break;
		case 'z':
			x1 = 0.1; x2 = 0.2; y1 = 0.5; y2 = 0.6;
			break;
		case '0':
			x1 = 0.2; x2 = 0.3; y1 = 0.5; y2 = 0.6;
			break;
		case '1':
			x1 = 0.3; x2 = 0.4; y1 = 0.5; y2 = 0.6;
			break;
		case '2':
			x1 = 0.4; x2 = 0.5; y1 = 0.5; y2 = 0.6;
			break;
		case '3':
			x1 = 0.5; x2 = 0.6; y1 = 0.5; y2 = 0.6;
			break;
		case '4':
			x1 = 0.6; x2 = 0.7; y1 = 0.5; y2 = 0.6;
			break;
		case '5':
			x1 = 0.7; x2 = 0.8; y1 = 0.5; y2 = 0.6;
			break;
		case '6':
			x1 = 0.8; x2 = 0.9; y1 = 0.5; y2 = 0.6;
			break;
		case '7':
			x1 = 0.9; x2 = 1.0; y1 = 0.5; y2 = 0.6;
			break;
		case '8':
			x1 = 0.0; x2 = 0.1; y1 = 0.6; y2 = 0.7;
			break;
		case '9':
			x1 = 0.1; x2 = 0.2; y1 = 0.6; y2 = 0.7;
			break;
		case '!':
			x1 = 0.2; x2 = 0.3; y1 = 0.6; y2 = 0.7;
			break;
		case '"':
			x1 = 0.3; x2 = 0.4; y1 = 0.6; y2 = 0.7;
			break;
		case '#':
			x1 = 0.4; x2 = 0.5; y1 = 0.6; y2 = 0.7;
			break;
		case '$':
			x1 = 0.5; x2 = 0.6; y1 = 0.6; y2 = 0.7;
			break;
		case '&':
			x1 = 0.6; x2 = 0.7; y1 = 0.6; y2 = 0.7;
			break;
		case '\'':
			x1 = 0.7; x2 = 0.8; y1 = 0.6; y2 = 0.7;
			break;
		case '(':
			x1 = 0.8; x2 = 0.9; y1 = 0.6; y2 = 0.7;
			break;
		case ')':
			x1 = 0.9; x2 = 1.0; y1 = 0.6; y2 = 0.7;
			break;
		case '*':
			x1 = 0.0; x2 = 0.1; y1 = 0.7; y2 = 0.8;
			break;
		case '+':
			x1 = 0.1; x2 = 0.2; y1 = 0.7; y2 = 0.8;
			break;
		case ',':
			x1 = 0.2; x2 = 0.3; y1 = 0.7; y2 = 0.8;
			break;
		case '-':
			x1 = 0.3; x2 = 0.4; y1 = 0.7; y2 = 0.8;
			break;
		case '.':
			x1 = 0.4; x2 = 0.5; y1 = 0.7; y2 = 0.8;
			break;
		case '/':
			x1 = 0.5; x2 = 0.6; y1 = 0.7; y2 = 0.8;
			break;
		case ':':
			x1 = 0.6; x2 = 0.7; y1 = 0.7; y2 = 0.8;
			break;
		case ';':
			x1 = 0.7; x2 = 0.8; y1 = 0.7; y2 = 0.8;
			break;
		case '<':
			x1 = 0.8; x2 = 0.9; y1 = 0.7; y2 = 0.8;
			break;
		case '=':
			x1 = 0.9; x2 = 1.0; y1 = 0.7; y2 = 0.8;
			break;
		case '>':
			x1 = 0.0; x2 = 0.1; y1 = 0.8; y2 = 0.9;
			break;
		case '?':
			x1 = 0.1; x2 = 0.2; y1 = 0.8; y2 = 0.9;
			break;
		case '@':
			x1 = 0.2; x2 = 0.3; y1 = 0.8; y2 = 0.9;
			break;
		case '[':
			x1 = 0.3; x2 = 0.4; y1 = 0.8; y2 = 0.9;
			break;
		case '\\':
			x1 = 0.4; x2 = 0.5; y1 = 0.8; y2 = 0.9;
			break;
		case ']':
			x1 = 0.5; x2 = 0.6; y1 = 0.8; y2 = 0.9;
			break;
		case '^':
			x1 = 0.6; x2 = 0.7; y1 = 0.8; y2 = 0.9;
			break;
		case '_':
			x1 = 0.7; x2 = 0.8; y1 = 0.8; y2 = 0.9;
			break;
		case '`':
			x1 = 0.8; x2 = 0.9; y1 = 0.8; y2 = 0.9;
			break;
		case '{':
			x1 = 0.9; x2 = 1.0; y1 = 0.8; y2 = 0.9;
			break;
		case '|':
			x1 = 0.0; x2 = 0.1; y1 = 0.9; y2 = 1.0;
			break;
		case '}':
			x1 = 0.1; x2 = 0.2; y1 = 0.9; y2 = 1.0;
			break;
		case '~':
			x1 = 0.2; x2 = 0.3; y1 = 0.9; y2 = 1.0;
			break;
		case ' ':
			x1 = 0.8; x2 = 0.9; y1 = 0.9; y2 = 1.0;
			break;
		default:
			x1 = 0.9; x2 = 1.0; y1 = 0.9; y2 = 1.0;
			break;
		}

		glm::vec3 model = glm::vec3(0.0f, 0.0f, 0.0f);
		model.x = loc.x + (0.2*s*i);//0.2 is the length of one text character
		model.y = loc.y;
		//model = glm::translate(model, glm::vec3(loc.x, loc.y, 0.0));
		//model = glm::translate(model, glm::vec3(i*0.1, 0.0, 0.0));
		//model rotate if needed
		//model scale if needed

		shader.setVec3("model", model);

		GLuint vao = createTextVao(x1, y1, x2, y2, s);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

	}

}

int main(int argc, char* argv[])
{
	// Initialize GLFW and OpenGL version
	glfwInit();

#if defined(PLATFORM_OSX)	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#endif

	// Sets the main window
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Comp371 - Project", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// General parameters
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}
	
#if defined(PLATFORM_OSX)
	brickTextureID = loadTexture("Textures/brick.jpg");
	metalTextureID = loadTexture("Textures/metal.jpg");
	tileTextureID = loadTexture("Textures/tile.png");
	skyboxBack = loadTexture("Textures/SkyboxBack.png");
	skyboxBottom = loadTexture("Textures/SkyboxBottom.png");
	skyboxFront = loadTexture("Textures/SkyboxFront.png");
	skyboxLeft = loadTexture("Textures/SkyboxLeft.png");
	skyboxRight = loadTexture("Textures/SkyboxRight.png");
	skyboxTop = loadTexture("Textures/SkyboxTop.png");
	font = loadTexture("Textures/font.png");
#else
	brickTextureID = loadTexture("../Assets/Textures/brick.jpg");
	metalTextureID = loadTexture("../Assets/Textures/metal.jpg");
	tileTextureID = loadTexture("../Assets/Textures/tile.png");
	skyboxBack = loadTexture("../Assets/Textures/SkyboxBack.png");
	skyboxBottom = loadTexture("../Assets/Textures/SkyboxBottom.png");
	skyboxFront = loadTexture("../Assets/Textures/SkyboxFront.png");
	skyboxLeft = loadTexture("../Assets/Textures/SkyboxLeft.png");
	skyboxRight = loadTexture("../Assets/Textures/SkyboxRight.png");
	skyboxTop = loadTexture("../Assets/Textures/SkyboxTop.png");
	font = loadTexture("../Assets/Textures/font.png");
#endif

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// General parameters
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	// Compiles and links shaders
	Shader mainShader("shape.vertexshader", "shape.fragmentshader");
	Shader shadowShader("depth.vertexshader", "depth.fragmentshader");
	Shader textShader("text.vertexshader", "text.fragmentshader");

	// Shader parameters
	mainShader.use();
	mainShader.setFloat("gridUnitLength", gridUnitLength);
	mainShader.setBool("useTextures", useTextures);
	mainShader.setVec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));

	// Creates array and buffer objects
	GLuint cubeVao = createCubeVao();
    GLuint depthMapFBO = buildDepthMapFrameBuffer();
	
	GLuint modelVAO = setupModelVBO("../Assets/Models/aircraft.obj", verticesCount);

	// Shape and wall randomization
	do
	{
		wallPositions = randomizeWall(numberOfCubesInWall);
	} while (wallPositions.size() == 0);
	do
	{
		shapePositions = randomizeShape();
	} while (shapePositions.size() == 0);
	randomizeRotations();

	// Sound
	ISoundEngine* SoundEngine = createIrrKlangDevice();
	SoundEngine->play2D("../audio/audio_signals.wav", true);

	double startTime = clock();
	// Entering Main Loop
	while (!glfwWindowShouldClose(window))
	{



		if (gameScreen && startGame) {
			startTime = clock();
			startGame = false;
			playGame = true;
			shapeMovement = 0.0f;
			level = 0.0;
			for (int i = 0; i < numberOfRotations; i++) {
				rotationHelp[i] = 0.0f;
			}
			processInput(window);
			numberOfRotations = 0;
			minChecked = 0;
			qPressed = false;
			wPressed = false;
			ePressed = false;
			aPressed = false;
			sPressed = false;
			dPressed = false;
		}
		
		currentTime = clock();
		deltaTime2 = (currentTime - startTime)/CLOCKS_PER_SEC;
		timeLeft = totalTime - deltaTime2;
		if (timeLeft < 0.0) {
			gameScreen = false;
			endScreen = true;
			shapeMovement = 0.0f;
			
		}
		int minutes = (int)timeLeft / 60;
		int seconds = (int)timeLeft % 60;
		
		char timeLeftChar[5];
		std::string s = std::to_string(minutes);
		char const *minutesChar = s.c_str();
		timeLeftChar[0] = minutesChar[0];
		timeLeftChar[1] = ':';
		s = std::to_string(100+seconds);
		char const *secondsChar = s.c_str();
		timeLeftChar[2] = secondsChar[1];
		timeLeftChar[3] = secondsChar[2];
		timeLeftChar[4] = '\0';
		// Each frame, reset color of each pixel to glClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Mouse positional difference
		glfwGetCursorPos(window, &mousePosX, &mousePosY);
		mouseXChange = mousePosX - lastMousePosX;
		mouseYChange = mousePosY - lastMousePosY;
		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;
		mouseYChange *= sensitivity;

		// Processing input
		if(!levelBeaten) processInput(window);
		rotateShapes(9);

		// Mouse movement
		calculateCameraViewVector();
		
		// Shadows
		mainShader.setBool("useTextures", false);
		calculateShadows(glm::vec3(-2.0f, 0.0f, 2.0 - shapeMovement * 0.07), shadowShader, mainShader, depthMapFBO, cubeVao, window);

		if (gameScreen && !levelBeaten) {
			glm::mat4 modelCheck = glm::mat4(1.0f);
			for (int i = numberOfRotations - 1; i >= 0; i--) {
				switch (rotations[i]) {
				case 0: modelCheck = glm::rotate(modelCheck, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				case 1:modelCheck = glm::rotate(modelCheck, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				case 2:modelCheck = glm::rotate(modelCheck, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					break;
				case 3:modelCheck = glm::rotate(modelCheck, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					break;
				case 4:modelCheck = glm::rotate(modelCheck, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case 5:modelCheck = glm::rotate(modelCheck, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					break;




				}
			}
			modelCheck = glm::rotate(modelCheck, glm::radians(rotateRandomY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelCheck = glm::rotate(modelCheck, glm::radians(rotateRandomZ), glm::vec3(0.0f, 0.0f, 1.0f));
			modelCheck = glm::rotate(modelCheck, glm::radians(rotateRandomX), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 modelCheck2 = glm::mat4(1.0f);
			levelBeaten = true;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (round(modelCheck[i][j]) != round(modelCheck2[i][j])) levelBeaten = false;
				}
			}

			if (levelBeaten) {
				SoundEngine->play2D("../audio/audio_correct.wav", false);
				level += 1;
			}

		}
		
		// Shape movement and randomization
		if (gameScreen && shapeMovement > 45.0f)
		{
			
			qPressed = false;
			wPressed = false;
			ePressed = false;
			aPressed = false;
			sPressed = false;
			dPressed = false;
			processInput(window);
			for (int i = 0; i < numberOfRotations; i++) {
				rotationHelp[i] = 0.0f;
			}
			numberOfRotations = 0;
			
			minChecked = 0;
			shapeMovement = 0.0f;
			if (!levelBeaten) {
				SoundEngine->play2D("../audio/audio_wrong.wav", false);
			}
			
			levelBeaten = false;
			setLevelParameters();
			randomizeRotations();
			
			do
				wallPositions = randomizeWall(numberOfCubesInWall);
			while (wallPositions.size() == 0);
			do
				shapePositions = randomizeShape();
			while (shapePositions.size() == 0);
		}
		else
			if (gameScreen && levelBeaten) {
				shapeMovement += 10 * shapeSpeed; //speed up on level beaten.
			}
			else if(gameScreen) shapeMovement += shapeSpeed;

		// Display parameters
		mainShader.use();
		buildMatrices(mainShader);
		mainShader.setVec3("viewPos", cameraPos);
		mainShader.setBool("useTextures", useTextures);
		mainShader.setBool("useShadows", useShadows);
		
		if (gameScreen) 
		{

			mainShader.setBool("useTextures", true);
			mainShader.setBool("useShadows", true);
			// Wall display
			glBindVertexArray(cubeVao);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, brickTextureID);
			glUniform1i(glGetUniformLocation(mainShader.ID, "ourTexture"), 2);
			displayWall(mainShader);

			// Shape display
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, metalTextureID);
			glUniform1i(glGetUniformLocation(mainShader.ID, "ourTexture"), 2);
			displayShape(mainShader);
			mainShader.setBool("useTextures", false);

			// Model display
			glBindVertexArray(modelVAO);
			displayModel(mainShader, verticesCount);
		}
		mainShader.setBool("useTextures", true);

		//Skybox display
		mainShader.setBool("useShadows", true); //CHANGE THIS FOR SKYBOX SHADOWS ON/OFF
		glBindVertexArray(cubeVao);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		drawSkybox(mainShader);

		mainShader.setBool("useTextures", useTextures);
		int score = 100000 + level * 25;
		std::string scoreString = std::to_string(score);
		char const *scoreChar = scoreString.c_str();
	
		//displaying text
		textShader.use();
		if (gameScreen) {
			displayText(textShader, "Score:", glm::vec2(-0.9f, 0.9f), 0.4);
			displayText(textShader, scoreChar + 1, glm::vec2(-0.8f, 0.8f), 0.4);
			displayText(textShader, "Time left:", glm::vec2(0.3f, 0.9f), 0.4);
			displayText(textShader, timeLeftChar, glm::vec2(0.5f, 0.8f), 0.4);
		}
		if (startScreen) {
			displayText(textShader, "Press ENTER to start", glm::vec2(-0.8f, 0.0f), 0.4);
			
		}
		if (endScreen) {
			displayText(textShader, "Final Score:", glm::vec2(-0.9f, 0.9f), 0.4);
			displayText(textShader, scoreChar + 1, glm::vec2(-0.8f, 0.8f), 0.4);
			displayText(textShader, "To Play Again", glm::vec2(-0.4f, 0.05f), 0.4);
			displayText(textShader, "Press ENTER", glm::vec2(-0.3f, -0.05f), 0.4);
		}
		// End frame
		glfwSwapBuffers(window);

		// Detect inputs
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	// Shutdown GLFW
	glfwTerminate();

	return 0;
}