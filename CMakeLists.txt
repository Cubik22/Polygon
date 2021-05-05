# Set minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set Project Name and language used
project(Polygon LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
# Add Google Testing Environment
####################################### Start
#add_definitions(-DGTEST_LANGUAGE_CXX11)
#include(GoogleTest)

#if ($ENV{GOOGLETEST_DIR})
#    set(GOOGLETEST_DIR $ENV{GOOGLETEST_DIR})
#else ()
#    if (NOT ${CMAKE_CURRENT_SOURCE_DIR}/googletest STREQUAL "")
#        message(WARNING "Using googletest src dir specified at Qt Creator wizard")
#    endif ()
#    set(GOOGLETEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
#endif ()

#if (EXISTS ${GOOGLETEST_DIR})
#    set(GTestSrc ${GOOGLETEST_DIR}/googletest)
#    set(GMockSrc ${GOOGLETEST_DIR}/googlemock)
#elseif (UNIX AND EXISTS /usr/src/gtest)
#    set(GTestSrc /usr/src/gtest)
#    message(WARNING "Using gtest from system")
#    if (EXISTS /usr/src/gmock)
#        set(GMockSrc /usr/src/gmock)
#    endif ()
#else ()
#    message( FATAL_ERROR "No googletest src dir found - set GOOGLETEST_DIR to enable!")
#endif ()

#set(GTestFiles ${GTestSrc}/src/gtest-all.cc)
#set(GTestIncludes ${GTestSrc} ${GTestSrc}/include)
#if (NOT ${GMockSrc} STREQUAL "")
#    list(APPEND GTestFiles ${GMockSrc}/src/gmock-all.cc)
#    list(APPEND GTestIncludes ${GMockSrc} ${GMockSrc}/include)
#endif ()

#include_directories(${GTestIncludes})
####################################### End

#set(include_eigen ${CMAKE_CURRENT_SOURCE_DIR}/eigen/include)
#include_directories(${include_eigen})

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

# Add Unit Tests code
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
#list(APPEND test_code ${CMAKE_CURRENT_SOURCE_DIR}/test/test_emptyclass.hpp)

# Build executable
add_executable(${PROJECT_NAME} main.cpp ${source_code} ${test_code} ${GTestFiles})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)

# Set Unit Tests
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads engine)
#gtest_discover_tests(${PROJECT_NAME})

# copy resources files to build directory
add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/files
                ${CMAKE_CURRENT_BINARY_DIR}/files)



