#include "visualisation.hpp"
#include "model.hpp"

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <chrono>
#include <mutex>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

static int getTimeMilliseconds() {
	// C++ nonsense
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

static GLuint shaderPidStatic; // needed by a callback

Visualisation::Visualisation(const int face_orientations[20]) {
	/* Initialize the library */
	if (!glfwInit())
		return;

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(windowW, windowH, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		printf("GLFW\n");
		return;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		printf("GLEW\n");
		return;
	}

	glEnable(GL_DEPTH_TEST);

	shaderPid = LoadShaders("vertexshader", "fragmentshader");
	glUseProgram(shaderPid);

	shaderPidStatic = shaderPid;
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 1.0f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderPid, "projection"), 1, GL_FALSE, &projection[0][0]);

	model = new Model(face_orientations, true);

	initFaceBuffers();
}

void Visualisation::start() {
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	setRotation(1.0f, 0.0f, 0.0f, 0.0f);

	int lastTimeMilliseconds = getTimeMilliseconds();
	int timeMilliseconds = lastTimeMilliseconds;
	int counter = 0;
	bool displayFps = false;

	// Key input
	float angular_speed = (M_PI)/2.0; // rad/s
	float angle_v = 0.0f, angle_h = 0.0f;
	float max_angle_v = 0.8*(M_PI/2.0);
	float cameraDistance = 600.0f;
	glm::vec3 position = cameraDistance * glm::vec3((float) (sin(angle_h) * cos(angle_v)), (float) (sin(angle_v)), (float) (cos(angle_h) * cos(angle_v)));
	glm::mat4 view = glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(shaderPid, "view"), 1, GL_FALSE, &view[0][0]);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		timeMilliseconds = getTimeMilliseconds();

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float h_speed = (angular_speed * (timeMilliseconds - lastTimeMilliseconds) / 1000.0) * ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS));
		float v_speed = (angular_speed * (timeMilliseconds - lastTimeMilliseconds) / 1000.0) * ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS));
		angle_v = std::min(max_angle_v, std::max(-max_angle_v, angle_v + v_speed));
		angle_h = (angle_h + h_speed);
		angle_h -= (2*M_PI) * floor(angle_h / (2.0*M_PI));
	glm::vec3 position = cameraDistance * glm::vec3((float) (sin(angle_h) * cos(angle_v)), (float) (sin(angle_v)), (float) (cos(angle_h) * cos(angle_v)));
		glm::mat4 view = glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(glGetUniformLocation(shaderPid, "view"), 1, GL_FALSE, &view[0][0]);
		

		if (icosahedronRotated) {
			lock.lock();
			glNamedBufferData(vboModel, 21 * sizeof(glm::mat4), &(*vboModelData)[0], GL_DYNAMIC_DRAW);
			icosahedronRotated = false;
			lock.unlock();
		}
		if (hasToFlushColors) {
			lock.lock();
			glNamedBufferData(vboColor, vboColorData.size() * sizeof(GLubyte), &vboColorData[0], GL_DYNAMIC_DRAW);
			hasToFlushColors = false;
			lock.unlock();
		}

		drawIcosahedron();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		// Display fps
		if (displayFps) {
			if (++counter == 100) {
				int elapsedTime = timeMilliseconds - lastTimeMilliseconds;
				int fps = (1000 * counter) / ((double) elapsedTime);
				std::cout << "FPS: " << fps << std::endl;
				counter = 0;
			}
		}
		lastTimeMilliseconds = timeMilliseconds;
	}

	glfwTerminate();
	return;
}

void Visualisation::setLedColor(int face, int led, GLubyte r, GLubyte g, GLubyte b) {
	vboColorData[(1 + model->getNumberOfLedsPerTriangle()) * face * 4 + 4 + led * 4] = r;
	vboColorData[(1 + model->getNumberOfLedsPerTriangle()) * face * 4 + 4 + led * 4 + 1] = g;
	vboColorData[(1 + model->getNumberOfLedsPerTriangle()) * face * 4 + 4 + led * 4 + 2] = b;
}

void Visualisation::setFaceColor(int face, GLubyte r, GLubyte g, GLubyte b) {
	vboColorData[(1 + model->getNumberOfLedsPerTriangle()) * face * 4] = r;
	vboColorData[(1 + model->getNumberOfLedsPerTriangle()) * face * 4 + 1] = g;
	vboColorData[(1 + model->getNumberOfLedsPerTriangle()) * face * 4 + 2] = b;
}

int Visualisation::getNumberOfLedsPerTriangle() {
	return model->getNumberOfLedsPerTriangle();
}

void Visualisation::flushColors() {
	hasToFlushColors = true;
}

