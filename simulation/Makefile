CPPFLAGS+=$(shell pkg-config --cflags gl glfw3 glew)
MYCPPFLAGS=-g -Wall -Wextra -Werror -DSIMULATION
CPPFLAGS+=$(MYCPPFLAGS)
LDLIBS+=$(shell pkg-config --libs gl glfw3 glew) -lpthread -lstdc++ -lm
EXE=main
SRC=main.cpp visualisation.cpp model.cpp
CONTROLLER_DIR=../firmwares/common
OBJ=$(SRC:%.cpp=%.o)
PPM=image.ppm
PPM_SIZE=200
INC=-I$(CONTROLLER_DIR) -I$(CONTROLLER_DIR)/sk9822strip -I./
CPPFLAGS+=$(INC)
CONTROLLER_OBJ=controller_constants.o controller.o controller_math.o sk9822strip.o
OBJ+=$(CONTROLLER_OBJ)
CC=g++
CONVERT=env MAGICK_OCL_DEVICE=OFF convert

#TODO: automatic dependencies

$(EXE): $(OBJ)

%.o: $(CONTROLLER_DIR)/%.c
	$(CC) $(CPPFLAGS) -c $< -o $@

%.o: $(CONTROLLER_DIR)/sk9822strip/%.c
	$(CC) $(CPPFLAGS) -c $< -o $@

controller.o: controller_constants.h

controller_constants.h: gen_controller_constants $(PPM)
	./$< $(PPM) -h > $@

controller_constants.c: gen_controller_constants $(PPM) controller_constants.h
	./$< $(PPM) > $@

gen_controller_constants: model.o gen_controller_constants.o
	$(CC) $(MYCPPFLAGS) $^ -o $@

visualisation.o: visualisation.hpp model.hpp vertexshader fragmentshader

main.o: visualisation.hpp model.hpp $(CONTROLLER_DIR)/controller.h

%.ppm: %.jpg
	$(CONVERT) -resize $(PPM_SIZE) -compress none $^ $@

%.ppm: %.jpeg
	$(CONVERT) -resize $(PPM_SIZE) -compress none $^ $@

%.ppm: %.png
	$(CONVERT) -resize $(PPM_SIZE) -compress none $^ $@

run: $(EXE) $(PPM)
	./$(EXE)

clean:
	rm -f $(EXE) *.o $(PPM) controller_constants.h controller_constants.c gen_controller_constants

