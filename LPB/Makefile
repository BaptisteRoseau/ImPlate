###				PARAMETERS

SRC_DIR = src
BLD_DIR = build


CC			= g++
CPPFLAGS	= -std=c++17 -Wall -Wextra -g

ALPR_CFG_PATH     = "/usr/local/share/openalpr/config/openalpr.defaults.con"
ALPR_RUNTIME_PATH = "/usr/local/share/openalpr/runtime_data/"

###				SOURCES
#Ce qui est mis en commentaire est dans le cas où l'installation ne s'est pas faite via sudo

LIBS   		= \
			-lopencv_imgcodecs \
			-lopencv_core \
			-lstdc++fs \
			-lopenalpr
INCLUDE		= -I ${SRC_DIR} \
			  -I /usr/local/include/ \
			  -I /usr/local/include/opencv4

MAIN_FILE   = src/main.cpp
CORNER_FILE = src/main_corner.cpp
SRC_FILES	= $(filter-out $(MAIN_FILE) $(CORNER_FILE), $(wildcard $(SRC_DIR)/*.cpp)) $(wildcard $(SRC_DIR)/gopt/*.cpp) #$(shell find $(SRC_DIR)/ -type f -name '*.cpp')
OBJ_MAIN    = $(MAIN_FILE:%.cpp=%.o)
OBJ_CORNER  = $(CORNER_FILE:%.cpp=%.o)
OBJ_FILES	= $(SRC_FILES:%.cpp=%.o)


# (for not-sudo install only)
OPENCV_DIR   = ../opencv
OPENALPR_DIR = ../openalpr_install
LIBS_NOSUDO  = $(wildcard $(OPENCV_DIR)/lib/*.so*) \
			   $(OPENALPR_DIR)/lib/libopenalpr.so \
			   -lstdc++fs
			   #-L $(OPENCV_DIR)/lib/ \
			   #-L $(OPENALPR_DIR)/lib/
INCLUDE_NOSUDO = -I $(OPENCV_DIR)/include/ \
			  	 -I $(OPENALPR_DIR)/include

BINARY	 	  = blur
BINARY_CORNER = blur_ui
#ARGS 	    = -s -v -r -i ~/Pictures/carpictures -o ~/Pictures/carpictures_blured -l log.txt
ARGS 	    = -s -v -r -i data -o test_dir -l log.txt
#ARGS 	    = -s test_dir -v -i data/tuture.jpg -o ../LPB/test_dir/fiat/fiat.jpg -l log.txt -b 100_150_200_145_250_213_145_200_300_350_400_345_450_413_345_400
#ARGS 	    = -s -v -i data/FiatPanda.jpg -o test_dir -l log.txt -b 100_150_250_213_200_145_145_200_300_350_450_413_345_400_400_345
ARGS_CORNER = -v -r -i ~/Pictures/carpictures -o ~/Pictures/carpictures_blured -l log_ui.txt

#######################################################
###				COMMANDS CATEGORIES
#######################################################

# Accessible commands
.PHONY: all help build run vrun docs clean

# Helper commands
.PHONY: --prebuild

#######################################################
###				DEFAULT MAKE COMMAND
#######################################################

all: build

#######################################################
###				MAKE INSTRUCTIONS / HELP
#######################################################

help:
	@echo -e \
	======================================================================	'\n'\
		'\t' YOUR PROJECT NAME HERE		'\n'\
	======================================================================	'\n'\
	'\n'Available commands:													'\n'\
		'\t' make help		'\t' Show the availables commands				'\n'\
		'\t' make build		'\t' Build the object and binaries				'\n'\
		'\t' make run		'\t' Run the program with default parameters	'\n'\
		'\t' make vrun		'\t' Run with memory check 						'\n'\
		'\t' make doc		'\t' Generate the documentation					'\n'\
		'\t' make clean 	'\t' Clean all generated objects and binairies	'\n'


#######################################################
###				MAKE BUILD
#######################################################

./$(BLD_DIR)/$(BINARY): $(OBJ_FILES) $(OBJ_MAIN)
	@mkdir -p $(BLD_DIR)
	$(CC) $(CPPFLAGS) $(OBJ_MAIN) $(OBJ_FILES) -DDFLT_MANUAL_BLUR_BIN=\"$(shell pwd)/$(BLD_DIR)/$(BINARY_CORNER)\" -o $(BLD_DIR)/$(BINARY) $(LIBS)

./$(BLD_DIR)/$(BINARY_CORNER): $(OBJ_FILES) $(OBJ_CORNER)
	$(CC) $(CPPFLAGS) $(OBJ_CORNER) $(OBJ_FILES) -o $(BLD_DIR)/$(BINARY_CORNER) $(shell pkg-config --libs gtk+-3.0) $(LIBS) $(shell pkg-config --libs gtk+-3.0) -lopencv_objdetect -lopencv_features2d -lopencv_imgproc -lopencv_highgui -lopencv_core

build: ./$(BLD_DIR)/$(BINARY) ./$(BLD_DIR)/$(BINARY_CORNER)

# Some aliases
$(BINARY): ./$(BLD_DIR)/$(BINARY)
$(BINARY_CORNER): ./$(BLD_DIR)/$(BINARY_CORNER)


#######################################################
###				MAKE INSTALL
#######################################################

install: build
	@rm -rf $(OBJ_FILES)
	@rm -rf $(OBJ_MAIN)
	@rm -rf $(OBJ_CORNER)

install_nosudo: $(OBJ_FILES)
	@mkdir -p $(BLD_DIR)
	$(CC) $(CPPFLAGS) $(OBJ_FILES) -o $(BLD_DIR)/$(BINARY) $(LIBS_NOSUDO)
	@rm -f $(OBJ_FILES)
	@rm -rf $(OBJ_MAIN)
	@rm -rf $(OBJ_CORNER)


#######################################################
###				MAKE RUN
#######################################################

run: ./$(BLD_DIR)/$(BINARY)
	./$(BLD_DIR)/$(BINARY) $(ARGS)

runui: ./$(BLD_DIR)/$(BINARY_CORNER)
	./$(BLD_DIR)/$(BINARY_CORNER) $(ARGS_CORNER)

#######################################################
###				MAKE RUN WITH GDB
#######################################################

grun:
	@echo Running program with gdb...
	@gdb -ex run --args ./$(BLD_DIR)/$(BINARY) $(ARGS)

#######################################################
###				MAKE RUN WITH VALGRING
#######################################################

vrun:
	@echo Running program with valgrind...
	@valgrind --leak-check=full --track-origins=yes ./$(BLD_DIR)/$(BINARY) $(ARGS)

#######################################################
###				MAKE DOCUMENTATION
#######################################################

doc:
	doxygen Doxyfile

#######################################################
###				MAKE CLEAN
#######################################################

clean:
	@find . -type f -name '*.o' -delete
	@find . -type f -name '*.so' -delete
	@rm -rf $(BLD_DIR)/*

#######################################################
###				OBJECTS FILES
#######################################################

%.o : %.cpp
	$(CC) -c -DDFLT_CONFIG_FILE=\"$(ALPR_CFG_PATH)\"     \
			 -DDFLT_RUNTIME_DIR=\"$(ALPR_RUNTIME_PATH)\" \
			 $(shell pkg-config --cflags gtk+-3.0) \
			 $(CPPFLAGS) $< -o $@ $(INCLUDE) $(INCLUDE_NOSUDO)
