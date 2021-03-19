#include "Window.h"
#include <iostream>

void updatePressedClick(bool glPressed, bool& pressed, bool& click){
    if (glPressed){
        if (!pressed){
            click = true;
        } else{
            click = false;
        }
        pressed = true;
    } else{
        click = false;
        pressed = false;
    }
}

void mouseCallback(GLFWwindow* glWindow, double xpos, double ypos){
    Window* window = (Window*)glfwGetWindowUserPointer(glWindow);
    window->xMouse =  2 * (xpos - window->WIDTH / 2.0) / window->WIDTH;
    window->yMouse = -2 * (ypos - window->HEIGHT / 2.0) / window->HEIGHT;
}

Window::Window(const char* name, unsigned int width, unsigned int height) :
    xMouse{0}, yMouse{0}, mouseLeftPressed{false}, mouseLeftClick{false}, WIDTH{width}, HEIGHT{height} {
    /* Create a windowed mode window and its OpenGL context */
    glWindow = glfwCreateWindow(WIDTH, HEIGHT, name, NULL, NULL);
    if (!glWindow){
        glfwTerminate();
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(glWindow);

    //constant frame rate
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(glWindow, this);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
}

Window::~Window(){
    glfwDestroyWindow(glWindow);
}

double Window::getXMouse(){
    return xMouse;
}

double Window::getYMouse(){
    return yMouse;
}

bool Window::shouldClose(){
    return glfwWindowShouldClose(glWindow);
}

void Window::swapBuffer(){
    glfwSwapBuffers(glWindow);
}

void Window::waitEvents(){
    glfwWaitEvents();
}

void Window::processImput(){
    bool glMouseLeftPressed = glfwGetMouseButton(glWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
    updatePressedClick(glMouseLeftPressed, mouseLeftPressed, mouseLeftClick);
    bool glEnterPressed = glfwGetKey(glWindow, GLFW_KEY_ENTER) == GLFW_PRESS;
    updatePressedClick(glEnterPressed, enterPressed, enterClick);
    bool glBackPressed = glfwGetKey(glWindow, GLFW_KEY_BACKSPACE) == GLFW_PRESS;
    updatePressedClick(glBackPressed, backPressed, backClick);
}

bool Window::isLeftClick() const{
    return mouseLeftClick;
}

bool Window::isEnterClick() const{
    return enterClick;
}

bool Window::isBackClick() const{
    return backClick;
}

void Window::initiGLFW(){
    /* Initialize the library */
    if (!glfwInit()){
        exit(-1);
    }
}

void Window::terminateGLFW(){
    glfwTerminate();
}
