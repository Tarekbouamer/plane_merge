cmake_minimum_required(VERSION 3.0)

project(SceneParser)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#find_package(COLMAP REQUIRED )
find_package(Eigen3     REQUIRED )
find_package(OpenCV     REQUIRED )
find_package(Glog       REQUIRED)
find_package(ZLIB)
find_package(Boost REQUIRED COMPONENTS
             program_options
             filesystem
             graph
             regex
             system
             unit_test_framework)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")

#include_directories(${COLMAP_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${GLog_INCLUDE_DIRS})

#link_directories(${COLMAP_LINK_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${OpenCV_LIB_DIR})
link_directories(${EIGEN3_INCLUDE_DIR})

# set(INTERNAL_LIBRARIES
#     graclus
# )

include_directories(
    lib
)


file(GLOB_RECURSE SceneParser_LIBS "src/*.cpp" "src/*.cc" "src/*.c" "src/*.h" "src/*.inc" )

add_executable(SceneParser ${SceneParser_LIBS})

target_link_libraries(SceneParser 
    ${CMAKE_DL_LIBS}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${OpenCV_LIBS} 
    ${ZLIB_LIBRARIES}
    ${GLOG_LIBRARIES} )

IF(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
ENDIF(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
