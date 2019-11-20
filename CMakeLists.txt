cmake_minimum_required(VERSION 2.8.11)

project(simpleMerge)

find_package(COLMAP REQUIRED )
# or to require a specific version: find_package(COLMAP 3.4 REQUIRED)

find_package(OpenCV REQUIRED PATHS /usr/local/include/opencv )
find_package(ZLIB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

include_directories(${COLMAP_INCLUDE_DIRS})
link_directories(${COLMAP_LINK_DIRS})





file(GLOB_RECURSE simpleMerge_SRC "src/*.cpp" "src/*.h" "src/*.inc" )
add_executable(simpleMerge ${simpleMerge_SRC})
target_link_libraries(simpleMerge ${COLMAP_LIBRARIES} ${OpenCV_LIBS} ${ZLIB_LIBRARIES})

IF(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
ENDIF(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
