cmake_minimum_required(VERSION 3.5)

project(LP_AutoBlur LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED imgcodecs core)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)
find_package(OpenMP)
include_directories(${OpenCV_INCLUDE_DIRS} /usr/local/include src/common src/common/gopt src/blur src/blur_gui)


########################### BLUR

add_executable(blur
  src/blur/main.cpp
  src/common/blur.cpp
  src/common/options.cpp
  src/common/utils.cpp
  src/common/gopt/gopt.cpp
  src/common/gopt/gopt-errors.cpp
)

target_link_libraries(blur PUBLIC ${OpenCV_LIBS} openalpr)
if(OpenMP_CXX_FOUND)
    target_link_libraries(blur PUBLIC OpenMP::OpenMP_CXX)
endif()

############## BLUR_GUI

add_executable(blur_gui
  src/blur_gui/customlabel.cpp
  src/blur_gui/main.cpp
  src/blur_gui/mainwindow.cpp
  src/blur_gui/mainwindow.ui
  src/blur_gui/processConfig.cpp
  src/common/blur.cpp
  src/common/options.cpp
  src/common/utils.cpp
  src/common/gopt/gopt.cpp
  src/common/gopt/gopt-errors.cpp
)

target_compile_options(blur_gui PRIVATE
  -DDFLT_FAILURE_PIC_FILE="blur_failures_gui.txt"
  -DDFLT_SUCCESS_PIC_FILE="blur_success_gui.txt"
)

target_link_libraries(blur_gui PUBLIC
                    Qt${QT_VERSION_MAJOR}::Core
                    Qt${QT_VERSION_MAJOR}::Widgets
                    ${OpenCV_LIBS}
                    openalpr)
if(OpenMP_CXX_FOUND)
    target_link_libraries(blur_gui PUBLIC OpenMP::OpenMP_CXX)
endif()



############## DOXYGEN SUPPORT

option(BUILD_DOC "Build documentation" ON)

find_package(Doxygen)
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( docs ALL
            COMMAND HAVE_DOT=NO ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
    message("Warning: Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif()
