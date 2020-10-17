cmake_minimum_required(VERSION 3.5)

project(blur_gui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_SOURCE_DIR ../../program/src/)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenMP)
include_directories(${OpenCV_INCLUDE_DIRS} /usr/local/include ${COMMON_SOURCE_DIR})
message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")

if(ANDROID)
  add_library(blur_gui SHARED
    customlabel.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    processConfig.cpp
    ${COMMON_SOURCE_DIR}/blur.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt-errors.cpp
    ${COMMON_SOURCE_DIR}/options.cpp
    ${COMMON_SOURCE_DIR}/utils.cpp
  )
else()
  add_executable(blur_gui
    customlabel.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    processConfig.cpp
    ${COMMON_SOURCE_DIR}/blur.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt-errors.cpp
    ${COMMON_SOURCE_DIR}/options.cpp
    ${COMMON_SOURCE_DIR}/utils.cpp
  )
endif()

target_link_libraries(blur_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OpenCV_LIBS} openalpr)
if(OpenMP_CXX_FOUND)
    target_link_libraries(blur_gui PUBLIC OpenMP::OpenMP_CXX)
endif()
