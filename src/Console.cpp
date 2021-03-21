#include "Console.h"
#include <filesystem>

Console::Console() : window{nullptr}, renderer{nullptr} {}

Console::~Console() {
    // already terminated before
    //terminate();
}

void Console::start(){
    askLoadFromFile();
    initWindowAndLibraries();
    if (!app.isVerticesIndicesLoaded()){
        drawPolygon();
    }
    createPolygon();
    if (!app.isSegmentLoaded()){
        drawSegment();
    }
    drawCuttedPolygon();
    askSaveToFile();
}

void Console::terminate(){
    if (renderer != nullptr){
        delete renderer;
    }
    if (window != nullptr){
        delete window;
    }
    Window::terminateGLFW();
}

void Console::drawFillingDoubleBuffers(){
    renderer->clear();
    renderer->drawShapes();
    window->swapBuffer();
    renderer->clear();
    renderer->drawShapes();
}

void Console::askLoadFromFile(){
    std::string consoleString;

    bool continueLoop = true;
    while (continueLoop){
        continueLoop = false;
        std::cout << "Do you want to load polygon from file? [y/N] ";
        std::getline(std::cin, consoleString);
        if (consoleString == "y" || consoleString == "Y"){
            std::cout << "Enter the name of the file you what to load polygon from: ";
            std::string fileName;
            std::getline(std::cin, fileName);
            int numberVertices = app.getNumberVerticesFromFile(fileName);
            if (numberVertices <= 0){
                continueLoop = true;
                continue;
            }
            if (app.searchInFile(fileName, "vertices") <= 0){
                continueLoop = true;
                continue;
            } else{
                std::cout << "Vertices found\n";
            }
            if (app.searchInFile(fileName, "indices", false) > 0){
                std::cout << "Indices found, do you want to load them? (If not it is assumed that vertices are in order) [Y/n] ";
                std::getline(std::cin, consoleString);
                if (consoleString == "n" || consoleString == "N"){
                    if (app.loadVerticesFromFile(fileName, numberVertices, false) < 0){
                        continueLoop = true;
                        continue;
                    }
                } else{
                    if (app.loadVerticesFromFile(fileName, numberVertices, true) < 0){
                        continueLoop = true;
                        continue;
                    }
                }
            } else{
                if (app.loadVerticesFromFile(fileName, numberVertices, false) < 0){
                    continueLoop = true;
                    continue;
                }
            }
            if (app.searchInFile(fileName, "segment", false) > 0){
                std::cout << "Segment found, do you want to load it? [y/N] ";
                std::getline(std::cin, consoleString);
                if (consoleString == "y" || consoleString == "Y"){
                    if (app.loadSegmentFromFile(fileName) < 0){
                        std::cout << "Segment not loaded\n";
                    }
                }
            }
        }
    }
    std::cout << std::endl;
}

void Console::initWindowAndLibraries(){
    Window::initiGLFW();

    window = new Window("Polygon", WIDTH, HEIGHT);

    Renderer::initGLEW();

    renderer = new Renderer();
    renderer->setPolygonColorFloat(WHITE);

    std::cout << std::endl;
}

void Console::drawPolygon(){
    bool end = false;
    while (!end){
        if (window->shouldClose()){
            terminate();
            exit(-1);
        }
        renderer->clear();
        window->processImput();
        if (window->isBackClick() || window->isEscClick()){
            app.removeLastVertex();
        }
        bool added = app.addVertex({(float)window->getXMouse(), (float)window->getYMouse()});
        renderer->replaceShape(0, new Lines(app.getVertices(), app.getIndices()));

        renderer->drawShapes();

        if (!window->isLeftClick() && added){
            app.removeLastVertex();
        }

        if (window->isEnterClick()){
            if (app.closePolygon()){
                end = true;
            } else{
                std::cout << "Cannot close polygon\n";
            }
        }

        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::createPolygon(){
    app.createMainPolygon();
    const Polygon& polygon = app.getPolygon();
    renderer->replaceShape(0, new LinesPointIndices(polygon.getPoints(), polygon.getIndices()));
    drawFillingDoubleBuffers();
}

void Console::drawSegment(){
    bool end = false;
    while (!end){
        if (window->shouldClose()){
            terminate();
            exit(-1);
        }
        renderer->clear();
        window->processImput();

        if (window->isBackClick() || window->isEscClick()){
            app.removeSegmentPoint();
        }

        app.addSegmentPoint({(float)window->getXMouse(), (float)window->getYMouse()});
        if (app.getSegmentSize() == 1){
            renderer->replaceShape(1, new Lines(app.getSegmentPoints(), {}));
        } else if (app.getSegmentSize() == 2){
            renderer->replaceShape(1, new Lines(app.getSegmentPoints(), {0, 1}));
        }

        renderer->drawShapes();

        if (!window->isLeftClick()){
            app.removeSegmentPoint();
        }

        if (app.getSegmentSize() == 2){
            end = true;
        }

        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::drawCuttedPolygon(){
    renderer->removeLastShape();
    renderer->removeLastShape();
    app.cutMainPolygon();
    const std::vector<std::vector<unsigned int>*>& polygonsIndices = app.getPolygonsIndices();

    std::cout << std::endl;
    std::cout << "Number of polygons: " << polygonsIndices.size() << "\n";
    for (unsigned int i = 0; i < polygonsIndices.size(); i++){
        const std::vector<unsigned int>& indices = *polygonsIndices[i];
        for (unsigned int n = 0; n < indices.size(); n++){
            std::cout << indices[n] << " ";
        }
        std::cout <<  "\n";
        Shape* shapeNuova = new LinesPointIndices(app.getPolygon().getPoints(), indices);
        renderer->addShape(shapeNuova);
    }

    drawFillingDoubleBuffers();

    while (!window->shouldClose()){
        window->processImput();
        if (window->isEnterClick() || window->isBackClick() || window->isEscClick()){
            break;
        }
        window->waitEvents();
    }
    std::cout << std::endl;

    terminate();
}

void Console::askSaveToFile(){
    std::cout << "Do you want to save polygon to file? [y/N] ";
    std::string consoleString;
    std::string fileName;
    std::string answer;
    std::getline(std::cin, consoleString);
    if (consoleString == "y" || consoleString == "Y"){
        bool repeat = true;
        while (repeat){
            repeat = false;
            std::cout << "Enter the name of the file you want to save polygon to: ";
            std::getline(std::cin, fileName);
            if (std::filesystem::exists(fileName)){
                std::cout << "File already exist, are you sure you want to override it? [y/N] ";
                std::getline(std::cin, answer);
                if (answer != "y" && answer != "Y"){
                    repeat = true;
                }
            }
        }
        app.savePolygonToFile(fileName);
    }
}



