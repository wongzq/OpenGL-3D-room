// This example demonstrate Light using Phong illumination model
// Phong illumination model assume 
//         Total Light = ambient light + diffuse light + specular light 

#include <gl/glew.h>
#include <gl/freeglut.h>
// GLM library - for matrix manipulation
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Copy the GLM folder to the "include" folder of Visual C++
#include <fstream>
#include <iostream>
#include <string>

#pragma warning(disable:4996)
using namespace std;

GLuint VBO;		// ID for vertex buffer objects
GLuint VAO;			// ID for vertex array objects
GLuint program;

GLfloat ground[] = {
	//vertices
	//coordinates			normal vector
	-50.0, 0.0, -50.0,		0.0, 1.0, 0.0,
	-50.0, 0.0, 50.0,		0.0, 1.0, 0.0,
	50.0, 0.0, 50.0,		0.0, 1.0, 0.0,
	50.0, 0.0, -50.0,		0.0, 1.0, 0.0
};

//predefined matrix type from GLM 
glm::mat4 model;		//model matrix
glm::mat4 view;			//view matrix
glm::mat4 proj;			//projection matrix -- camera settings

						//Camera position
GLfloat camX = 0.0;
GLfloat camY = 3.0;
GLfloat camZ = 10.0;
//Camera facing direction
GLfloat dirX = 0.0;
GLfloat dirY = 0.0;
GLfloat dirZ = 0.0;

//Point Light Source
glm::vec3 lightPos = { 10.0, 10.0, 5.0 };		//Light position in world coordinate
glm::vec3 lightColor = { 1.0, 1.0, 1.0 };		//white light
glm::vec3 ambient = { 0.33, 0.33, 0.33 };		//fixed ambient light

int choice = 0;
int object;

// function to load shaders
GLuint loadShaders(const string vertexShaderFile, const string fragmentShaderFile)
{
	GLint status;	// for checking compile and linking status

					// load vertex shader code from file
	string vertexShaderCode;	// to store shader code
	ifstream vertexShaderStream(vertexShaderFile, ios::in);	// open file stream

															// check whether file stream was successfully opened
	if (vertexShaderStream.is_open())
	{
		// read from stream line by line and append it to shader code
		string line = "";
		while (getline(vertexShaderStream, line))
			vertexShaderCode += line + "\n";

		vertexShaderStream.close();		// no longer need file stream
	}
	else
	{
		// output error message and exit
		cout << "Failed to open vertex shader file - " << vertexShaderFile << endl;
		exit(EXIT_FAILURE);
	}

	// load fragment shader code from file
	string fragmentShaderCode;	// to store shader code
	ifstream fragmentShaderStream(fragmentShaderFile, ios::in);	// open file stream

																// check whether file stream was successfully opened
	if (fragmentShaderStream.is_open())
	{
		// read from stream line by line and append it to shader code
		string line = "";
		while (getline(fragmentShaderStream, line))
			fragmentShaderCode += line + "\n";

		fragmentShaderStream.close();	// no longer need file stream
	}
	else
	{
		// output error message and exit
		cout << "Failed to open fragment shader file - " << fragmentShaderFile << endl;
		exit(EXIT_FAILURE);
	}

	// create shader objects
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code for shaders
	const GLchar* vShaderCode = vertexShaderCode.c_str();
	const GLchar* fShaderCode = fragmentShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vShaderCode, NULL);
	glShaderSource(fragmentShaderID, 1, &fShaderCode, NULL);

	// compile vertex shader
	glCompileShader(vertexShaderID);

	// check compile status
	status = GL_FALSE;
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		// output error message
		cout << "Failed to compile vertex shader - " << vertexShaderFile << endl;

		// output error information
		int infoLogLength;
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMessage = new char[(long long)(infoLogLength) + 1];
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, errorMessage);
		cout << errorMessage << endl;
		delete[] errorMessage;

		exit(EXIT_FAILURE);
	}

	// compile fragment shader
	glCompileShader(fragmentShaderID);

	// check compile status
	status = GL_FALSE;
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		// output error message
		cout << "Failed to compile fragment shader - " << fragmentShaderFile << endl;

		// output error information
		int infoLogLength;
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMessage = new char[(long long)infoLogLength + 1];
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, errorMessage);
		cout << errorMessage << endl;
		delete[] errorMessage;

		exit(EXIT_FAILURE);
	}

	// create program
	GLuint programID = glCreateProgram();

	// attach shaders to the program object
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	// flag shaders for deletion (will not be deleted until detached from program)
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// link program object
	glLinkProgram(programID);

	// check link status
	status = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		// output error message
		cout << "Failed to link program object." << endl;

		// output error information
		int infoLogLength;
		glGetShaderiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* errorMessage = new char[(long long)infoLogLength + 1];
		glGetShaderInfoLog(programID, infoLogLength, NULL, errorMessage);
		cout << errorMessage << endl;
		delete[] errorMessage;

		exit(EXIT_FAILURE);
	}

	return programID;
}

