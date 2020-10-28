#include <gl/glew.h>
#include <gl/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#pragma warning(disable:4996)
using namespace std;

GLuint VAO[5];
GLuint VBO[5];
GLuint program;

GLfloat ground[] = {
	// coordinates			// normal vector
	-500.0, 0.0,    0.0,	0.0, 1.0, 0.0,
	-500.0, 0.0, 1000.0,	0.0, 1.0, 0.0,
	+500.0, 0.0, 1000.0,	0.0, 1.0, 0.0,
	+500.0, 0.0,    0.0,	0.0, 1.0, 0.0
};

GLfloat wallLeft[] = {
	// coordinates			// normal vector
	-500.0,   0.0,    0.0,	1.0, 0.0, 0.0,
	-500.0, 750.0,    0.0,	1.0, 0.0, 0.0,
	-500.0, 750.0, 1000.0,	1.0, 0.0, 0.0,
	-500.0,   0.0, 1000.0,	1.0, 0.0, 0.0
};

GLfloat wallBack[] = {
	// coordinates			// normal vector
	-500.0,   0.0, 0.0,		0.0, 0.0, 1.0,
	-500.0, 750.0, 0.0,		0.0, 0.0, 1.0,
	+500.0, 750.0, 0.0,		0.0, 0.0, 1.0,
	+500.0,   0.0, 0.0,		0.0, 0.0, 1.0
};

GLfloat wallRight[] = {
	// coordinates			// normal vector
	500.0,   0.0,    0.0,	-1.0, 0.0, 0.0,
	500.0, 750.0,    0.0,	-1.0, 0.0, 0.0,
	500.0, 750.0, 1000.0,	-1.0, 0.0, 0.0,
	500.0,   0.0, 1000.0,	-1.0, 0.0, 0.0
};

GLfloat ceiling[] = {
	// coordinates			// normal vector
	-500.0, 750.0,    0.0,	0.0, -1.0, 0.0,
	-500.0, 750.0, 1000.0,	0.0, -1.0, 0.0,
	+500.0, 750.0, 1000.0,	0.0, -1.0, 0.0,
	+500.0, 750.0,    0.0,	0.0, -1.0, 0.0
};

//predefined matrix type from GLM 
glm::mat4 model;	// model matrix
glm::mat4 view;		// view matrix
glm::mat4 proj;		// projection matrix - camera settings

// Camera position
GLfloat camX = 0.0;
GLfloat camY = 500.0;
GLfloat camZ = 2000.0;
// Camera facing direction
GLfloat dirX = 0.0;
GLfloat dirY = 250.0;
GLfloat dirZ = 0.0;

// Point Light Source
glm::vec3 lightPos1 = { -400, 600, 800 };	// Light position in world coordinate
glm::vec3 lightColor1 = { 0.5, 0.3, 0.3 };	// white light
glm::vec3 lightPos2 = { +400, 600, 800 };	// Light position in world coordinate
glm::vec3 lightColor2 = { 0.3, 0.5, 0.3 };	// white light
glm::vec3 ambient = { 0.3, 0.3, 0.3 };	// fixed ambient light

enum Object { NONE, WALL, ITEM };
int object;
int choice = 0;

enum Key { ALT, UP, DOWN, LEFT, RIGHT, PG_UP, PG_DN, KEYS_LENGTH };
bool keys[KEYS_LENGTH] = { false };

bool showMenu = true;

// animation objects's
GLfloat cupboard1Y = 0.0f;
GLfloat cupboard2Y = 0.0f;

GLfloat bedY = 0.0f;
GLfloat pillowX = 0.0f;
GLfloat bedPillar1Y = 0.0f;
GLfloat bedPillar2Y = 0.0f;
GLfloat bedPillar3Y = 0.0f;
GLfloat bedPillar4Y = 0.0f;

GLfloat tableZ = 0.0f;
GLfloat book1Y = 0.0f;
GLfloat book2Y = 0.0f;
GLfloat book3Y = 0.0f;
GLfloat book4Y = 0.0f;
GLfloat book5Y = 0.0f;
GLfloat book6Y = 0.0f;

GLfloat laptopX = 0.0f;
GLfloat lampY = 0.0f;
GLfloat teapotRotate = 0.0f;

