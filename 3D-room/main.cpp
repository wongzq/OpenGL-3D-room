#include <gl/glew.h>
#include <gl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <string>

#pragma warning(disable:4996)
using namespace std;

GLuint VBO;
GLuint VAO;
GLuint program;

GLfloat ground[] = {
	// coordinates			// normal vector
	-500.0, 0.0,    0.0,	0.0, 1.0, 0.0,
	-500.0, 0.0, 1000.0,	0.0, 1.0, 0.0,
	+500.0, 0.0, 1000.0,	0.0, 1.0, 0.0,
	+500.0, 0.0,    0.0,	0.0, 1.0, 0.0
};

//predefined matrix type from GLM 
glm::mat4 model;	// model matrix
glm::mat4 view;		// view matrix
glm::mat4 proj;		// projection matrix - camera settings

// Camera position
GLfloat camX = 0.0;
GLfloat camY = 350.0;
GLfloat camZ = 2000.0;
// Camera facing direction
GLfloat dirX = 0.0;
GLfloat dirY = 0.0;
GLfloat dirZ = 0.0;

// Point Light Source
glm::vec3 lightPos = { 0, 250, 1000 };	// Light position in world coordinate
glm::vec3 lightColor = { 0.5, 0.5, 0.5 };	// white light
glm::vec3 ambient = { 0.3, 0.3, 0.3 };	// fixed ambient light

enum Object { NONE, FLOOR, ITEM };
int object;
int choice = 4;

enum Key { ALT, UP, DOWN, LEFT, RIGHT, PG_UP, PG_DN, KEYS_LENGTH };
bool keys[KEYS_LENGTH] = { false };

// function prototype
GLuint loadShaders(const std::string, const std::string);
void init(void);
void drawGround(void);
void drawWall(float, float, float);
void drawCupboard(float, float, float, float);
void drawCupboardDoor(float, float, float, float);
void drawBook(float, float, float, glm::vec3);
void drawLamp(float, float, float);
void drawLaptop(float, float, float);
void drawChair(float, float, float);
void drawTableLeg(float, float, float);
void drawTable(float, float, float);

void display(void);
void speckeyup(int, int, int);
void speckey(int, int, int);
void processMenu(int);
void mymenu(void);
int main(int, char**);

