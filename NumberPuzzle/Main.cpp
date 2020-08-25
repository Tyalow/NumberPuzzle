#include "Node.h"
#include <stack>
#include <list>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <image_loader_library/stb_image.h>
#include "shader_s.h"
#include "camera.h"

#include<iostream>

void cornerSwap(Node node, int swapLocOne, int swapLocTwo);
void faceSwap(Node node, int swapLocOne, int swapLocTwo, int swapLocThree);
void centerSwap(Node node, int swapLocOne, int swapLocTwo, int swapLocThree, int swapLocFour);
void generateFromSwap(Node node, int swapLoc);
void swapCaseSeperation(Node node);
void DFS(Node node);
void IDDFS(Node root);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int textureGenAlpha(std::string texPath);
void initializeModelList();
void updateModelList(int movePos);
glm::vec3 generateVector(int position);
void incrementalTranslation(int movePos);

//Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Vectors for camera definition
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//Mouse movement variables
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
//Timing variables
float lastFrame = 0.0;
float deltaTime = 0.0;

//Search variables
stack<Node> gameStack;
vector<int> moveList;
bool movesFound = false;
int depthLimit;

//Model maintenance variables
glm::mat4 modelList[8];
int currentState[9];

int main()
{
	int initialState[9] = { 3, 5, 4, 8, 1, 2, 7, 6, 0 };
	for (int i = 0; i < 9; i++)
	{
		currentState[i] = initialState[i];
	}
	
	Node root(initialState);

	IDDFS(root);


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create window object
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//Set callback functions for mouse, scroll wheel, and window resizing
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Set cursor to be locked and disappear for capture
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Texture signatures for numbers
	unsigned int oneTexture = textureGenAlpha("One.png");
	
	unsigned int twoTexture = textureGenAlpha("Two.png");
	unsigned int threeTexture = textureGenAlpha("Three.png");
	unsigned int fourTexture = textureGenAlpha("Four.png");
	unsigned int fiveTexture = textureGenAlpha("Five.png");
	unsigned int sixTexture = textureGenAlpha("Six.png");
	unsigned int sevenTexture = textureGenAlpha("Seven.png");
	unsigned int eightTexture = textureGenAlpha("Eight.png");
	unsigned int textureList[8] = {oneTexture, twoTexture, threeTexture, fourTexture, fiveTexture, sixTexture, sevenTexture, eightTexture};

	glm::mat4 modelOne = glm::mat4(1.0);
	glm::mat4 modelTwo = glm::mat4(1.0);
	glm::mat4 modelThree = glm::mat4(1.0);
	glm::mat4 modelFour = glm::mat4(1.0);
	glm::mat4 modelFive = glm::mat4(1.0);
	glm::mat4 modelSix = glm::mat4(1.0);
	glm::mat4 modelSeven = glm::mat4(1.0);
	glm::mat4 modelEight = glm::mat4(1.0);
	modelList[0] = modelOne; modelList[1] = modelTwo; modelList[2] = modelThree; modelList[3] = modelFour; modelList[4] = modelFive; modelList[5] = modelSix; modelList[6] = modelSeven; modelList[7] = modelEight;

	initializeModelList();

	Shader ourShader("shader.vs", "shader.fs");
	//Triangle vertices for VBO, attributes, in order: position, texture coords
	float vertices[] = {
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f
	};

	//Define Vertex Array Object and bind
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//Define Vertex Buffer Object, bind to GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Load vertex information into GPU memory in buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//How to interpret vertex data (per vertex attribute)
	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//Enable the vertex attribute
	glEnableVertexAttribArray(0);
	//Texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);

	//Controller variables for tile moves
	float nextMoveTime = 0;
	int currentMove = 0;

	//Render loop
	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();

		float currentFrame = time;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//Input call
		processInput(window);

		//Rendering commands
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//Clear color and depth buffers, or info piles up
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		glBindVertexArray(VAO);
		//Position setting for cubes
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		ourShader.setInt("texture1", 0);
		glActiveTexture(GL_TEXTURE0);

		for (int i = 0; i < 8; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureList[i]);
			ourShader.setMat4("model", modelList[i]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		if ((nextMoveTime >= 2) && (currentMove != -1))
		{		
			if (currentMove < moveList.size())
			{
				updateModelList(currentMove);
				currentMove++;
			}
			else
			{
				currentMove = -1;
			}
			nextMoveTime = 0;
		}
		if ((nextMoveTime < 1.5) && (currentMove != -1) && (currentMove < moveList.size()))
		{
			incrementalTranslation(currentMove);
		}
		//Increment controlling time variable if end of movelist hasn't been reached
		if (currentMove != -1)
		{
			nextMoveTime = nextMoveTime + deltaTime;
		}
		

		//Swap buffers and poll IO events (keys press, mouse moved, etc)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	//Clear all allocated resources to glfw
	glfwTerminate();

	return 0;
}

void IDDFS(Node root)
{
	depthLimit = 0;
	while(!movesFound)
	{
		DFS(root);
		depthLimit++;
	}
}

void DFS(Node node)
{
	if (!movesFound)
	{
		if (node.isWin == true)
		{
			moveList = node.moves;
			movesFound = true;
		}
		else 
		{
			if (node.depth < depthLimit)
			{
				swapCaseSeperation(node);
				if (node.hasParent)
				{
					if (node.zeroLocation == 0 || node.zeroLocation == 2 || node.zeroLocation == 6 || node.zeroLocation == 8)
					{
						Node nextNode = gameStack.top();
						gameStack.pop();
						DFS(nextNode);
					}
					else if (node.zeroLocation == 1 || node.zeroLocation == 3 || node.zeroLocation == 5 || node.zeroLocation == 7)
					{
						for (int i = 0; i < 2; i++)
						{
							Node nextNode = gameStack.top();
							gameStack.pop();
							DFS(nextNode);
						}
					}
					else if (node.zeroLocation == 4)
					{
						for (int i = 0; i < 3; i++)
						{
							Node nextNode = gameStack.top();
							gameStack.pop();
							DFS(nextNode);
						}
					}
				}
				else
				{
					if (node.zeroLocation == 0 || node.zeroLocation == 2 || node.zeroLocation == 6 || node.zeroLocation == 8)
					{
						for (int i = 0; i < 2; i++)
						{
							Node nextNode = gameStack.top();
							gameStack.pop();
							DFS(nextNode);
						}
					}
					else if (node.zeroLocation == 1 || node.zeroLocation == 3 || node.zeroLocation == 5 || node.zeroLocation == 7)
					{
						for (int i = 0; i < 3; i++)
						{
							Node nextNode = gameStack.top();
							gameStack.pop();
							DFS(nextNode);
						}
					}
					else if (node.zeroLocation == 4)
					{
						for (int i = 0; i < 4; i++)
						{
							Node nextNode = gameStack.top();
							gameStack.pop();
							DFS(nextNode);
						}
					}
				}
				
				
			}
			
		}

	}

}

void swapCaseSeperation(Node node)
{
	if (node.zeroLocation == 0)
	{
		cornerSwap(node, 1, 3);
	}
	else if (node.zeroLocation == 1)
	{
		faceSwap(node, 0, 2, 4);
	}
	else if (node.zeroLocation == 2)
	{
		cornerSwap(node, 1, 5);
	}
	else if (node.zeroLocation == 3)
	{
		faceSwap(node, 0, 4, 6);
	}
	else if (node.zeroLocation == 4)
	{
		centerSwap(node, 1, 3, 5, 7);
	}
	else if (node.zeroLocation == 5)
	{
		faceSwap(node, 2, 4, 8);
	}
	else if (node.zeroLocation == 6)
	{
		cornerSwap(node, 3, 7);
	}
	else if (node.zeroLocation == 7)
	{
		faceSwap(node, 4, 6, 8);
	}
	else if (node.zeroLocation == 8)
	{
		cornerSwap(node, 5, 7);
	}
}

void cornerSwap(Node node, int swapLocOne, int swapLocTwo)
{
	if (!node.hasParent)
	{
		generateFromSwap(node, swapLocOne);
		generateFromSwap(node, swapLocTwo);
	}
	else
	{
		
		if (node.parentZeroLocation == swapLocOne)
		{
			generateFromSwap(node, swapLocTwo);
		}
		else if (node.parentZeroLocation == swapLocTwo)
		{
			generateFromSwap(node, swapLocOne);
		}
	}
}

void faceSwap(Node node, int swapLocOne, int swapLocTwo, int swapLocThree)
{
	if (!node.hasParent)
	{
		generateFromSwap(node, swapLocOne);
		generateFromSwap(node, swapLocTwo);
		generateFromSwap(node, swapLocThree);
	}
	else
	{
		if (node.parentZeroLocation == swapLocOne)
		{
			generateFromSwap(node, swapLocTwo);
			generateFromSwap(node, swapLocThree);
		}
		else if (node.parentZeroLocation == swapLocTwo)
		{
			generateFromSwap(node, swapLocOne);
			generateFromSwap(node, swapLocThree);
		}
		else if (node.parentZeroLocation == swapLocThree)
		{
			generateFromSwap(node, swapLocOne);
			generateFromSwap(node, swapLocTwo);
		}
	}
}

void centerSwap(Node node, int swapLocOne, int swapLocTwo, int swapLocThree, int swapLocFour)
{
	if (!node.hasParent)
	{
		generateFromSwap(node, swapLocOne);
		generateFromSwap(node, swapLocTwo);
		generateFromSwap(node, swapLocThree);
		generateFromSwap(node, swapLocFour);
	}
	else
	{
		if (node.parentZeroLocation == swapLocOne)
		{
			generateFromSwap(node, swapLocTwo);
			generateFromSwap(node, swapLocThree);
			generateFromSwap(node, swapLocFour);
		}
		else if (node.parentZeroLocation == swapLocTwo)
		{
			generateFromSwap(node, swapLocOne);
			generateFromSwap(node, swapLocThree);
			generateFromSwap(node, swapLocFour);
		}
		else if (node.parentZeroLocation == swapLocThree)
		{
			generateFromSwap(node, swapLocOne);
			generateFromSwap(node, swapLocTwo);
			generateFromSwap(node, swapLocFour);
		}
		else if (node.parentZeroLocation == swapLocFour)
		{
			generateFromSwap(node, swapLocOne);
			generateFromSwap(node, swapLocTwo);
			generateFromSwap(node, swapLocThree);
		}
	}
}

void generateFromSwap(Node node, int swapLoc)
{
	int zeroLoc = node.zeroLocation;
	int puzzle[9];
	for (int i = 0; i < 9; i++)
	{
		puzzle[i] = node.puzzle[i];
	}
	int swapValue = puzzle[swapLoc];
	puzzle[swapLoc] = 0;
	puzzle[zeroLoc] = swapValue;
	Node newNode(puzzle, node, swapLoc);
	gameStack.push(newNode);	
}

//Process all inputs
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	//Camera keyboard controls, defined through camera class enum to be device independent 
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

//Callback for mouse movement (camera mouse controls)
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
//Callback for scroll wheel
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


//Called on window size change (by user or OS)
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//matches viewport to window dimensions on init and resize
	glViewport(0, 0, width, height);
}

