#include <iostream>
#include <cstdio>
#include <string>

#include "cmath"
#include "model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <vector>

using namespace std;

std::vector<glm::mat4> facePositions;

int main(int argc, char **argv) {
	if (argc < 2)
		return -1;
	FILE *file = fopen(argv[1], "r");
	if (file == NULL)
		return -2;
	int w, h;
	if (fscanf(file, "P3\n%d %d\n255\n", &w, &h) != 2) {
		fclose(file);
		return -3;
	}

	bool generateHeader = false;
	if (argc == 3)
		generateHeader = true;

	if (generateHeader) {
		cout << "#ifndef CONTROLLER_CONSTANTS_H\n";
		cout << "#define CONTROLLER_CONSTANTS_H\n\n";
		cout << "#include \"controller_math.h\"\n\n";
		cout << "#include \"controller.h\"\n\n";
		cout << "#define NUMBER_OF_ROWS 12\n";
		cout << "#define imageW " << w << "\n";
		cout << "#define imageH " << h << "\n\n";
		cout << "#ifdef __cplusplus\nextern \"C\" {\n#endif\n";
		cout << "extern color_t image[imageW*imageH];\n";
		cout << "extern const vec2_t led_positions[3];\n";
		cout << "extern const mat4_t face_positions[20];\n";
		cout << "#ifdef __cplusplus\n}\n#endif\n";
		cout << "#endif";
	} else {
		cout << "#include \"controller_math.h\"\n\n";
		cout << "#include \"controller_constants.h\"\n\n";
		cout << "#include \"controller.h\"\n\n";
		cout << "#define imageW " << w << "\n";
		cout << "#define imageH " << h << "\n\n";
		cout << "color_t image[imageW*imageH] = {"; // not const so it goes in the data section
		int r, g, b;
		for (int row = 0; row < h; row++) {
			cout << "\n\t";
			for (int col = 0; col < w; col++) {
				fscanf(file, "%d %d %d", &r, &g, &b);
				cout << "{" << r << ", " << g << ", " << b << "}" << ((col < w-1)?", ":"");
			}
			if (row < h-1)
				cout << ",";
		}
		cout << "\n};";

		fclose(file);

		int face_orientations[20] = {1, 0, 1, 1, 0,
			0, -1, 1, 1, 1,
			-1, 0, 0, -1, 0,
			-1, -1, 0, -1, 0};
		Model *model = new Model(face_orientations, false);
		vector<pair<float, float>> cornerLeds = model->getCornerLeds();
		vector<glm::mat4> facePositions;
		facePositions = model->getFacePositions();

		cout << "const vec2_t led_positions[3] = {";
		for (int i = 0; i < 3; i++)
			cout << "{" << cornerLeds[i].first << ", " << cornerLeds[i].second << "}" << ((i==2)?"};\n":", ");

		cout << "const mat4_t face_positions[20] = {\n";
		for (int face = 0; face < 20; face++) {
			auto facePosition = facePositions.at(face);
			cout << "\t{\n";
			for (int j = 0; j < 4; j++) {
				cout << "\t\t{";
				auto column = glm::column(facePosition, j);
				for (int i = 0; i < 4; i++)
					cout << column[i] << ((i==3)?"}":", ");
				cout << ((j==3)?"\n\t},":",\n");
			}
			cout <<"\t\t" << ((face==19)?"\n};\n":"\n");
		}

	}
}
