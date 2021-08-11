#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <filesystem>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" //image loading library from https://github.com/nothings/stb
using namespace std;
GLuint loadTexture(const char *filename);
glm::mat4 view;
glm::mat4 projection;
float gridUnitLength = 0.01953125f;
float shapeMovement = 0.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.07*6.0f, 1.0f+shapeMovement*0.07);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 direction;
float cameraSpeed = 2.0f;
float sensitivity = 0.03f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 512.0f;
float lastY = 384.0f;
float yaw = -90.0f;
float pitch = -10.0f;
float fov = 45.0f;
float wallMovement = 0.0f;
double mousePosX, mousePosY;
double lastMousePosX, lastMousePosY;
double mouseXChange;
double mouseYChange;
//Stores rendering modes, default is triangles.
GLenum renderModeShape = GL_TRIANGLES;



//FOR GLOW
bool useGlow = false;
bool kPressed = false;

//Rotations
bool qPressed = false;
bool ePressed = false;
bool wPressed = false;
bool sPressed = false;
bool aPressed = false;
bool dPressed = false;



//To randomize on release on only
bool rPressed = false;
//TEXTURE + SHADOW TOGGLE
bool xPressed = false;
bool bPressed = false;
bool useTextures = false;
bool useShadows = false;
//Ryan Random Related Things
void randomizeShape();
glm::vec3 positionsShape[9];
glm::vec3 defaultPositionsShape[] =
{
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(2.0f, 1.0f, 1.0f),
	glm::vec3(3.0f, 1.0f, 1.0f),
	glm::vec3(3.0f, 2.0f, 1.0f),
	glm::vec3(3.0f, 2.0f, 2.0f)
};

//Texture Things:
GLuint brickTextureID;
GLuint metalTextureID;
GLuint tileTextureID;



//SHAPE TRANSFORMATIONS



float translateShapeX = 0.0f;
float translateShapeY = 0.0f;
float translateShapeZ = 0.0f;
float scaleShape = 1.0f;
float rotateShapeX = 0.0f;
float rotateShapeY = 0.0f;
float rotateShapeZ = 0.0f;


//to only rotate once 
bool rotateShape = false;
bool glowingObjects = false;


GLuint createCubeVao() //Taken from lab and modified. 
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


			- 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,//Background
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 20.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 20.0f, 20.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 20.0f, 20.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 20.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

			- 0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 20.0f, //Floor
		 0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,  20.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 20.0f, 20.0f,
		 0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,  20.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 20.0f,
		-0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f

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



void displayShape(Shader shader)
{

	for (int i = 0; i < 9; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3((0.5f + translateShapeX) * 0.07f, (3.0f + translateShapeY) * 0.07f, (3.0f + translateShapeZ - shapeMovement) * 0.07f));
		model = glm::rotate(model, glm::radians(rotateShapeZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(rotateShapeY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotateShapeX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f * scaleShape, 0.07f * scaleShape, 0.07f * scaleShape));
		model = glm::translate(model, positionsShape[i]);
		model = glm::translate(model, glm::vec3(-1.5f, -1.0f, -1.0f));
		shader.setMat4("model", model);

		glDrawArrays(renderModeShape, 0, 36);
	}

}


void displayFloor(Shader shader)
{

	
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 0.0f, 10.0f));
		shader.setMat4("model", model);

		glDrawArrays(renderModeShape, 42, 6);
	

}


void displayBackground(Shader shader)
{


		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, -5.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 0.0f));
	
		shader.setMat4("model", model);

		glDrawArrays(renderModeShape, 36, 6);
	

}