// function prototype
GLuint loadShaders(const std::string, const std::string);
void init(void);
void drawCupboard(float, float, float, float);
void drawCupboardDoor(float, float, float, float);
void drawBook(float, float, float, glm::vec3);
void drawLamp(float, float, float);
void drawLaptop(float, float, float);
void drawTableLeg(float, float, float);
void drawTable(float, float, float);
void drawText(int, int, char*);

void display(void);
void animate(int);
void speckeyup(int, int, int);
void speckey(int, int, int);
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
	glGenVertexArrays(5, VAO);
	glGenBuffers(5, VBO);

	// ground
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glutSetVertexAttribCoord3(2);
	glutSetVertexAttribNormal(3);

	// left wall
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallLeft), wallLeft, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glutSetVertexAttribCoord3(2);
	glutSetVertexAttribNormal(3);

	// back wall
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallBack), wallBack, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glutSetVertexAttribCoord3(2);
	glutSetVertexAttribNormal(3);

	// right wall
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallRight), wallRight, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glutSetVertexAttribCoord3(2);
	glutSetVertexAttribNormal(3);

	// ceiling
	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ceiling), ceiling, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glutSetVertexAttribCoord3(2);
	glutSetVertexAttribNormal(3);

	// Activate the shader program
	program = loadShaders("vertexShader.glsl", "fragmentShader.glsl");
	glUseProgram(program);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// fov, aspect, near, far
	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 5000.0f);
	unsigned int projLoc = glGetUniformLocation(program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	// Pass light position vector to fragment shader for light calculation
	unsigned int lightPosLoc1 = glGetUniformLocation(program, "lightPos1");
	glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightPos1));

	// Pass light color to fragment shader for light calculation
	unsigned int lightColorLoc1 = glGetUniformLocation(program, "lightColor1");
	glUniform3fv(lightColorLoc1, 1, glm::value_ptr(lightColor1));

	// Pass light position vector to fragment shader for light calculation
	unsigned int lightPosLoc2 = glGetUniformLocation(program, "lightPos2");
	glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightPos2));

	// Pass light color to fragment shader for light calculation
	unsigned int lightColorLoc2 = glGetUniformLocation(program, "lightColor2");
	glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));

	// Pass ambient light value to fragment shader for light calculation
	unsigned int ambientLoc = glGetUniformLocation(program, "ambient");
	glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
}

// draw room - ground, walls, ceiling
void drawWalls(void) {
	// Set buffer to use to draw the floor
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	object = Object::WALL;

	glUniform1i(objLoc, object);
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
	glDrawArrays(GL_QUADS, 0, 4);

	glUniform1i(objLoc, object);
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glDrawArrays(GL_QUADS, 0, 4);

	glUniform1i(objLoc, object);
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glDrawArrays(GL_QUADS, 0, 4);

	glUniform1i(objLoc, object);
	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glDrawArrays(GL_QUADS, 0, 4);

	glUniform1i(objLoc, object);
	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glDrawArrays(GL_QUADS, 0, 4);
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
	float finalY = y + height / 2.0f;
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
	model = glm::translate(glm::mat4(1.0f), glm::vec3(pillowX + x - width / 2 - pillarRadius - 75, y + height + 15, z - depth / 2.0 + 50));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(whiteColor));
	glutSolidSphere(20, 20, 20);

	// pillow - right ball
	model = glm::translate(glm::mat4(1.0f), glm::vec3(pillowX + x - width / 2 - pillarRadius + 75, y + height + 15, z - depth / 2.0 + 50));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(whiteColor));
	glutSolidSphere(20, 20, 20);

	// pillow - middle cylinder
	model = glm::translate(glm::mat4(1.0f), glm::vec3(pillowX + x - width / 2 - pillarRadius - 75, y + height + 15, z - depth / 2.0 + 50));
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
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width - pillarRadius, y + bedPillar1Y, z - depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(15.0, height * 2.0, 50, 20);

	// bed pillar top right
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - pillarRadius, y + bedPillar2Y, z - depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(15.0, height * 2.0, 50, 20);

	// bed pillar bottom left
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - width - pillarRadius, y + bedPillar3Y, z + depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(15.0, height * 1.25, 50, 20);

	// bed pillar bottom right
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - pillarRadius, y + bedPillar4Y, z + depth / 2.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(lightBrownColor));
	glutSolidCylinder(pillarRadius, height * 1.25, 50, 20);
}

// draw table and items
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