// function to load shaders
GLuint loadShaders(const std::string vShaderFile, const std::string fShaderFile) {
	GLint status;	// to check compile and linking status

	// VERTEX SHADER
	// load vertex shader code from file
	std::string vShaderCodeStr;
	std::ifstream vShaderStream(vShaderFile, std::ios::in);
	if (vShaderStream.is_open()) {
		// read from stream line by line and append it to shader code
		std::string line = "";
		while (std::getline(vShaderStream, line))
			vShaderCodeStr += line + "\n";
		vShaderStream.close();
	}
	else {
		// output error message and exit
		std::cout << "Failed to open vertex shader file - " << vShaderFile << std::endl;
		exit(EXIT_FAILURE);
	}

	// FRAGMENT SHADER
	// load fragment shader code from file
	std::string fShaderCodeStr;
	std::ifstream fShaderStream(fShaderFile, std::ios::in);
	if (fShaderStream.is_open()) {
		// read from stream line by line and append it to shader code
		std::string line = "";
		while (std::getline(fShaderStream, line))
			fShaderCodeStr += line + "\n";
		fShaderStream.close();
	}
	else {
		// output error message and exit
		std::cout << "Failed to open fragment shader file - " << fShaderFile << std::endl;
		exit(EXIT_FAILURE);
	}

	// compile vertex shader
	GLuint vShaderID = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vShaderCode = vShaderCodeStr.c_str();
	glShaderSource(vShaderID, 1, &vShaderCode, NULL);

	status = GL_FALSE;
	glCompileShader(vShaderID);
	glGetShaderiv(vShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		std::cout << "Failed to compile vertex shader - " << vShaderFile << std::endl;
		int infoLogLength;
		glGetShaderiv(vShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMsg = new char[static_cast<__int64>(infoLogLength) + 1];
		glGetShaderInfoLog(vShaderID, infoLogLength, NULL, errorMsg);
		std::cout << errorMsg << std::endl;
		delete[] errorMsg;
		exit(EXIT_FAILURE);
	}

	// compile fragment shader
	GLuint fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fShaderCode = fShaderCodeStr.c_str();
	glShaderSource(fShaderID, 1, &fShaderCode, NULL);

	status = GL_FALSE;
	glCompileShader(fShaderID);
	glGetShaderiv(fShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		std::cout << "Failed to compile fragment shader - " << fShaderFile << std::endl;
		int infoLogLength;
		glGetShaderiv(fShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMsg = new char[static_cast<__int64>(infoLogLength) + 1];
		glGetShaderInfoLog(fShaderID, infoLogLength, NULL, errorMsg);
		std::cout << errorMsg << std::endl;
		delete[] errorMsg;
		exit(EXIT_FAILURE);
	}

	// create program
	GLuint programID = glCreateProgram();
	// attach shaders to program object
	glAttachShader(programID, vShaderID);
	glAttachShader(programID, fShaderID);
	// link program object
	glLinkProgram(programID);

	// check link status
	status = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		std::cout << "Failed to link program object." << std::endl;
		int infoLogLength;
		glGetShaderiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMsg = new char[static_cast<__int64>(infoLogLength) + 1];
		glGetShaderInfoLog(programID, infoLogLength, NULL, errorMsg);
		std::cout << errorMsg << std::endl;
		delete[] errorMsg;
		exit(EXIT_FAILURE);
	}

	return programID;
}

void init(void) {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// for self created objects (floor)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// for GLUT built-in objects
	glutSetVertexAttribCoord3(2);
	glutSetVertexAttribNormal(3);

	program = loadShaders("vertexShader.glsl", "fragmentShader.glsl");
	// Activate the shader program
	glUseProgram(program);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	// fov, aspect, near, far
	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 5000.0f);
	unsigned int projLoc = glGetUniformLocation(program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	// Pass light position vector to fragment shader for light calculation
	unsigned int lightPosLoc = glGetUniformLocation(program, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	// Pass light color to fragment shader for light calculation
	unsigned int lightColorLoc = glGetUniformLocation(program, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	// Pass ambient light value to fragment shader for light calculation
	unsigned int ambientLoc = glGetUniformLocation(program, "ambient");
	glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
}

// draw room - ground
void drawGround(void) {
	// Set buffer to use to draw the floor
	unsigned int objLoc = glGetUniformLocation(program, "obj");

	object = Object::FLOOR;

	glUniform1i(objLoc, object);

	// Set drawing color
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_QUADS, 0, 4);
}

// draw room - wall
void drawWall(float x, float z, float yRotate) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 1000.0;
	float height = 500.0;
	float depth = 10.0;

	glm::vec3 color = glm::vec3(1.0, 0.8, 1.0);

	object = Object::ITEM;

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(
		x + (x == 0 ? 0 : x > 0 ? depth / 2 : -depth / 2),
		height / 2.0,
		z - depth / 2.0));
	model = glm::rotate(model, glm::radians(yRotate), glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(x == 0 ? width + 2 * depth : width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(color));
	glutSolidCube(1.0);
}

// draw cupboard
void drawCupboard(float x, float y, float z, float yRotate) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 300;
	float height = 500;
	float depth = 100;

	glm::vec3 brownColor = glm::vec3(0.60, 0.46, 0.32);

	object = Object::ITEM;

	float finalX = x + depth / 2.0f;
	float finalY = height / 2.0f;
	float finalZ = z;

	// left board
	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(finalX, finalY, finalZ));
	model = glm::rotate(model, glm::radians(yRotate), glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(brownColor));
	glutSolidCube(1.0);

	drawCupboardDoor(finalX + depth / 2.0f, y, finalZ + width / 4.0f, yRotate);
	drawCupboardDoor(finalX + depth / 2.0f, y, finalZ - width / 4.0f, yRotate);
}

void drawCupboardDoor(float x, float y, float z, float yRotate) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 125;
	float height = 450;
	float depth = 5;

	glm::vec3 brownColor = glm::vec3(0.50, 0.36, 0.22);

	object = Object::ITEM;

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x + depth / 2, y + height / 2.0 + 25, z));
	model = glm::rotate(model, glm::radians(yRotate), glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(brownColor));
	glutSolidCube(1.0);
}

