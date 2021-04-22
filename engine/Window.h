#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"


class Window{

public:
    const unsigned int WIDTH;
    const unsigned int HEIGHT;

    Window(const char* name, unsigned int width, unsigned int height);
    ~Window();

    double getXMouse() const;
    double getYMouse() const;
    bool shouldClose() const;
    void swapBuffer() const;
    void waitEvents() const;
    void pollEvents() const;
    void processImput();
    bool isLeftClick() const;
    bool isEnterClick() const;
    bool isBackClick() const;
    bool isEscClick() const;

    static void initiGLFW();
    static void terminateGLFW();

private:
    GLFWwindow* glWindow;
    double xMouse;
    double yMouse;
    bool mouseLeftPressed;
    bool mouseLeftClick;
    bool enterPressed;
    bool enterClick;
    bool backPressed;
    bool backClick;
    bool escPressed;
    bool escClick;

    friend void updatePressedClick(bool glPressed, bool& pressed, bool& click);
    friend void mouseCallback(GLFWwindow* glWindow, double xpos, double ypos);
};

#endif // WINDOW_H
