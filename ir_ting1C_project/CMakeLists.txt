cmake_minimum_required(VERSION 3.1)
project(sample
  LANGUAGES CXX
)

set(OpenCV_DIR /usr/local/lib)
find_package(OpenCV 3.4.15 REQUIRED)

set(SRC_LIST
	camera.cpp
	cmd.cpp
	data.cpp
	display.cpp
	sample.cpp	
	temperature.cpp
)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
	${OpenCV_INCLUDE_DIR}
)


link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
link_directories(${OpenCV_LIBRARY_DIRS})

add_executable(sample ${SRC_LIST})
target_link_libraries(sample
    iruvc
    irtemp
    irprocess
    irparse
    pthread

#opencv related
	${OpenCV_LIBRARIES}
)
