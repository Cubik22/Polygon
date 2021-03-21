#ifndef CONSOLE_H
#define CONSOLE_H

#include "Window.h"
#include "Renderer.h"
#include "Application.h"

#define WIDTH 1200
#define HEIGHT 800


class Console{

public:
    Console();
    ~Console();

    void start();

private:
    Window* window;
    Renderer* renderer;
    Application app;

    void drawFillingDoubleBuffers();
    void askLoadFromFile();
    void initWindowAndLibraries();
    void drawPolygon();
    void createPolygon();
    void drawSegment();
    void drawCuttedPolygon();
    void askSaveToFile();
};

#endif // CONSOLE_H