void displayWall(Shader shader)
{


	glm::vec3 positions[] =
	{
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(-1.0f, 2.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		glm::vec3(1.0f, 2.0f, 0.0f),
		glm::vec3(2.0f, 2.0f, 0.0f),
		glm::vec3(2.0f, 3.0f, 0.0f),
		glm::vec3(3.0f, 3.0f, 0.0f),
		glm::vec3(4.0f, 3.0f, 0.0f),
		glm::vec3(4.0f, 2.0f, 0.0f),
		glm::vec3(4.0f, 1.0f, 0.0f),
		glm::vec3(4.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, -1.0f, 0.0f),
		glm::vec3(3.0f, -1.0f, 0.0f),
		glm::vec3(4.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 3.0f, 0.0f),
		glm::vec3(0.0f, 3.0f, 0.0f),
		glm::vec3(1.0f, 3.0f, 0.0f),
	};



	for (int i = 0; i < sizeof(positions)/sizeof(glm::vec3); i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.03f));
		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, -100.0f+wallMovement));
		model = glm::translate(model, positions[i]);
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

void calculateShadows(Shader shadowShader, Shader mainShader, GLuint depthMapFBO, GLuint vao, GLFWwindow* window)
{
	glm::vec3 lightPosition(0.0, 30.0f * gridUnitLength, 0.0);
	glm::vec3 lightFocus(0.0, 0.0, 0.0);
	glm::vec3 lightDirection = glm::normalize(lightFocus - lightPosition);

	glm::mat4 lightProjectionMatrix = glm::perspective(65.0f, 1.0f, 0.01f, 100.0f);
	glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, lightPosition + lightDirection, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0, 30.0f * gridUnitLength, 0.0));

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
	
	/*int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void processInput(GLFWwindow* window)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	//Camera Controls
	const float actualSpeed = cameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += actualSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= actualSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * actualSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * actualSpeed;
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) { //HAVE TO FIX THIS IMPORTANT MAYBE IT WILL BE FIXED BY ITSELF
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		//lastX = 512.0f;
		//lastY = 384.0f;
		//firstMouse = true;
		yaw = -90.0f;
		pitch = 0.0f;
		fov = 45.0f;

	}



	//Shape Reset
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {

		renderModeShape = GL_TRIANGLES;
		translateShapeX = 0.0;
		translateShapeY = 0.0;
		translateShapeZ = 0.0;

	}


	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // texture controls
	{
		ePressed = true;

	}


	if (ePressed && glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) // Rotate clockwise
	{
		ePressed = false;
		rotateShapeY += 90;


	}


	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // texture controls
	{
		qPressed = true;

	}

	if (qPressed && glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) // Rotate counterclockwise
	{
		qPressed = false;
		rotateShapeY -= 90;

	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // texture controls
	{
		wPressed = true;

	}

	if (wPressed && glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) // Rotate counterclockwise
	{
		wPressed = false;
		rotateShapeX += 90;



	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // texture controls
	{
		sPressed = true;

	}

	if (sPressed && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) // Rotate counterclockwise
	{
		sPressed = false;
		rotateShapeX -= 90;

	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // texture controls
	{
		aPressed = true;

	}
	
	if (aPressed && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) // Rotate counterclockwise
	{
		aPressed = false;
		rotateShapeZ += 90;

	}
	 
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // texture controls
	{
		dPressed = true;

	}

	if (dPressed && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) // Rotate counterclockwise
	{
		dPressed = false;
		rotateShapeZ -= 90;

	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Reset Shapes
	{

		renderModeShape = GL_TRIANGLES;
		rotateShapeX = 0.0;
		rotateShapeY = 0.0;
		rotateShapeZ = 0.0;
		scaleShape = 1.0;
		translateShapeX = 0.0;
		translateShapeY = 0.0;
		translateShapeZ = 0.0;
		for (int i = 0; i < 9; i++) {
			positionsShape[i] = defaultPositionsShape[i];

		}
	}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // texture controls
		{
			xPressed = true;

		}

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE && xPressed)
		{
			xPressed = false;
			if (useTextures == true) useTextures = false;
			else useTextures = true;
		}

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) // glow controls
		{
			kPressed = true;

		}

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE && kPressed)
		{
			kPressed = false;
			if (useGlow == true) useGlow = false;
			else useGlow = true;
		}


		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) //shadow controls
		{
			bPressed = true;

		}

		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE && bPressed)
		{
			bPressed = false;
			if (useShadows == true) useShadows = false;
			else useShadows = true;
		}


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
			mouseYChange *= sensitivity;
			pitch += mouseYChange;

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

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			mouseXChange *= sensitivity;
			yaw += mouseXChange;
			glm::vec3 direction;
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(direction);

		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			fov += (float)mouseYChange;
			if (fov < 1.0f)
				fov = 1.0f;
			if (fov > 45.0f)
				fov = 45.0f;
		}
	}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// TODO Redo because became incompatible after shadows implementation
	float aspectRatio = 1024.0f / 768.0f;
	float newAspectRatio = (float)width / float(height);
	float ratioDifference = aspectRatio - newAspectRatio;

	if (ratioDifference > -0.01f && ratioDifference < 0.01f)
		glViewport(0, 0, width, height);
	else
	{
		int newHeight = (float)width / aspectRatio;
		glViewport(0, 0, width, newHeight);
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

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Comp371 - Assignment 1", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
//	glEnable(GL_CULL_FACE);
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

	for (int i = 0; i < 9; i++) {
		positionsShape[i] = defaultPositionsShape[i];
	}

#if defined(PLATFORM_OSX)
	brickTextureID = loadTexture("Textures/brick.jpg");
	metalTextureID = loadTexture("Textures/metal.jpg");
	tileTextureID = loadTexture("Textures/tile.png");
#else
	brickTextureID = loadTexture("../Assets/Textures/brick.jpg");
	metalTextureID = loadTexture("../Assets/Textures/metal.jpg");
	tileTextureID = loadTexture("../Assets/Textures/tile.png");
#endif
	glEnable(GL_CULL_FACE);
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	// Compile and link shaders here ...
	Shader mainShader("shape.vertexshader", "shape.fragmentshader");
	Shader shadowShader("depth.vertexshader", "depth.fragmentshader");
	

	mainShader.use();
	mainShader.setFloat("gridUnitLength", gridUnitLength);
	mainShader.setBool("useTextures", useTextures);
	
	GLuint cubeVao = createCubeVao();


	GLuint depthMapFBO = buildDepthMapFrameBuffer();

	// glowFBO = buildGlowFrameBuffer();
	

	// Entering Main Loop
	while (!glfwWindowShouldClose(window))
	{
		// Each frame, reset color of each pixel to glClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//MOUSE MOVEMENTS.
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		// Calculate the positional differences
		mouseXChange = mousePosX - lastMousePosX;
		mouseYChange = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		// Processing input
		processInput(window);

		
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
		cameraPos = glm::vec3(0.0f, 0.07*6.0f, 1.0f - shapeMovement * 0.07);
		mainShader.setBool("useTextures", false);
		calculateShadows(shadowShader, mainShader, depthMapFBO, cubeVao, window);
		
		if (shapeMovement > 45.0f) shapeMovement = 0.0f;
		else shapeMovement += 0.10f;

		// Display 
		mainShader.use();
		buildMatrices(mainShader);
		mainShader.setVec3("viewPos", cameraPos);
		mainShader.setBool("useTextures", useTextures);
		mainShader.setBool("useShadows", useShadows);
		
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, brickTexture);
		glBindVertexArray(cubeVao);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brickTextureID);
		glUniform1i(glGetUniformLocation(mainShader.ID, "ourTexture"), 2);
		
		displayWall(mainShader);
		
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, metalTextureID);
			glUniform1i(glGetUniformLocation(mainShader.ID, "ourTexture"), 2);
			displayShape(mainShader);
		
		







		mainShader.setBool("useTextures", true);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tileTextureID);
		glUniform1i(glGetUniformLocation(mainShader.ID, "ourTexture"), 2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		displayFloor(mainShader);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brickTextureID);
		glUniform1i(glGetUniformLocation(mainShader.ID, "ourTexture"), 2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		displayBackground(mainShader);
		mainShader.setBool("useTextures", useTextures);
		

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

void randomizeShape() {
	while (true) {
		int x = rand() % 4;
		int y = rand() % 3;
		int z = 2;

		if (x == 0 && y == 0) {
			positionsShape[0] = glm::vec3(x, y, z);
			break;
		}
		if (x == 0 && y == 1) {
			positionsShape[0] = glm::vec3(x, y, z);
			break;
		}
		if (x == 1 && y == 0) {
			positionsShape[0] = glm::vec3(x, y, z);
			break;
		}
		if (x == 1 && y == 1) {
			positionsShape[0] = glm::vec3(x, y, z);
			break;
		}
		if (x == 2 && y == 1) {
			positionsShape[0] = glm::vec3(x, y, z);
			break;
		}
		if (x == 3 && y == 1) {
			positionsShape[0] = glm::vec3(x, y, z);
			break;
		}
		if (x == 3 && y == 2) {
			positionsShape[0] = glm::vec3(x, y, z);
			break;
		}
	}

	for (int i = 1; i < 9; i++) {


		bool next = true;
		while (next) {
			int nextBlock = rand() % i;
			float y = positionsShape[nextBlock].y;
			float x = positionsShape[nextBlock].x;
			float z = positionsShape[nextBlock].z;
			int nextChange = rand() % 6;

			switch (nextChange) {
			case 0:
				x += 1;
				break;

			case 1:
				x -= 1;
				break;
			case 2:
				y += 1;
				break;
			case 3:
				y -= 1;
				break;
			case 4:
				z += 1;
			case 5:
				z -= 1;
				break;
			}
			if (x == 0.0f && y == 0.0f) {
				positionsShape[i] = glm::vec3(x, y, z);
				next = false;
				for (int j = 0; j < i; j++) {
					if (positionsShape[j].x == positionsShape[i].x && positionsShape[j].y == positionsShape[i].y && positionsShape[i].z == positionsShape[j].z) next = true;
				}
			}

			if (x == 0.0f && y == 1.0f) {
				positionsShape[i] = glm::vec3(x, y, z);
				next = false;
				for (int j = 0; j < i; j++) {
					if (positionsShape[j].x == positionsShape[i].x && positionsShape[j].y == positionsShape[i].y && positionsShape[i].z == positionsShape[j].z) next = true;
				}
			}

			if (x == 1.0f && y == 0.0f) {
				positionsShape[i] = glm::vec3(x, y, z);
				next = false;
				for (int j = 0; j < i; j++) {
					if (positionsShape[j].x == positionsShape[i].x && positionsShape[j].y == positionsShape[i].y && positionsShape[i].z == positionsShape[j].z) next = true;
				}
			}

			if (x == 1.0f && y == 1.0f) {
				positionsShape[i] = glm::vec3(x, y, z);
				next = false;
				for (int j = 0; j < i; j++) {
					if (positionsShape[j].x == positionsShape[i].x && positionsShape[j].y == positionsShape[i].y && positionsShape[i].z == positionsShape[j].z) next = true;
				}
			}

			if (x == 2.0f && y == 1.0f) {
				positionsShape[i] = glm::vec3(x, y, z);
				next = false;
				for (int j = 0; j < i; j++) {
					if (positionsShape[j].x == positionsShape[i].x && positionsShape[j].y == positionsShape[i].y && positionsShape[i].z == positionsShape[j].z) next = true;
				}
			}

			if (x == 3.0f && y == 1.0f) {
				positionsShape[i] = glm::vec3(x, y, z);
				next = false;
				for (int j = 0; j < i; j++) {
					if (positionsShape[j].x == positionsShape[i].x && positionsShape[j].y == positionsShape[i].y && positionsShape[i].z == positionsShape[j].z) next = true;
				}
			}

			if (x == 3.0f && y == 2.0f) {
				positionsShape[i] = glm::vec3(x, y, z);
				next = false;
				for (int j = 0; j < i; j++) {
					if (positionsShape[j].x == positionsShape[i].x && positionsShape[j].y == positionsShape[i].y && positionsShape[i].z == positionsShape[j].z) next = true;
				}
			}
		}
	}

};

GLuint loadTexture(const char *filename) //FROM LAB
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
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
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