unsigned int textureGenAlpha(std::string texPath)
{
	//Load in textures
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//Set texture wrap/mipmap properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Generate texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}

void initializeModelList()
{
	for (int i = 0; i < 9; i++)
	{
		//Check if empty square
		if (currentState[i] != 0)
		{
			if (i == 0)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(-1,1,0));
			}
			else if (i == 1)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(0, 1, 0));
			}
			else if (i == 2)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(1, 1, 0));
			}
			else if (i == 3)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(-1, 0, 0));
			}
			else if (i == 5)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(1, 0, 0));
			}
			else if (i == 6)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(-1, -1, 0));
			}
			else if (i == 7)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(0, -1, 0));
			}
			else if (i == 8)
			{
				modelList[currentState[i] - 1] = glm::translate(modelList[currentState[i] - 1], glm::vec3(1, -1, 0));
			}
		}
	}
}

void updateModelList(int movePos)
{
	int move = moveList.at(movePos);
	int zeroPos;
	int exchangeVal = currentState[move];
	for (int i = 0; i < 9; i++)
	{
		if (currentState[i] == 0)
		{
			zeroPos = i;
		}
	}
	//Updating state tracker
	currentState[move] = 0;
	currentState[zeroPos] = exchangeVal;
	//Updating tile position
	glm::vec3 zeroVec = generateVector(zeroPos);
	if (exchangeVal > 0)
	{
		modelList[exchangeVal - 1] = glm::mat4(1.0);
		modelList[exchangeVal - 1] = glm::translate(modelList[exchangeVal - 1], zeroVec);
	}
}

