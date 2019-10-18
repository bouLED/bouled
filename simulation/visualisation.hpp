#ifndef VISIALISATION_H
#define VISIALISATION_H

#include "model.hpp"

#include <vector>
#include <mutex>
#include <atomic>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// TODO: be coherent with names (triangle vs face)
class Visualisation {
	public:
		/*
		 * face_orientations contains integers between 0 and 2
		 * 0: no rotation
		 * 1: 2pi/3 counter clockwise
		 * 2: 4pi/3 counter clockwise
		 */
		Visualisation(const int face_orientations[20]);
		void start();
		void setLedColor(int triangle, int led, GLubyte r, GLubyte g, GLubyte b); // lock before calling
		void setFaceColor(int face, GLubyte r, GLubyte g, GLubyte b); // lock before calling
		void flushColors(); // lock before calling
		void setRotation(glm::quat q);
		void setRotation(float w, float x, float y, float z);
		int getNumberOfLedsPerTriangle();

		std::mutex lock; // not elegant but simple
		int face_orientations[20];

	private:
		std::atomic<bool> hasToFlushColors = {false};
		GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
		void initFaceBuffers();
		void drawIcosahedron();
		void putTriangle();
		void addLedToBufferObjects(int led);

		GLFWwindow* window;
		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

		Model *model;
		std::vector<GLfloat> vboData;
		std::vector<GLushort> iboData;
		std::vector<GLubyte> vboColorData;
		std::vector<glm::mat4> facePositions; // before rotation
		std::vector<glm::mat4>* vboModelData; // after rotation
		std::vector<std::pair<float, float>> ledPositions;
		GLuint vboColor;

		GLuint shaderPid;
		GLuint icosahedronRotationLocation;
		GLuint vboModel;
		std::atomic<bool> icosahedronRotated = {false};

		int windowW = 640, windowH = 480;
};

#endif
