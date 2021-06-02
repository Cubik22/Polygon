# Set minimum CMake version
cmake_minimum_required(VERSION 3.14)

# Set Project Name and language used
project(Polygon LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glew/include)
#link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/glew/src)

#find_package(OpenGL REQUIRED)
#set(OpenGL_GL_PREFERENCE GLVND)
#find_package(GLEW REQUIRED)
#include_directories(${GLEW_INCLUDE_DIRS})
#link_libraries(${GLEW_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/engine)

# Add source code
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Polygon.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Polygon.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Intersector.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Intersector.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Network.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Network.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Console.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Console.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Loader.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Loader.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Element.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Element.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.h)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Segment.cpp)
list(APPEND source_code ${CMAKE_CURRENT_SOURCE_DIR}/src/Segment.h)

# Build executable
add_executable(${PROJECT_NAME} main.cpp ${source_code})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)

# Set Unit Tests
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads engine)

# copy resources files to build directory
add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/res/files
                ${CMAKE_CURRENT_BINARY_DIR}/files)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/res/fonts
                ${CMAKE_CURRENT_BINARY_DIR}/fonts)