// draw bed
void drawBed(float x, float y, float z) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 250;
	float height = 100;
	float depth = 500;
	float pillarRadius = 15;

	glm::vec3 whiteColor = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 blanketColor = glm::vec3(0.9, 0.9, 0.9);
	glm::vec3 brownColor = glm::vec3(0.5, 0.36, 0.22);
	glm::vec3 lightBrownColor = glm::vec3(0.83, 0.77, 0.63);

	object = Object::ITEM;
	glUniform1i(objLoc, object);

	// bed body
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width / 2 - pillarRadius, y + height / 2.0, z));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(brownColor));
	glutSolidCube(1.0);

	// bed board
	float width2 = width;
	float height2 = height * 1.8f;
	float depth2 = 20.0f;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width / 2 - pillarRadius, y + height2 / 2.0, z - depth / 2.0));
	model = glm::scale(model, glm::vec3(width2, height2, depth2));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(brownColor));
	glutSolidCube(1.0);

	// pillow - left ball
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width / 2 - pillarRadius - 75, y + height + 15, z - depth / 2.0 + 50));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(whiteColor));
	glutSolidSphere(20, 20, 20);

	// pillow - right ball
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width / 2 - pillarRadius + 75, y + height + 15, z - depth / 2.0 + 50));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(whiteColor));
	glutSolidSphere(20, 20, 20);

	// pillow - middle cylinder
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width / 2 - pillarRadius - 75, y + height + 15, z - depth / 2.0 + 50));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(whiteColor));
	glutSolidCylinder(20, 150, 20, 20);

	// bed blanket
	float width3 = width + 10;
	float height3 = height + 10;
	float depth3 = depth * 0.7f;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width / 2 - pillarRadius, y + height3 / 2.0, z + depth * 0.15 + 10));
	model = glm::scale(model, glm::vec3(width3, height3, depth3));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(blanketColor));
	glutSolidCube(1.0);

	// bed pillar top left
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width - pillarRadius, y, z - depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(15.0, height * 2.0, 50, 20);

	// bed pillar top right
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - pillarRadius, y, z - depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(15.0, height * 2.0, 50, 20);

	// bed pillar bottom left
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width - pillarRadius, y, z + depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(15.0, height * 1.25, 50, 20);

	// bed pillar bottom right
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - pillarRadius, y, z + depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(pillarRadius, height * 1.25, 50, 20);
}

// draw book
void drawBook(float x, float y, float z, glm::vec3 color) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 10;
	float height = 50;
	float depth = 30;

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x + width / 2, y + height / 2.0, z + depth / 2));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(color));
	glutSolidCube(1.0);

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x + width / 2, y + height / 2, z - 1 + depth / 2));
	model = glm::scale(model, glm::vec3(width - 2, height + 0.2, depth - 2));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
	glutSolidCube(1.0);
}

// draw lamp
void drawLamp(float x, float y, float z) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	glm::vec3 lampColor = glm::vec3(1, 1, 1);
	glm::vec3 lampStandColor = glm::vec3(0.3, 0.3, 0.3);

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lampStandColor));
	glutSolidCone(25, 35, 20, 20);

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 40, z));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lampColor));
	glutSolidSphere(25, 20, 20);
}

// draw laptop

// draw chair

// draw table - leg
void drawTableLeg(float x, float y, float z) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 20.0;
	float height = 200.0;
	float depth = 20.0;

	glm::vec3 brownColor = glm::vec3(0.6, 0.46, 0.32);

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + height / 2.0, z));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(brownColor));
	glutSolidCube(1.0);
}

// draw table - board
void drawTable(float x, float y, float z) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 400.0;
	float height = 20.0;
	float depth = 200.0;

	glm::vec3 brownColor = glm::vec3(0.6, 0.46, 0.32);

	// table
	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y + 200, z + depth / 2.0));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(brownColor));
	glutSolidCube(1.0);

	// table legs
	drawTableLeg(x - width / 2 + 30, y, z + 30);
	drawTableLeg(x + width / 2 - 20, y, z + 30);
	drawTableLeg(x - width / 2 + 30, y, z + depth - 30);
	drawTableLeg(x + width / 2 - 30, y, z + depth - 30);

	// items on table
	// table books
	drawBook(x - 175.0f, y + 210.0f, z + depth / 5.0f + 7.5f, glm::vec3(0.5, 0.0, 0.0));
	drawBook(x - 165.0f, y + 210.0f, z + depth / 5.0f + 0.0f, glm::vec3(0.5, 0.5, 0.0));
	drawBook(x - 155.0f, y + 210.0f, z + depth / 5.0f + 5.0f, glm::vec3(0.0, 0.5, 0.0));
	drawBook(x - 140.0f, y + 210.0f, z + depth / 5.0f + 5.0f, glm::vec3(0.0, 0.0, 0.5));
	drawBook(x - 125.0f, y + 210.0f, z + depth / 5.0f + 0.0f, glm::vec3(0.5, 0.0, 0.5));
	drawBook(x - 115.0f, y + 210.0f, z + depth / 5.0f + 5.0f, glm::vec3(0.0, 0.5, 0.5));

	// table lamp
	drawLamp(x + 150.0f, y + 210.0f, z + depth / 5.0f + 15.0f);

	// table computer
}

