#include "Console.h"
#include "Logger.h"
#include "Loader.h"
#include <fstream>

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
    drawBox();
    terminate();
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

void Console::drawNoInput(){
    while (true){
        if (window->shouldClose()){
            terminate();
            exit(-1);
        }
        window->processImput();
        if (window->isEnterClick() || window->isBackClick() || window->isEscClick()){
            break;
        }
        renderer->clear();
        renderer->drawShapes();
        window->swapBuffer();
        window->waitEvents();
    }
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
            fileName = "files/" + fileName;
            int numberVertices = Loader::GetNumberVerticesFromFile(fileName);
            if (numberVertices <= 0){
                continueLoop = true;
                continue;
            }
            if (Loader::SearchInFile(fileName, "vertices") <= 0){
                continueLoop = true;
                continue;
            } else{
                LOG(LogLevel::INFO) << "Vertices found";
            }
            if (Loader::SearchInFile(fileName, "indices", false) > 0){
                LOG(LogLevel::INFO) << "Indices found";
                std::cout << "Do you want to load indices? (If not it is assumed that vertices are in order) [Y/n] ";
                std::getline(std::cin, consoleString);
                if (consoleString == "n" || consoleString == "N"){
                    if (Loader::LoadVerticesFromFile(app.getVerticesForLoading(), app.getIndicesForLoading(),
                                                     fileName, numberVertices, false) < 0){
                        continueLoop = true;
                        continue;
                    } else{
                        app.setVerticesIndicesLoaded();
                    }
                } else{
                    if (Loader::LoadVerticesFromFile(app.getVerticesForLoading(), app.getIndicesForLoading(),
                                                     fileName, numberVertices, true) < 0){
                        continueLoop = true;
                        continue;
                    } else{
                        app.setVerticesIndicesLoaded();
                    }
                }
            } else{
                if (Loader::LoadVerticesFromFile(app.getVerticesForLoading(), app.getIndicesForLoading(),
                                                 fileName, numberVertices, false) < 0){
                    continueLoop = true;
                    continue;
                } else{
                    app.setVerticesIndicesLoaded();
                }
            }
            if (Loader::SearchInFile(fileName, "segment", false) > 0){
                LOG(LogLevel::INFO) << "Segment found";
                std::cout << "Do you want to load it? [y/N] ";
                std::getline(std::cin, consoleString);
                if (consoleString == "y" || consoleString == "Y"){
                    if (Loader::LoadSegmentFromFile(app.getSegmentForLoading(), fileName) < 0){
                        LOG(LogLevel::WARN) << "Segment not loaded";
                    } else{
                        app.setSegmentLoaded();
                    }
                }
            }
        }
    }
    LOG::NewLine();
//    app.printVertices();
//    app.printIndices();
}

void Console::initWindowAndLibraries(){
    Window::initiGLFW();

    window = new Window("Polygon", WIDTH, HEIGHT);

    Renderer::initGLEW();

    renderer = new Renderer();
    renderer->setPolygonColorFloat(WHITE);

    LOG::NewLine(LogLevel::INFO);
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
        renderer->replaceShape(0, new LinesPointIndicesOpen(app.getVertices(), app.getIndices()));

        renderer->drawShapes();

        if (!window->isLeftClick() && added){
            app.removeLastVertex();
        }

        if (window->isEnterClick()){
            if (app.closePolygon()){
                end = true;
            } else{
                LOG(LogLevel::WARN) << "Cannot close polygon";
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
        window->processImput();
        renderer->clear();

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

    LOG::NewLine(LogLevel::INFO);
    LOG(LogLevel::INFO) << "Number of polygons: " << polygonsIndices.size();
    for (unsigned int i = 0; i < polygonsIndices.size(); i++){
        const std::vector<unsigned int>& indices = *polygonsIndices[i];
        {
            LOG log = LOG(LogLevel::INFO);
            for (unsigned int n = 0; n < indices.size(); n++){
                log << indices[n] << " ";
            }
        }

        Shape* shapeNuova = new LinesPointIndices(app.getPolygon().getPoints(), indices);
        renderer->addShape(shapeNuova);
    }

    drawNoInput();

    LOG::NewLine(LogLevel::INFO);
}

void Console::drawBox(){
    app.createBox();
    renderer->addShape(new LinesPointIndices(app.getBoxVertices()));
    drawNoInput();
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
            fileName = "files/" + fileName;
            std::ifstream tryFile;
            tryFile.open(fileName);
            if (!tryFile.fail()){
                tryFile.close();
                std::cout << "File already exist, are you sure you want to override it? [y/N] ";
                std::getline(std::cin, answer);
                if (answer != "y" && answer != "Y"){
                    repeat = true;
                }
            }
        }
        Loader::SavePolygonToFile(app.getVertices(), app.getIndices(), app.getSegmentPoints(), app.getPolygonsIndices(), fileName);
    }
}