void drawLamp(float x, float y, float z) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	glm::vec3 lampColor = glm::vec3(1, 1, 1);
	glm::vec3 lampStandColor = glm::vec3(0.4, 0.4, 0.4);

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

void drawLaptop(float x, float y, float z) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 100;
	float height = 3;
	float depth = 75;
	glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x + width / 2, y + height / 2.0, z + depth / 2));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(color));
	glutSolidCube(1.0);

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x + width / 2, y + 25, z - depth / 4));
	model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(width, height, depth));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(color));
	glutSolidCube(1.0);

	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x + width / 2, y + 27, z - depth / 4));
	model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(width - 5, height + 2, depth - 15));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
	glutSolidCube(1.0);
}

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

void drawTable(float x, float y, float z) {
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");

	float width = 400.0;
	float height = 20.0;
	float depth = 200.0;

	glm::vec3 brownColor = glm::vec3(0.6, 0.46, 0.32);
	glm::vec3 teapotColor = glm::vec3(1.0, 1.0, 1.0);

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
	drawBook(x - 175.0f, y + 210.0f + book1Y, z + depth / 5.0f + 7.5f, glm::vec3(0.5, 0.0, 0.0));
	drawBook(x - 165.0f, y + 210.0f + book2Y, z + depth / 5.0f + 0.0f, glm::vec3(0.5, 0.5, 0.0));
	drawBook(x - 155.0f, y + 210.0f + book3Y, z + depth / 5.0f + 5.0f, glm::vec3(0.0, 0.5, 0.0));
	drawBook(x - 140.0f, y + 210.0f + book4Y, z + depth / 5.0f + 5.0f, glm::vec3(0.0, 0.0, 0.5));
	drawBook(x - 125.0f, y + 210.0f + book5Y, z + depth / 5.0f + 0.0f, glm::vec3(0.5, 0.0, 0.5));
	drawBook(x - 115.0f, y + 210.0f + book6Y, z + depth / 5.0f + 5.0f, glm::vec3(0.0, 0.5, 0.5));

	// table lamp
	drawLamp(x + 150.0f, y + 210.0f + lampY, z + depth / 5.0f + 15.0f);

	// table computer
	drawLaptop(x + laptopX, y + 210.0f, z + depth / 5.0f + 50.0f);

	// teapot
	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x - 50.0f, y + 220.0f, z + depth / 2.0f + 15.0f));
	model = glm::rotate(model, glm::radians(teapotRotate - 135.0f), glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(teapotColor));
	glutSolidTeapot(20.0f);
}

void drawText(int x, int y, char* string) {
	glRasterPos2d(x, y);
	glColor3f(1.0, 1.0, 1.0);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)string);
}

// main program functions
void display(void) {
	glUseProgram(program);
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

	// draw floor, walls, ceiling
	drawWalls();

	drawCupboard(-499, cupboard1Y, 600, 90);
	drawCupboard(-499, cupboard2Y, 200, 90);

	drawBed(500, bedY, 500);

	drawTable(0, 0, tableZ);

	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 800.0, 0.0, 800.0);
	if (showMenu) {
		drawText(50, 260, (char*)"      Arrow Key : Move camera");
		drawText(50, 240, (char*)"ALT + Arrow Key : Tilt camera");
		drawText(50, 220, (char*)"PG UP / PG DN   : Zoom camera");

		drawText(50, 180, (char*)"Fn + F2         : Animated mode");

		drawText(50, 140, (char*)"   W            : Reset camera");
		drawText(50, 120, (char*)"A  S  D         : Left, both, right lights");
		drawText(50, 100, (char*)"   X            : Exit program");
	}
	drawText(50, 50, (char*)" SPACE          : Toggle Help Menu");
	glUseProgram(program);

	glutSwapBuffers();
}