// main program functions
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Tell the shader what "choice" the user select
	unsigned int choiceLoc = glGetUniformLocation(program, "choice");
	glUniform1i(choiceLoc, choice);

	// view matrix - glm::lookAt (camera position, direction, Up vector) 
	view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(dirX, dirY, dirZ), glm::vec3(0.0, 1.0, 0.0));
	unsigned int viewLoc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Pass camera position to fragment shader for light calculation
	unsigned int viewPosLoc = glGetUniformLocation(program, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(glm::vec3(camX, camY, camZ)));

	drawGround();
	drawWall(-500, 500, 90.0f);	// left wall
	drawWall(+500, 500, 90.0f);	// right wall
	drawWall(0, 0, 0);			// back wall

	drawCupboard(-500, 0, 600, 90);
	drawCupboard(-500, 0, 200, 90);

	drawBed(500, 0, 500);

	drawTable(0, 0, 0);

	glutSwapBuffers();
}

void speckeyup(int key, int mouseX, int mouseY) {
	switch (key) {
	case GLUT_KEY_ALT_L:
	case GLUT_KEY_ALT_R:
		keys[Key::ALT] = false;
		break;
	case GLUT_KEY_UP:
		keys[Key::UP] = false;
		break;
	case GLUT_KEY_DOWN:
		keys[Key::DOWN] = false;
		break;
	case GLUT_KEY_LEFT:
		keys[Key::LEFT] = false;
		break;
	case GLUT_KEY_RIGHT:
		keys[Key::RIGHT] = false;
		break;
	case GLUT_KEY_PAGE_UP:
		keys[Key::PG_UP] = false;
		break;
	case GLUT_KEY_PAGE_DOWN:
		keys[Key::PG_DN] = false;
		break;
	}
}

void speckey(int key, int mouseX, int mouseY) {
	switch (key) {
	case GLUT_KEY_ALT_L:
	case GLUT_KEY_ALT_R:
		keys[Key::ALT] = true;
		break;
	case GLUT_KEY_UP:
		keys[Key::UP] = true;
		break;
	case GLUT_KEY_DOWN:
		keys[Key::DOWN] = true;
		break;
	case GLUT_KEY_LEFT:
		keys[Key::LEFT] = true;
		break;
	case GLUT_KEY_RIGHT:
		keys[Key::RIGHT] = true;
		break;
	case GLUT_KEY_PAGE_UP:
		keys[Key::PG_UP] = true;
		break;
	case GLUT_KEY_PAGE_DOWN:
		keys[Key::PG_DN] = true;
		break;
	}

	//Change camera position and make sure camera always "look" to the front
	GLfloat limit = 5000.0;
	GLfloat move = 20;

	if (key == GLUT_KEY_LEFT && camX > -limit) {
		dirX -= keys[Key::ALT] ? 2 * move : move;
		camX -= keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_RIGHT && camX < limit) {
		dirX += keys[Key::ALT] ? 2 * move : move;
		camX += keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_UP && camY < limit) {
		dirY += keys[Key::ALT] ? 2 * move : move;
		camY += keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_DOWN && camY > -limit) {
		dirY -= keys[Key::ALT] ? 2 * move : move;
		camY -= keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_PAGE_UP && camZ > -limit) {
		dirZ -= keys[Key::ALT] ? 2 * move : move;
		camZ -= keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_PAGE_DOWN && camZ < limit) {
		dirZ += keys[Key::ALT] ? 2 * move : move;
		camZ += keys[Key::ALT] ? 0 : move;
	}
}

void processMenu(int option) {
	choice = option;
}

void mymenu(void) {
	int sel;
	sel = glutCreateMenu(processMenu);
	glutAddMenuEntry("No Light Effect", 0);
	glutAddMenuEntry("Ambient Light Only", 1);
	glutAddMenuEntry("Diffuse Light Only", 2);
	glutAddMenuEntry("Specular Light Only", 3);
	glutAddMenuEntry("Ambient + Diffuse Lighting", 4);
	glutAddMenuEntry("Ambient + Diffuse + Specular Lighting", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	// set display mode
																//use RGB Color, double buffering
	glutInitWindowSize(600, 600);					// set window size
	glutInitWindowPosition(50, 50);					// set window position on screen
	glutCreateWindow("3D Room");
	glewInit();										// Initialize and load required OpenGL components
	init();

	mymenu();
	glutDisplayFunc(display);						// register redraw function
	glutIdleFunc(display);

	glutSpecialFunc(speckey);
	glutSpecialUpFunc(speckeyup);

	glutMainLoop();									// go into a permenant loop

	return 0;
}