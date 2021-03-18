cmake_minimum_required(VERSION 3.10)

# Set Project Name and language used
project(engine LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

add_library(
    engine
    Vector2f.cpp
    IndexBuffer.cpp
    VertexBuffer.cpp
    VertexArray.cpp
    Shader.cpp
    Shape.cpp
    Renderer.cpp
    Window.cpp
    Vector2f.h
    IndexBuffer.h
    VertexBuffer.h
    VertexArray.h
    Shader.h
    Shape.h
    Renderer.h
    Window.h
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glew)

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads libglew_static glfw)
