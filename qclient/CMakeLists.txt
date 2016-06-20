cmake_minimum_required(VERSION 2.8.11)

project(qsegnetclienet)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(-std=c++11)
add_definitions(-DQT_NO_DEBUG_OUTPUT)

set(QtLibs Qt5Widgets Qt5Network)

foreach(Lib ${QtLibs})
    find_package(${Lib} REQUIRED)
    set(LIBRARIES ${${Lib}_LIBRARIES} ${LIBRARIES})
    set(INCLUDE_DIRS ${${Lib}_INCLUDE_DIRS} ${INCLUDE_DIRS})
    add_definitions(${${Lib}_DEFINITIONS})
endforeach()

find_package(OpenCV REQUIRED)
set(LIBRARIES ${OpenCV_LIBRARIES} ${LIBRARIES})
set(INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIRS})

set(INCLUDE
        include/MainWindow.h
        include/ImageLoader.h
        include/SegInfo.h
        include/SettingDialog.h
        include/GCModule.h
        include/ImageViewer.h)

set(SOURCES
        main.cpp
        src/MainWindow.cpp
        src/ImageLoader.cpp
        src/SegInfo.cpp
        src/SettingDialog.cpp
        src/GCModule.cpp
        src/ImageViewer.cpp)

qt5_add_resources(resources resources/resource.qrc)

add_executable(QSegNetClienet ${INCLUDE} ${SOURCES} ${resources})

target_link_libraries(QSegNetClienet ${LIBRARIES})
target_include_directories(QSegNetClienet PUBLIC include ${INCLUDE_DIRS})