void animate(int _) {
	time_t seconds;
	seconds = time(NULL);

	choice = choice < 3
		? choice
		: seconds % 2 == 0
		? 4
		: seconds % 2 == 1
		? 5
		: 3;

	if (choice == 3 || choice == 4 || choice == 5) {
		teapotRotate += 15.0f;
		teapotRotate = teapotRotate >= 360.0f ? 0.0f : teapotRotate;

		if (seconds % 2 == 0) {
			cupboard1Y -= 10.0f;
			cupboard2Y += 10.0f;

			bedY += 5.0f;
			pillowX -= 5.0f;
			bedPillar1Y -= 10.0f;
			bedPillar2Y -= 15.0f;
			bedPillar3Y -= 10.0f;
			bedPillar4Y -= 15.0f;

			tableZ += 50.0f;

			book1Y += 5.0f;
			book2Y += 2.0f;
			book3Y += 4.0f;
			book4Y += 1.0f;
			book5Y += 3.0f;
			book6Y += 2.0f;

			laptopX -= 5.0f;
			lampY += 5.0f;
		}
		else if (seconds % 2 == 1) {
			cupboard1Y += 10.0f;
			cupboard2Y -= 10.0f;

			bedY -= 5.0f;
			pillowX += 5.0f;
			bedPillar1Y += 10.0f;
			bedPillar2Y += 15.0f;
			bedPillar3Y += 10.0f;
			bedPillar4Y += 15.0f;

			tableZ -= 50.0f;
			tableZ = tableZ < 0 ? 0 : tableZ;

			book1Y -= 5.0f;
			book2Y -= 2.0f;
			book3Y -= 4.0f;
			book4Y -= 1.0f;
			book5Y -= 3.0f;
			book6Y -= 2.0f;

			laptopX += 5.0f;
			lampY -= 5.0f;
		}
	}
	else {
		cupboard1Y = 5.0f;
		cupboard2Y = 2.0f;

		bedY = 0.0f;
		pillowX = 0.0f;
		bedPillar1Y = 0.0f;
		bedPillar2Y = 0.0f;
		bedPillar3Y = 0.0f;
		bedPillar4Y = 0.0f;

		tableZ = 0.0f;
		book1Y = 0.0f;
		book2Y = 0.0f;
		book3Y = 0.0f;
		book4Y = 0.0f;
		book5Y = 0.0f;
		book6Y = 0.0f;

		laptopX = 0.0f;
		lampY = 0.0f;

		teapotRotate = 0.0f;
	}

	glFlush();
	glutTimerFunc(100, animate, 0);
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
	case GLUT_KEY_F2:
		// disco mode
		choice = choice == 3 || choice == 4 || choice == 5 ? 0 : 3;
		break;

	case GLUT_KEY_ALT_L:
	case GLUT_KEY_ALT_R:
		// toggle tilt with ALT
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
	GLfloat limit = 1000.0;
	GLfloat move = 20;

	if (key == GLUT_KEY_LEFT && dirX > -limit) {
		dirX -= keys[Key::ALT] ? 2 * move : move;
		camX -= keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_RIGHT && dirX < limit) {
		dirX += keys[Key::ALT] ? 2 * move : move;
		camX += keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_UP && dirY < limit) {
		dirY += keys[Key::ALT] ? 2 * move : move;
		camY += keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_DOWN && dirY > -limit) {
		dirY -= keys[Key::ALT] ? 2 * move : move;
		camY -= keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_PAGE_UP && dirZ > -limit) {
		dirZ -= keys[Key::ALT] ? 2 * move : move;
		camZ -= keys[Key::ALT] ? 0 : move;
	}
	if (key == GLUT_KEY_PAGE_DOWN && dirZ < limit) {
		dirZ += keys[Key::ALT] ? 2 * move : move;
		camZ += keys[Key::ALT] ? 0 : move;
	}
}

void myKey(unsigned char key, int mouseX, int mouseY) {
	switch (key) {
	case ' ':
		showMenu = !showMenu;
		break;

	case 'A':
	case 'a':
		choice = 1;
		break;

	case 'S':
	case 's':
		choice = 0;
		break;

	case 'D':
	case 'd':
		choice = 2;
		break;

	case 'W':
	case 'w':
		camX = 0.0;
		camY = 500.0;
		camZ = 2000.0;
		dirX = 0.0;
		dirY = 250.0;
		dirZ = 0.0;
		break;

	case 'X':
	case 'x':
		exit(0);
		break;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	// set display mode
																//use RGB Color, double buffering
	glutInitWindowSize(720, 720);					// set window size
	glutInitWindowPosition(0, 0);					// set window position on screen
	glutCreateWindow("3D Room");
	glewInit();										// Initialize and load required OpenGL components
	init();

	glutDisplayFunc(display);						// register redraw function
	glutIdleFunc(display);
	glutTimerFunc(100, animate, 0);

	glutSpecialFunc(speckey);
	glutSpecialUpFunc(speckeyup);
	glutKeyboardFunc(myKey);

	glutMainLoop();									// go into a permenant loop

	return 0;
}