SRC_DIR = src
BLD_DIR = build

CC			= g++
CFLAGS		= 
CPPFLAGS	= -std=c++17 -Wall -Wextra -g -Wno-unused-variable -Wno-unused-parameter

LIBS   		= -L /usr/local/lib/ \
-lopencv_gapi \
-lopencv_photo \
-lopencv_highgui \
-lopencv_imgcodecs \
-lopencv_stitching \
-lopencv_core \
-lopencv_videoio \
-lopencv_dnn \
-lopencv_video \
-lopencv_imgproc \
-lopencv_ml \
-lopencv_features2d \
-lopencv_objdetect \
-lopencv_flann \
-lstdc++fs
INCLUDE		= -I /usr/local/include/opencv4/ \
			  ${SRC_DIR}


ALPR_LIB    = alpr/build/openalpr/libopenalpr.so
SRC_FILES	= $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/gopt/*.cpp) #$(shell find $(SRC_DIR)/ -type f -name '*.cpp')
OBJ_FILES	= $(SRC_FILES:%.cpp=%.o) 

BINARY	 	= blur
TEST_ARGS 	= -s -v -i data -o test_dir -l log.txt

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
		'\t' make run		'\t' Run the program with test parameters		'\n'\
		'\t' make vrun		'\t' Run with memory check 						'\n'\
		'\t' make docs		'\t' Generate the documentation					'\n'\
		'\t' make clean 	'\t' Clean all generated objects and binairies	'\n'


#######################################################
###				MAKE BUILD
#######################################################

--prebuild:
	@mkdir -p $(BLD_DIR)
	#@rm -f $(BLD_DIR)/$(BINARY)
	#@rm -f $(BLD_DIR)/$(OBJ_FILES)

build: --prebuild $(OBJ_FILES) --build_end

--build_end: $(OBJ_FILES)
	$(CC) $(CPPFLAGS) $(OBJ_FILES) -o $(BLD_DIR)/$(BINARY) $(LIBS) $(ALPR_LIB)

#######################################################
###				MAKE INSTALL
#######################################################

install: build
	@mkdir -p $(BLD_DIR)
	@rm -rf $(OBJ_FILES)


#######################################################
###				MAKE RUN
#######################################################

run:
	./$(BLD_DIR)/$(BINARY) $(TEST_ARGS)


#######################################################
###				MAKE RUN WITH GDB
#######################################################

grun:
	@echo Running program with gdb...
	@gdb -ex run --args ./$(BLD_DIR)/$(BINARY) $(TEST_ARGS)

#######################################################
###				MAKE RUN WITH VALGRING
#######################################################

vrun:
	@echo Running program with valgrind...
	@valgrind --leak-check=full --track-origins=yes ./$(BLD_DIR)/$(BINARY) $(TEST_ARGS)

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
	$(CC) -c $(CPPFLAGS) $< -o $@ $(INCLUDE)
