###				PARAMETERS

SRC_DIR = src
BLD_DIR = build

# (for not-sudo install only)
OPENCV_DIR   = ../opencv
OPENALPR_DIR = ../openalpr_install

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
			#$(wildcard $(OPENCV_DIR)/lib/*.so*) \
			$(OPENALPR_DIR)/lib/libopenalpr.so \
			-L $(OPENALPR_DIR)/lib/ \
			-L $(OPENCV_DIR)/lib/ \
			-L /usr/local/lib/ \
			-lstdc++fs
INCLUDE		= -I ${SRC_DIR} \
			  -I /usr/local/include/ \
			  -I /usr/local/include/opencv4
			  #-I $(OPENCV_DIR)/include/ \
			  -I $(OPENALPR_DIR)/include \

SRC_FILES	= $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/gopt/*.cpp) #$(shell find $(SRC_DIR)/ -type f -name '*.cpp')
OBJ_FILES	= $(SRC_FILES:%.cpp=%.o) 

BINARY	 	= blur
#ARGS 	    = -s -v -r -i ~/Pictures/carpictures -o ~/Pictures/carpictures_blured -l log.txt
ARGS 	    = -s -v -r -i data -o test_dir -l log.txt
#ARGS 	    = -s test_dir -v -i data/tuture.jpg -o ../LPB/test_dir/fiat/fiat.jpg -l log.txt #-b 100_150_200_145_250_213_145_200_300_350_400_345_450_413_345_400
#ARGS 	    = -s -v -i data/FiatPanda.jpg -o test_dir -l log.txt -b 100_150_250_213_200_145_145_200_300_350_450_413_345_400_400_345

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

./$(BLD_DIR)/$(BINARY): $(OBJ_FILES)
	@mkdir -p $(BLD_DIR)
	$(CC) $(CPPFLAGS) $(OBJ_FILES) -o $(BLD_DIR)/$(BINARY) $(LIBS)

build: ./$(BLD_DIR)/$(BINARY)

#######################################################
###				MAKE INSTALL
#######################################################

install: build
	@rm -rf $(OBJ_FILES)

#######################################################
###				MAKE RUN
#######################################################

run: ./$(BLD_DIR)/$(BINARY)
	./$(BLD_DIR)/$(BINARY) $(ARGS)

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
			  $(CPPFLAGS) $< -o $@ $(INCLUDE)