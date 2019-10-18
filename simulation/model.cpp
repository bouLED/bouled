#include "model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

Model::Model(const int face_orientations[20], bool referencePlane) {
	computeFacePositions(face_orientations, referencePlane);
}

void Model::computeFacePositions(const int face_orientations[20], bool referencePlane) {
	// Positionning the faces
	double phi = (1.0f + sqrt(5.0f))/2.0f;
	double phi2 = phi*phi;
	double angle = atan(phi2);

	// Middle down
	glm::mat4 centerTriangle = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -height, 0.0f)); // center the triangle
	glm::mat4 rotateTriangle = glm::rotate(glm::mat4(1.0f), (float) (angle - M_PI/2.0), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate it
	glm::mat4 placeTriangle = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, ((float) (phi*side)/2.0f), side/2.0f));
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), (float) ((2.0*M_PI)/5.0), glm::vec3(0.0f, (phi*side)/2.0f, -side/2.0f));
	glm::mat4 faceRotation = glm::mat4(1.0f);
	for (int i = 0; i < 5; i++) {
		facePositions.push_back(faceRotation * placeTriangle * rotateTriangle * centerTriangle * orientTriangle(facePositions.size(), face_orientations));
		faceRotation *= rotation;
	}

	// Bottom
	glm::mat4 upsideDown = glm::rotate(glm::mat4(1.0f), (float) M_PI, glm::vec3(0.0f, 0.0f, 1.0f));
	rotateTriangle = glm::rotate(glm::mat4(1.0f), (float) (angle - M_PI/2.0), glm::vec3(1.0f, 0.0f, 0.0f));
	placeTriangle = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, (float) (phi*side/2.0f)));
	rotation = glm::rotate(glm::mat4(1.0f), (float) ((2.0*M_PI)/5.0), glm::vec3(0.0f, (phi*side)/2.0f, -side/2.0));
	faceRotation = glm::mat4(1.0f);
	for (int i = 0; i < 5; i++) {
		facePositions.push_back(faceRotation * placeTriangle * upsideDown * rotateTriangle * orientTriangle(facePositions.size(), face_orientations));
		faceRotation *= rotation;
	}

	// Top
	centerTriangle = glm::rotate(glm::mat4(1.0f), (float) (M_PI / 2.0), glm::vec3(0.0f, -1.0f, 0.0f));
	rotateTriangle = glm::rotate(glm::mat4(1.0f), (float) (M_PI - angle), glm::vec3(0.0f, 0.0f, -1.0f));
	placeTriangle = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float) (side*phi)/2.0f, 0.0f));
	rotation = glm::rotate(glm::mat4(1.0f), (float) ((2.0*M_PI)/5.0), glm::vec3(0.0f, (phi*side)/2.0f, -side/2.0f));
	faceRotation = glm::mat4(1.0f);
	for (int i = 0; i < 5; i++) {
		facePositions.push_back(faceRotation * placeTriangle * rotateTriangle * centerTriangle * orientTriangle(facePositions.size(), face_orientations));
		faceRotation *= rotation;
	}

	// Middle up
	centerTriangle = glm::rotate(glm::mat4(1.0f), (float) (-M_PI / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	rotateTriangle = glm::rotate(glm::mat4(1.0f), (float) (M_PI - angle), glm::vec3(0.0f, 1.0f, 0.0f));
	placeTriangle = glm::translate(glm::mat4(1.0f), glm::vec3((float) (side*phi)/2.0f, 0.0f, 0.0f));
	rotation = glm::rotate(glm::mat4(1.0f), (float) ((2.0*M_PI)/5.0), glm::vec3(0.0f, (phi*side)/2.0f, -side/2.0f));
	faceRotation = glm::mat4(1.0f);
	for (int i = 0; i < 5; i++) {
		facePositions.push_back(faceRotation * placeTriangle * rotateTriangle * centerTriangle * orientTriangle(facePositions.size(), face_orientations));
		faceRotation *= rotation;
	}

	// Rotate the icosahedron to have a tip at the top
	glm::mat4 tipTop = glm::rotate(glm::mat4(1.0f), (float) atan(phi - 1), glm::vec3(1.0f, 0.0f, 0.0f));
	for (int i = 0; i < 20; i++)
		facePositions[i] = tipTop * facePositions[i];

	if (referencePlane) {
		centerTriangle = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -height/2.0f, 0.0f));
		rotateTriangle = glm::rotate(glm::mat4(1.0f), (float) (M_PI/2.0), glm::vec3(-1.0f, 0.0f, 0.0f));
		placeTriangle = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -side, 0.0f));
		facePositions.push_back(placeTriangle * rotateTriangle * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)) * centerTriangle);
	}
}

glm::mat4 Model::orientTriangle(int face, const int face_orientations[20]) {
	int f = face_orientations[face];

	if (10 <= face && face <= 14)
		f = (f+1) % 3;
	else if (15 <= face && face <= 20)
		f = (f+2) % 3;

	// Center the triangle, rotate it and put it back in place
	glm::mat4 center = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -(side * tan(M_PI/6))/2.0f, 0.0f));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), (float) ((f * 2.0 * M_PI) / 3.0), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 back = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (side * tan(M_PI/6))/2.0f, 0.0f));
	return back * rotate * center;
}

std::vector<glm::mat4> Model::getFacePositions() {
	return facePositions;
}

std::vector<std::pair<float, float>> Model::getCornerLeds() {
	std::vector<std::pair<float, float>> cornerLeds;
	std::vector<std::pair<float, float>> ledPositions;
	putLeds(ledPositions);
	cornerLeds.push_back(ledPositions.at(0));
	cornerLeds.push_back(ledPositions.at(nbLines-1));
	cornerLeds.push_back(ledPositions.at(numberOfLedsPerTriangle-1));
	return cornerLeds;
}

int Model::ledCounts(int line) {
		return nbLines-line;
}

void Model::putLeds(std::vector<std::pair<float, float>> &ledPositions) {
	int count = 0;
	for (int i = 0; i < nbLines; i++)
		count += ledCounts(i);
	if (count != numberOfLedsPerTriangle)
		std::cerr << "Wrong number of leds" << std::endl;
	float startY = clearanceY;
	float availY = height2;
	float interledY = (availY - (nbLines-1)*ledH)/(nbLines - 1);
	float ySpacing = ledH + interledY;
	for (int i = 0; i < nbLines; i++) {
		float y = i*ySpacing;
		float L = side2*(1.0-y/height2);
		float availX = L;
		int nb = ledCounts(i);
		putRow(ledPositions, -availX/2.0, startY + i*ySpacing, nb, availX);
	}
}

int Model::putRow(std::vector<std::pair<float, float>> &ledPositions, float startPosX, float startPosY, int nb, float avail) {
	if (nb == 1) {
		ledPositions.push_back(std::make_pair(0, startPosY));
	} else {
		float spacing = ledW + (avail-(nb-1)*ledW) / (nb-1);
		for (int i = 0; i < nb; i++)
			ledPositions.push_back(std::make_pair(startPosX + i*spacing, startPosY));
	}
	return 0;
}

int Model::getNumberOfLedsPerTriangle() {
	return numberOfLedsPerTriangle;
}

float Model::getSide() {
	return side;
}

float Model::getHeight() {
	return height;
}

float Model::getLedW() {
	return ledW;
}

float Model::getLedH() {
	return ledH;
}