void Visualisation::putTriangle() {
	float side = model->getSide();
	float height = model->getHeight();
	GLfloat triangleVertices[] {
		-side/2, 0.0f, 0.0f,
			side/2, 0.0f, 0.0f,
			0.0f, height, 0.0f
	};
	GLushort triangleIndices[] = {0, 1, 2};
	int indexOffset = vboData.size()/3;
	for (int i = 0; i < 3; i++)
		triangleIndices[i] += indexOffset;
	vboData.insert(vboData.end(), triangleVertices, triangleVertices + 9);
	iboData.insert(iboData.end(), triangleIndices, triangleIndices + 3);
}

void Visualisation::setRotation(glm::quat q) {
	lock.lock();
	glm::mat4 icosahedronRotation = glm::mat4_cast(q);
	for (int i = 0; i < 20; i++)
		(*vboModelData)[i] = icosahedronRotation * facePositions[i];
	icosahedronRotated = true;
	lock.unlock();
}

void Visualisation::setRotation(float w, float x, float y, float z) {
	setRotation(glm::quat(w, x, y, z));
}

void Visualisation::addLedToBufferObjects(int led) {
	float x = ledPositions[led].first;
	float y = ledPositions[led].second;
	float ledW = model->getLedW();
	float ledH = model->getLedH();
	float z_offset = ledW/10.0f; // Put leds above the surface
	x -= ledW/2.0;
	y += ledH/2.0;
	GLfloat squareVertices[] {
		x, y, z_offset,
			x, y - ledH, z_offset,
			x + ledW, y - ledH, z_offset,
			x + ledW, y, z_offset
	};
	GLushort squareIndices[] = {
		0, 1, 3,
		1, 2, 3
	};
	int indexOffset = vboData.size()/3;
	for (int i = 0; i < 6; i++)
		squareIndices[i] += indexOffset;
	vboData.insert(vboData.end(), squareVertices, squareVertices + 12);
	iboData.insert(iboData.end(), squareIndices, squareIndices + 6);
}

void Visualisation::initFaceBuffers() {
	const int maxNumberOfLeds = 95;
	vboData.reserve(9 + 12*maxNumberOfLeds);
	iboData.reserve(3 + 6*maxNumberOfLeds);
	putTriangle();
	model->putLeds(ledPositions);
	for (int i = 0; i < model->getNumberOfLedsPerTriangle(); i++)
		addLedToBufferObjects(i);

	// Vertices and indices
	GLuint vbo;
	GLuint ibo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(GLfloat), &vboData[0], GL_STATIC_DRAW);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboData.size() * sizeof(GLushort), &iboData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	// Colors
	glUniform1i(glGetUniformLocation(shaderPid, "numberOfLedsPerTriangle"), getNumberOfLedsPerTriangle());
	vboColorData.insert(vboColorData.begin(), (1 + getNumberOfLedsPerTriangle()) * 21 * 4, (GLubyte) 255); // * 4 because RGBA
	for (int i = 0; i < 21; i++) {
		vboColorData[(1 + getNumberOfLedsPerTriangle()) * i * 4] = (GLubyte) 128;
		vboColorData[(1 + getNumberOfLedsPerTriangle()) * i * 4 + 1] = (GLubyte) 128;
		vboColorData[(1 + getNumberOfLedsPerTriangle()) * i * 4 + 2] = (GLubyte) 128;
	}
	vboColorData[(1 + getNumberOfLedsPerTriangle()) * 20 * 4 + 4] = (GLubyte) 0;

	GLuint texture;
	glGenBuffers(1, &vboColor);
	glBindBuffer(GL_TEXTURE_BUFFER, vboColor);
	glBufferData(GL_TEXTURE_BUFFER, vboColorData.size() * sizeof(GLubyte), &vboColorData[0], GL_DYNAMIC_DRAW);
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA8, vboColor);
	glUniform1i(glGetAttribLocation(shaderPid, "texture_buffer"), 0);
	glEnableVertexAttribArray(0);

	facePositions = model->getFacePositions();

	vboModelData = new std::vector<glm::mat4>(facePositions);
	
	// Instancing with MVP
	GLuint mvpId = 1;
	glGenBuffers(1, &vboModel);
	glBindBuffer(GL_ARRAY_BUFFER, vboModel);
	glNamedBufferData(vboModel, 21 * sizeof(glm::mat4), &(*vboModelData)[0], GL_DYNAMIC_DRAW);
	for (int i = 0; i < 4; i++) {
		GLuint mvpColumn = mvpId + i;
		glEnableVertexAttribArray(mvpColumn);
		glVertexAttribPointer(mvpColumn, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *) (sizeof(glm::vec4) * i));
		glVertexAttribDivisor(mvpColumn, 1);
	}
}

void Visualisation::drawIcosahedron() {
	glDrawElementsInstanced(GL_TRIANGLES, iboData.size(), GL_UNSIGNED_SHORT, 0, 21);
}

GLuint Visualisation::LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void Visualisation::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	(void) window;
	glViewport(0, 0, width, height);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 1.0f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderPidStatic, "projection"), 1, GL_FALSE, &projection[0][0]);
}
