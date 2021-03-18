#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"

class Window{
private:
    GLFWwindow* glWindow;
    double xMouse;
    double yMouse;
    bool mouseLeftPressed;
    bool mouseLeftClick;
public:
    const unsigned int WIDTH;
    const unsigned int HEIGHT;
    Window(const char* name, unsigned int width, unsigned int height);
    ~Window();
    double getXMouse();
    double getYMouse();
    bool shouldClose();
    void swapBuffer();
    void waitEvents();
    void processImput();
    bool isLeftClick() const;
    bool isEnterPressed() const;

    static void initiGLFW();
    static void terminateGLFW();

    friend void mouseCallback(GLFWwindow* glWindow, double xpos, double ypos);
};

#endif // WINDOW_H
