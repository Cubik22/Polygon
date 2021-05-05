#include "Console.h"
#include "Logger.h"
#include "Loader.h"
#include "Element.h"
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
    createElement();
//    if (!app.isSegmentLoaded()){
//        drawSegment();
//    }
//    drawCuttedPolygon();
    //drawBox();
    //moveAround();
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

    Renderer::PRINT_INFO = false;
    Renderer::initGLEW();

    renderer = new Renderer();

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
        //renderer->replaceShape(0, new LinesPointIndicesOpen(app.getVertices(), app.getIndices()));
        std::vector<float> color = Renderer::getLastColor();
        renderer->replaceShape(0, new Shape(app.getVertices(), app.getIndices(), GeometricPrimitive::LinePointOpen,
                                            color[0], color[1], color[2]));

        renderer->drawShapes();

        if (!window->isMouseLeftClick() && added){
            app.removeLastVertex();
        }

        if (window->isEnterClick() || window->isMouseRightClick()){
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
//    renderer->replaceShape(0, new LinesPointIndices(polygon.getPoints(), polygon.getIndices()));
    std::vector<float> color = Renderer::getLastColor();
    renderer->replaceShape(0, new Shape(polygon.getPoints(), polygon.getIndices(), GeometricPrimitive::LinePointClosed,
                                        color[0], color[1], color[2]));
    Renderer::getNextColor();
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
//        if (app.getSegmentSize() == 1){
////            renderer->replaceShape(1, new Lines(app.getSegmentPoints(), {}));
//            renderer->replaceShape(1, new Shape(app.getSegmentPoints(), GeometricPrimitive::LinePointOpen));
//        } else if (app.getSegmentSize() == 2){
////            renderer->replaceShape(1, new Lines(app.getSegmentPoints(), {0, 1}));
//            renderer->replaceShape(1, new Shape(app.getSegmentPoints(), GeometricPrimitive::LinePointOpen));
//        }
        if (app.getSegmentSize() == 1 || app.getSegmentSize() == 2){
            std::vector<float> color = Renderer::getLastColor();
            renderer->replaceShape(1, new Shape(app.getSegmentPoints(), GeometricPrimitive::LinePointOpen,
                                                color[0], color[1], color[2]));
        } else{
            LOG(LogLevel::DEBUG) << "3 points in segment";
        }

        renderer->drawShapes();

        if (!window->isMouseLeftClick()){
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

//        Shape* shapeNuova = new LinesPointIndices(app.getPolygon().getPoints(), indices);
        std::vector<float> color = Renderer::getNextColor();
        Shape* shapeNuova = new Shape(app.getPolygon().getPoints(), indices, GeometricPrimitive::LinePointClosed,
                                      color[0], color[1], color[2]);
        renderer->addShape(shapeNuova);
    }

    drawNoInput();

    LOG::NewLine(LogLevel::INFO);
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

void Console::drawBox(){
    app.createBox();
//    renderer->addShape(new LinesPointIndices(app.getBoxVertices()));
    std::vector<float> color = Renderer::getNextColor();
    renderer->addShape(new Shape(app.getBoxVertices(), GeometricPrimitive::LinePointClosed,
                                 color[0], color[1], color[2]));
    drawNoInput();
    renderer->removeLastShape();
    renderer->removeLastShape();

    //moveAround();

    float min = 0.2;
    float side = 0.2;
    unsigned int number = (2 - 2 * min) / side;

    app.createGrid(-1 + min, -1 + min, side, side, number, number);
    for (unsigned int x = 0; x < number; x++){
        for (unsigned int y = 0; y < number; y++){
            //renderer->addShape(new LinesPointIndices(app.getBox(x * number + y), app.getIndices()));
            std::vector<float> color = Renderer::getNextColor();
            renderer->addShape(new Shape(app.getBox(x * number + y), app.getIndices(), GeometricPrimitive::LinePointClosed,
                                         color[0], color[1], color[2]));
        }
    }

    drawNoInput();
}

void Console::moveAround(){
    app.createBox();
    app.copyPolygon(0, 0, app.getWidth(), app.getHeight());
    const std::vector<Vector2f>& box = app.getBox(0);
//    renderer->replaceShape(0, new LinesPointIndices(box, app.getIndices()));
    std::vector<float> color = Renderer::getNextColor();
    renderer->replaceShape(0, new Shape(box, app.getIndices(), GeometricPrimitive::LinePointClosed, color[0], color[1], color[2]));
    Renderer::getNextColor();

    float change = 0.1;

    while (true){
        if (window->shouldClose()){
            terminate();
            exit(-1);
        }
        window->processImput();
        if (window->isEnterClick() || window->isBackClick() || window->isEscClick()){
            break;
        }
        if (window->isUpClick()){
            app.movePolygon(0, 0, change);
        }
        if (window->isDownClick()){
            app.movePolygon(0, 0, -change);
        }
        if (window->isRightClick()){
            app.movePolygon(0, change, 0);
        }
        if (window->isLeftClick()){
            app.movePolygon(0, -change, 0);
        }
//        renderer->replaceShape(0, new LinesPointIndices(app.getBox(0), app.getIndices()));
        std::vector<float> color = Renderer::getLastColor();
        renderer->replaceShape(0, new Shape(app.getBox(0), app.getIndices(), GeometricPrimitive::LinePointClosed,
                                            color[0], color[1], color[2]));
        renderer->clear();
        renderer->drawShapes();
        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::createElement(){
    renderer->removeAllShapes();

    Element element = Element(app.getPolygon());
    element.createElement();

    const std::vector<Vector2f>& points = element.getPoints();
    const std::vector<std::vector<unsigned int>*>& polygonsIndices = element.getPolygonsIndices();

    for (unsigned int i = 0; i < polygonsIndices.size(); i++){
        const std::vector<unsigned int>& indices = *polygonsIndices[i];

    //        Shape* shapeNuova = new LinesPointIndices(app.getPolygon().getPoints(), indices);
        std::vector<float> color = Renderer::getNextColor();
        Shape* shapeNuova = new Shape(points, indices, GeometricPrimitive::LinePointClosed,
                                      color[0], color[1], color[2]);
        renderer->addShape(shapeNuova);
    }

    drawNoInput();
}
