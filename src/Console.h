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

    void setDebugStatus(bool what);
    void setDebugMode(const ModeApp& mode);
    void setFileNameDebug(const std::string& name);

    void start();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    Application app;

    ModeApp mode;

    unsigned int numberX;
    unsigned int numberY;

    // debug variables

    // this set the status to tebug
    bool debug;
    ModeApp debugMode;
    std::string fileNameDebug;

    // drawDebug draw indices on the screen
    bool drawDebug;
    float textScale;

    void terminate();
    void drawFillingDoubleBuffers();
    bool processWindow();
    void drawNoInput();
    void drawNoInput(const std::vector<Vector2f>& vertices,
                     const std::vector<std::shared_ptr<std::vector<unsigned int>>>& indices);

    void drawNoInput(const std::vector<std::shared_ptr<std::vector<Vector2f>>>& vertices,
                     const std::vector<std::shared_ptr<std::vector<unsigned int>>>& indices);

    void drawNoInput(const std::vector<Vector2f>& vertices,
                     const IndicesElement& indicesElement);

    void drawNoInput(const std::vector<std::shared_ptr<std::vector<Vector2f>>>& vertices,
                     const std::vector<IndicesElement>& indicesElement);

    void drawIndices(const std::vector<Vector2f>& vertices,
                     const std::vector<std::shared_ptr<std::vector<unsigned int>>>& indices);

    void askModeApp();
    void askLoadFromFile();
    void askNumberPolygonMesh();

    void initWindowAndLibraries();
    void drawPolygon();
    void createPolygon();
    void drawSegment();
    void drawCuttedPolygon();

    void askSaveToFile(const Element& element);
    void savePolygon();
    void saveElement(const Element& element);

    void drawBox();
    void drawGrid();
    void moveAround();

    void createElement(Element& element);
    void createMesh(Element& element);
};

#endif // CONSOLE_H
