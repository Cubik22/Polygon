#ifndef CONSOLE_H
#define CONSOLE_H

#include <memory>
#include "Window.h"
#include "Renderer.h"
#include "Application.h"
#include "Mesh.h"

#define WIDTH 1200
#define HEIGHT 800


class Console{

public:
    Console();
    ~Console();

    void setNumberX(unsigned int _numberX);
    void setNumberY(unsigned int _numberY);

    void setDebugMode(bool what);
    void setFileNameDebug(const char* name);

    void start();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    Application app;

    unsigned int numberX;
    unsigned int numberY;

    bool debug;
    const char* fileNameDebug;

    void terminate();
    void drawFillingDoubleBuffers();
    void drawNoInput();

    void askLoadFromFile();
    void initWindowAndLibraries();
    void drawPolygon();
    void createPolygon();
    void drawSegment();
    void drawCuttedPolygon();
    void askSaveToFile();

    void drawBox();
    void moveAround();

    void createElement(Element& element);
    void createMesh(Element& element);
};

#endif // CONSOLE_H
