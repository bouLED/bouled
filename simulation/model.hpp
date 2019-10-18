#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

class Model {
	public:
		Model(const int face_orientations[20], bool referencePlane);
		std::vector<std::pair<float, float>> getCornerLeds();
		std::vector<glm::mat4> getFacePositions();
		int getNumberOfLedsPerTriangle();
		void putLeds(std::vector<std::pair<float, float>> &ledPositions);
		float getSide();
		float getHeight();
		float getLedW();
		float getLedH();

	private:
		std::vector<glm::mat4> facePositions;
		/* Constants for placing the LEDs */
		const float side = 179.0f;
		const float height = (sqrt(3.0)*side)/2.0;
		const float ledW = 5.0;
		const float ledH = ledW;
		const float thickness = 0.01;
		const float eps = ledH / (2.0 * tan(M_PI/3.0));
		const float clearanceY = 6.0;
		const float clearanceX = clearanceY * (1.0/sin(M_PI/3.0) + 1.0/tan(M_PI/3.0));
		const int nbLines = 12;
		const float side2 = side - 2.0*clearanceX;
		const float height2 = side2*tan(M_PI/3.0)/2.0;
		const int numberOfLedsPerTriangle = 78;
		int ledCounts(int line);
		glm::mat4 orientTriangle(int face, const int face_orientations[20]);
		int putRow(std::vector<std::pair<float, float>> &ledPositions, float startPosX, float startPosY, int nb, float avail);
		void computeFacePositions(const int face_orientations[20], bool referencePlane);

};

#endif
