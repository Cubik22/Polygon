#ifndef CONSOLE_H
#define CONSOLE_H

#include <memory>
#include "Window.h"
#include "Renderer.h"
#include "Application.h"
#include "Mesh.h"

#define WIDTH 1200
#define HEIGHT 800


enum class ModeApp{
    Cut,
    Mesh
};

class Console{

public:
    Console();
    ~Console();

    Console(const Console&)                 = delete;
    Console(Console&&) noexcept             = delete;
    Console& operator=(const Console&)      = delete;
    Console& operator=(Console&&) noexcept  = delete;

    void setNumberX(unsigned int _numberX);
    void setNumberY(unsigned int _numberY);

    void setDebugMode(bool what);
    void setFileNameDebug(const std::string& name);

    void start();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    Application app;

    ModeApp mode;

    unsigned int numberX;
    unsigned int numberY;

    bool debug;
    std::string fileNameDebug;

    void terminate();
    void drawFillingDoubleBuffers();
    void drawNoInput();

    void askModeApp();
    void askLoadFromFile();
    void askNumberPolygonMesh();

    void initWindowAndLibraries();
    void drawPolygon();
    void createPolygon();
    void drawSegment();
    void drawCuttedPolygon();

    void askSaveToFile();

    void drawBox();
    void drawGrid();
    void moveAround();

    void createElement(Element& element);
    void createMesh(Element& element);
};

#endif // CONSOLE_H
