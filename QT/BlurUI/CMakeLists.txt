cmake_minimum_required(VERSION 3.5)

project(BlurUI LANGUAGES CXX)

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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${COMMON_SOURCE_DIR})

set(TS_FILES BlurUI_fr_FR.ts)

if(ANDROID)
  add_library(BlurUI SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    processConfig.cpp
    ${COMMON_SOURCE_DIR}/blur.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt-errors.cpp
    ${COMMON_SOURCE_DIR}/options.cpp
    ${COMMON_SOURCE_DIR}/utils.cpp
    ${TS_FILES}
  )
else()
  add_executable(BlurUI
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    processConfig.cpp
    ${COMMON_SOURCE_DIR}/blur.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt.cpp
    ${COMMON_SOURCE_DIR}/gopt/gopt-errors.cpp
    ${COMMON_SOURCE_DIR}/options.cpp
    ${COMMON_SOURCE_DIR}/utils.cpp
    ${TS_FILES}
  )
endif()

target_link_libraries(BlurUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OpenCV_LIBS} openalpr)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