void incrementalTranslation(int movePos)
{
	int move = moveList.at(movePos);
	int zeroPos;
	int exchangeVal = currentState[move];
	for (int i = 0; i < 9; i++)
	{
		if (currentState[i] == 0)
		{
			zeroPos = i;
		}
	}

	float scalingFactor = deltaTime / 1.5;
	//Updating tile position
	glm::vec3 zeroVec = generateVector(zeroPos);
	glm::vec3 tileVec = generateVector(move);
	glm::vec3 translationVec = glm::vec3((zeroVec.x - tileVec.x) * scalingFactor, (zeroVec.y - tileVec.y) * scalingFactor, 0);
	if (exchangeVal > 0)
	{
		modelList[exchangeVal - 1] = glm::translate(modelList[exchangeVal - 1], translationVec);
	}
}

glm::vec3 generateVector(int position)
{
	glm::vec3 posVec = glm::vec3(0,0,0);
	if (position == 0)
	{
		posVec = glm::vec3(-1, 1, 0);
	}
	else if (position == 1)
	{
		posVec = glm::vec3(0, 1, 0);
	}
	else if (position == 2)
	{
		posVec = glm::vec3(1, 1, 0);
	}
	else if (position == 3)
	{
		posVec = glm::vec3(-1, 0, 0);
	}
	else if (position == 5)
	{
		posVec = glm::vec3(1, 0, 0);
	}
	else if (position == 6)
	{
		posVec = glm::vec3(-1, -1, 0);
	}
	else if (position == 7)
	{
		posVec = glm::vec3(0, -1, 0);
	}
	else if (position == 8)
	{
		posVec = glm::vec3(1, -1, 0);
	}
	return posVec;
}


