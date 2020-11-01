# CONFIGURATION 
# Server informations
SERV_ROOT=             # The ssh adress
IMG_DIR= # The path to the "img" on the server (used to crop buffer files path)
DIST_SERV_IMG_DIR=             # The remote "img" directory
DIST_SERV_BACKUP_DIR=    # The remote image backup directory 

# Server buffer localization
BUFFER_SERV=
BUFFER_GUI_SERV=

# Local buffer localization (copied from server)
BUFFER=buffer.txt          # The path to the buffer for blur
BUFFER_GUI=buffer_gui.txt  # The path to the buffer for blur_gui

# Input and output directories
INPUT=Input/         # The input file for the blur programm
OUTPUT=Output/         # The output file used for backup, that will be copied on the server with the same name.
INPUT_GUI=Input_gui/ # The input file for the blur_gui programm
OUTPUT_GUI=Output_gui/ # The output file used for backup, that will be copied on the server with the same name.

# Path to blur and blur_gui executables
BLUR= # Path to the blur executable
BLUR_GUI= # Path to the blur_gui executable

# Blur executable output
SUCCESS_FILE=blur_success.txt  # Blur output success files
FAILURE_FILE=blur_failures.txt # Blur output failure files
LOG_FILE=log.txt               # Blur logs
SUCCESS_FILE_GUI=blur_success_gui.txt  # Blur output success files
FAILURE_FILE_GUI=blur_failures_gui.txt # Blur output failure files
LOG_FILE_GUI=log_gui.txt               # Blur logs
