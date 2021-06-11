#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"


class Window{

public:
    const unsigned int WIDTH;
    const unsigned int HEIGHT;

    Window(const char* name, unsigned int width, unsigned int height);
    ~Window();

    Window(const Window&)                 = delete;
    Window(Window&&) noexcept             = delete;
    Window& operator=(const Window&)      = delete;
    Window& operator=(Window&&) noexcept  = delete;

    bool shouldClose() const;
    void swapBuffer() const;
    void waitEvents() const;
    void pollEvents() const;
    void processImput();

    double getXMouse() const;
    double getYMouse() const;

    bool isMouseRightClick() const;
    bool isMouseLeftClick() const;

    bool isEnterClick() const;
    bool isBackClick() const;
    bool isEscClick() const;
    bool isSpaceClick() const;

    bool isUpClick() const;
    bool isDownClick() const;
    bool isRightClick() const;
    bool isLeftClick() const;

    bool isBClick() const;
    bool isDClick() const;
    bool isWClick() const;

    static void initiGLFW();
    static void terminateGLFW();

private:
    GLFWwindow* glWindow;

    double xMouse;
    double yMouse;

    bool mouseRightPressed;
    bool mouseRightClick;
    bool mouseLeftPressed;
    bool mouseLeftClick;

    bool enterPressed;
    bool enterClick;
    bool backPressed;
    bool backClick;
    bool escPressed;
    bool escClick;
    bool spacePressed;
    bool spaceClick;

    bool upPressed;
    bool upClick;
    bool downPressed;
    bool downClick;
    bool rightPressed;
    bool rightClick;
    bool leftPressed;
    bool leftClick;

    bool bPressed;
    bool bClick;
    bool dPressed;
    bool dClick;
    bool wPressed;
    bool wClick;

    friend void updatePressedClick(bool glPressed, bool& pressed, bool& click);
    friend void mouseCallback(GLFWwindow* glWindow, double xpos, double ypos);
};

#endif // WINDOW_H