void init(void)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// for GLUT built-in objects
	glutSetVertexAttribCoord3(2);
	glutSetVertexAttribNormal(3);

	program = loadShaders("vertexShader.glsl", "fragmentShader.glsl");
	// Activate the shader program
	glUseProgram(program);

	glEnable(GL_DEPTH_TEST);			// enable depth buffering
	glClearColor(0.0, 0.0, 0.0, 1.0);	// set background color

	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	// fov, aspect, near, far
	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	unsigned int projLoc = glGetUniformLocation(program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	//Pass light position vector to fragment shader for light calculation
	unsigned int lightPosLoc = glGetUniformLocation(program, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	//Pass light color to fragment shader for light calculation
	unsigned int lightColorLoc = glGetUniformLocation(program, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//Pass ambient light value to fragment shader for light calculation
	unsigned int ambientLoc = glGetUniformLocation(program, "ambient");
	glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
}

void drawHouse(float x, float z)
{
	//float red, green, blue;

	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");
	object = 2;
	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0, z));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.8, 0.6, 0.4)));
	glutSolidCylinder(1.0, 1.5, 50, 50);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 1.5, z));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.5)));
	glutSolidCone(1.2, 1.0, 50, 50);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.375, z + 0.75));
	model = glm::scale(model, glm::vec3(1.0, 1.5, 1.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.8, 0.1, 0.1)));
	glutSolidCube(0.5);

}

void drawTree(float x, float z)
{
	//float red, green, blue;

	unsigned int objLoc = glGetUniformLocation(program, "obj");
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	unsigned int modelLoc = glGetUniformLocation(program, "model");
	object = 2;
	glUniform1i(objLoc, object);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0, z));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.5, 0.25, 0.25)));
	glutSolidCylinder(0.1, 0.5, 50, 50);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 1.0, z));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.2, 0.5, 0.2)));
	glutSolidSphere(0.5, 50, 50);
}

void drawGround()
{
	// Set buffer to use to draw the floor
	unsigned int objLoc = glGetUniformLocation(program, "obj");
	object = 1;
	glUniform1i(objLoc, object);

	// Set drawing color
	unsigned int vColorLoc = glGetUniformLocation(program, "vColor");
	glUniform3fv(vColorLoc, 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_QUADS, 0, 4);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);		// clear screen
	glClear(GL_DEPTH_BUFFER_BIT);		// clear depth buffer

										//Tell the shader what "choice" the user select
	unsigned int choiceLoc = glGetUniformLocation(program, "choice");
	glUniform1i(choiceLoc, choice);

	// view matrix - glm::lookAt (camera position, direction, Up vector) 
	view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(dirX, dirY, dirZ), glm::vec3(0.0, 1.0, 0.0));
	unsigned int viewLoc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//Pass camera position to fragment shader for light calculation
	unsigned int viewPosLoc = glGetUniformLocation(program, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(glm::vec3(camX, camY, camZ)));

	drawHouse(-1.0, 1.0);
	drawHouse(1.0, -5.0);
	drawTree(2.0, 2.0);
	drawTree(3.5, 0.0);
	drawTree(-4.0, -8.0);
	drawGround();
	glutSwapBuffers();
}

void speckey(int theKey, int mouseX, int mouseY)
{
	//Change camera position and make sure camera always "look" to the front
	switch (theKey)
	{
	case GLUT_KEY_LEFT:
		if (camX > -20.0) {
			camX -= (GLfloat)0.1;
			dirX -= (GLfloat)0.1;
		}
		break;

	case GLUT_KEY_RIGHT:
		if (camX < 20.0) {
			camX += (GLfloat)0.1;
			dirX += (GLfloat)0.1;
		}
		break;

	case GLUT_KEY_UP:
		if (camZ > -10.0) {
			camZ -= (GLfloat)0.1;
			dirZ -= (GLfloat)0.1;
		}
		break;

	case GLUT_KEY_DOWN:
		if (camZ < 15.0) {
			camZ += (GLfloat)0.1;
			dirZ += (GLfloat)0.1;
		}
		break;

	case GLUT_KEY_PAGE_UP:
		if (camY < 10.0) {
			camY += (GLfloat)0.1;
			dirY += (GLfloat)0.1;
		}
		break;

	case GLUT_KEY_PAGE_DOWN:
		if (camY > 1.0) {
			camY -= (GLfloat)0.1;
			dirY -= (GLfloat)0.1;
		}
		break;

	default:
		break;
	}

}

void processMenu(int option)
{
	choice = option;
}

void mymenu()
{
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

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	// set display mode
																//use RGB Color, double buffering
	glutInitWindowSize(600, 600);					// set window size
	glutInitWindowPosition(50, 50);					// set window position on screen
	glutCreateWindow("Light");
	glewInit();										// Initialize and load required OpenGL components
	init();
	mymenu();
	glutDisplayFunc(display);						// register redraw function
	glutIdleFunc(display);
	glutSpecialFunc(speckey);
	glutMainLoop();									// go into a permenant loop
	
	return 0;
}