#include "Console.h"
#include "Logger.h"
#include "Loader.h"
#include "Element.h"
#include <fstream>
#include <sstream>

Console::Console() : window{nullptr}, renderer{nullptr}, numberX(3), numberY(3),
                     debug(false), debugMode{ModeApp::Mesh}, alsoSegmentPoints(false), drawDebug{true}, textScale(0.035f), numberBorder(0) {}

Console::~Console() {
    // using smart pointers
    // already terminated before
    //terminate();
}

void Console::setNumberX(unsigned int _numberX){
    numberX = _numberX;
}

void Console::setNumberY(unsigned int _numberY){
    numberY = _numberY;
}

void Console::setDebugStatus(bool what){
    debug = what;
}

void Console::setDebugMode(const ModeApp& mode){
    debugMode = mode;
}

void Console::setFileNameDebug(const std::string& name){
    fileNameDebug = name;
}

void Console::setAlsoSegmentPoint(bool what){
    alsoSegmentPoints = what;
}

void Console::setNumberBorder(unsigned int number){
    if (number >= GetVerticesBorders(0, 0, 0, 0).size()){
        LOG(LogLevel::WARN) << "number too big";
        numberBorder = 0;
    } else{
        numberBorder = number;
    }
}

void Console::start(){
    if (!debug){
        askModeApp();
    } else{
        mode = debugMode;
    }

    askLoadFromFile();

    initWindowAndLibraries();
    if (!app.isVerticesIndicesLoaded()){
        drawPolygon();
    }
    createPolygon();

    std::unique_ptr<Element> element;
    if (mode == ModeApp::Cut){
        if (!app.isSegmentLoaded()){
            drawSegment();
        }
        drawCuttedPolygon();
//        drawBox();
//        drawGrid();
//        moveAround();
    } else if (mode == ModeApp::Mesh){
        element = std::make_unique<Element>(app.getPolygon());
        createElement(*element);
        createMesh(*element);
    }

    terminate();
    if (!debug){
        askSaveToFile(*element);
    }
}

void Console::terminate(){

    Window::terminateGLFW();
}

void Console::drawFillingDoubleBuffers(){
    renderer->clear();
    renderer->drawShapes();
    window->swapBuffer();
    renderer->clear();
    renderer->drawShapes();
}

bool Console::processWindow(){
    if (window->shouldClose()){
        terminate();
        exit(-1);
    }
    window->processImput();
    if (window->isEnterClick() || window->isBackClick() || window->isEscClick()){
        return true;
    }
    if (window->isSpaceClick() || window->isDClick()){
        drawDebug = !drawDebug;
    }
    if (window->isUpClick() || window->isRightClick()){
        if (textScale < 0.1f){
            textScale += 0.005f;
        }
    }
    if (window->isDownClick() || window->isLeftClick()){
        if (textScale > 0.0125f){
            textScale -= 0.005f;
        }
    }
    return false;
}

void Console::drawNoInput(){
    while (true){
        if (processWindow()){
            break;
        }

        renderer->clear();
        renderer->drawShapes();

        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::drawNoInput(const std::vector<Vector2f>& vertices,
                          const std::vector<std::shared_ptr<std::vector<unsigned int>>>& indices){
    while (true){
        if (processWindow()){
            break;
        }

        renderer->clear();
        renderer->drawShapes();

        if (drawDebug){
            drawIndices(vertices, indices);
        }

        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::drawNoInput(const std::vector<std::shared_ptr<std::vector<Vector2f>>>& vertices,
                          const std::vector<std::shared_ptr<std::vector<unsigned int>>>& indices){
    while (true){
        if (processWindow()){
            break;
        }

        renderer->clear();
        renderer->drawShapes();

        if (drawDebug){
            unsigned int size = vertices.size();
            for (unsigned int i = 0; i < size; i++){
               drawIndices(*vertices[i], indices);
               drawIndices(*vertices[i], indices);
            }
        }

        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::drawNoInput(const std::vector<Vector2f>& vertices, const IndicesElement& indicesElement){
    while (true){
        if (processWindow()){
            break;
        }

        renderer->clear();
        renderer->drawShapes();

        if (drawDebug){
            drawIndices(vertices, *indicesElement.indicesInside);
            drawIndices(vertices, *indicesElement.indicesOutside);
        }

        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::drawNoInput(const std::vector<std::shared_ptr<std::vector<Vector2f>>>& vertices,
                          const std::vector<IndicesElement>& indicesElement){
    while (true){
        if (processWindow()){
            break;
        }

        renderer->clear();
        renderer->drawShapes();

        if (drawDebug){
            unsigned int size = indicesElement.size();
            for (unsigned int i = 0; i < size; i++){
               drawIndices(*vertices[i], *indicesElement[i].indicesInside);
               drawIndices(*vertices[i], *indicesElement[i].indicesOutside);
            }
        }

        window->swapBuffer();
        window->waitEvents();
    }
}

void Console::drawIndices(const std::vector<Vector2f>& vertices, const std::vector<std::shared_ptr<std::vector<unsigned int>>>& indices){
    std::stringstream convert;
    for (unsigned int i = 0; i < indices.size(); i++){
        const std::vector<unsigned int>& indi = *indices[i];
        for (unsigned int l = 0; l < indi.size(); l++){
            unsigned int index = indi[l];
            convert << index;
            std::string toString;
            convert >> toString;
            convert.clear();
            renderer->drawText(toString, vertices[index].x, vertices[index].y, textScale);
        }
    }
}

void Console::askLoadFromFile(){
    std::string consoleString;

    bool continueLoop = true;
    while (continueLoop){
        continueLoop = false;
        std::cout << "Do you want to load polygon from file? [y/N] ";
        if (!debug){
            std::getline(std::cin, consoleString);
        } else{
            consoleString = "y";
        }
        if (consoleString == "y" || consoleString == "Y"){
            std::cout << "Enter the name of the file you what to load polygon from: ";
            std::string fileName;
            if (!debug){
                std::getline(std::cin, fileName);
            } else{
                fileName = fileNameDebug;
            }
            fileName = "files/polygons/" + fileName;
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
                if (!debug){
                    std::getline(std::cin, consoleString);
                } else{
                    consoleString = "y";
                }
                if (consoleString == "n" || consoleString == "N"){
                    if (Loader::LoadVerticesIndicesFromFile(app.getVerticesForLoading(), app.getIndicesForLoading(),
                                                     fileName, numberVertices, false) < 0){
                        continueLoop = true;
                        continue;
                    } else{
                        app.setVerticesIndicesLoaded();
                    }
                } else{
                    if (Loader::LoadVerticesIndicesFromFile(app.getVerticesForLoading(), app.getIndicesForLoading(),
                                                     fileName, numberVertices, true) < 0){
                        continueLoop = true;
                        continue;
                    } else{
                        app.setVerticesIndicesLoaded();
                    }
                }
            } else{
                if (Loader::LoadVerticesIndicesFromFile(app.getVerticesForLoading(), app.getIndicesForLoading(),
                                                 fileName, numberVertices, false) < 0){
                    continueLoop = true;
                    continue;
                } else{
                    app.setVerticesIndicesLoaded();
                }
            }
            if (mode == ModeApp::Cut){
                if (Loader::SearchInFile(fileName, "segment", false) > 0){
                    LOG(LogLevel::INFO) << "Segment found";
                    std::cout << "Do you want to load segment? [y/N] ";
                    std::getline(std::cin, consoleString);
                    if (consoleString == "y" || consoleString == "Y"){
                        if (Loader::LoadSegmentFromFile(app.getSegmentForLoading(), fileName) < 0){
                            LOG(LogLevel::WARN) << "Segment not loaded";
                        } else{
                            app.setSegmentLoaded();
                        }
                    }
                }
                LOG::NewLine();
                std::cout << "Do you want to also add segment points if inside polygon? [y/N] ";
                std::getline(std::cin, consoleString);
                if (consoleString == "y" || consoleString == "Y"){
                    alsoSegmentPoints = true;
                } else{
                    alsoSegmentPoints = false;
                }
            }
        }
    }
    if (mode == ModeApp::Mesh && !debug){
        LOG::NewLine();
        askNumberPolygonMesh();
    }
    LOG::NewLine();
//    app.printVertices();
    //    app.printIndices();
}

void Console::askNumberPolygonMesh(){
    std::string consoleString;

    std::stringstream convert;

    while (true){
        std::cout << "Choose the polygon inside the mesh\n";
        std::cout << "Insert 1 for convex, 2 for concave: ";
        std::getline(std::cin, consoleString);

        unsigned int num = 0;

        convert.str(consoleString);
        convert >> num;
        if (convert.fail()){
            LOG(LogLevel::WARN) << "Please insert a valid number";
            convert.clear();
            continue;
        }
        convert.clear();

        if (num != 1 && num != 2){
            LOG(LogLevel::WARN) << "You inserted a wrong number, please try again";
            continue;
        }
        numberBorder = num - 1;
        break;
    }

    LOG::NewLine();

    while (true){
        std::cout << "Choose the number of polygons you want on the x axis\n";
        std::cout << "Insert a number between 1 and 20: ";
        std::getline(std::cin, consoleString);

        unsigned int xNum = 0;

        convert.str(consoleString);
        convert >> xNum;
        if (convert.fail()){
            LOG(LogLevel::WARN) << "Please insert a valid number";
            convert.clear();
            continue;
        }
        convert.clear();

        if (xNum >= 1 && xNum <= 20){
            setNumberX(xNum);
            break;
        } else{
            LOG(LogLevel::WARN) << "You inserted a wrong number, please try again";
            continue;
        }
    }

    LOG::NewLine();

    while (true){
        std::cout << "Choose the number of polygons you want on the y axis\n";
        std::cout << "Please insert a number between 1 and 20: ";
        std::getline(std::cin, consoleString);

        unsigned int yNum = 0;

        convert.str(consoleString);
        convert >> yNum;
        if (convert.fail()){
            LOG(LogLevel::WARN) << "Please insert a valid number";
            convert.clear();
            continue;
        }
        convert.clear();

        if (yNum >= 1 && yNum <= 20){
            setNumberY(yNum);
            break;
        } else{
            LOG(LogLevel::WARN) << "You inserted a wrong number, please try again";
            continue;
        }
    }
}

void Console::askModeApp(){
    std::string consoleString;

    while (true){
        std::cout << "<----- Select what you want to do: ----->\n";
        std::cout << "1) Cut the polygon\n";
        std::cout << "2) Create the mesh\n";
        std::cout << "Please insert a number: ";
        std::getline(std::cin, consoleString);

        if (consoleString == "1"){
            mode = ModeApp::Cut;
            break;
        } else if (consoleString == "2"){
            mode = ModeApp::Mesh;
            break;
        } else{
            LOG(LogLevel::WARN) << "You inserted a wrong number, please try again";
        }
    }

    LOG::NewLine();
}

void Console::initWindowAndLibraries(){
    Window::initiGLFW();

    window = std::make_unique<Window>("Polygon", WIDTH, HEIGHT);

    Renderer::PRINT_INFO = false;

    Renderer::initGLEW();

//    Renderer::initFreetype();

    renderer = std::make_unique<Renderer>();

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
    app.cutMainPolygon(alsoSegmentPoints);
    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices = app.getPolygonsIndices();

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

    drawNoInput(app.getPolygon().getPoints(), polygonsIndices);

    LOG::NewLine(LogLevel::INFO);
}

void Console::askSaveToFile(const Element& element){
    std::string consoleString;

    if (mode == ModeApp::Cut){
        std::cout << "Do you want to save polygon to file? [y/N] ";
        std::getline(std::cin, consoleString);
        if (consoleString == "y" || consoleString == "Y"){
            savePolygon();
        }
    } else if (mode == ModeApp::Mesh){
        std::cout << "Do you want to save element to file? [y/N] ";
        std::getline(std::cin, consoleString);
        if (consoleString == "y" || consoleString == "Y"){
            saveElement(element);
        }
    }
}

void Console::savePolygon(){
    std::string fileName;
    std::string answer;
    while (true){
        std::cout << "Enter the name of the file you want to save polygon to: ";
        std::getline(std::cin, fileName);
        fileName = "files/polygons/" + fileName;
        std::ifstream tryFile;
        tryFile.open(fileName);
        if (!tryFile.fail()){
            tryFile.close();
            std::cout << "File already exist, are you sure you want to override it? [y/N] ";
            std::getline(std::cin, answer);
            if (answer != "y" && answer != "Y"){
                continue;
            } else{
                break;
            }
        } else{
            break;
        }
    }
    Loader::SavePolygonToFile(app.getVertices(), app.getIndices(), app.getSegmentPoints(), app.getPolygonsIndices(), fileName);
}

void Console::saveElement(const Element& element){
    std::string fileName;
    std::string answer;
    while (true){
        std::cout << "Enter the name of the file you want to save element to: ";
        std::getline(std::cin, fileName);
        fileName = "files/elements/" + fileName;
        std::ifstream tryFile;
        tryFile.open(fileName);
        if (!tryFile.fail()){
            tryFile.close();
            std::cout << "File already exist, are you sure you want to override it? [y/N] ";
            std::getline(std::cin, answer);
            if (answer != "y" && answer != "Y"){
                continue;
            } else{
                break;
            }
        } else{
            break;
        }
    }
    Loader::SaveElementToFile(element.getPoints(), element.getPolygonsIndices(), fileName);
}

void Console::drawBox(){
    app.createBox();
//    renderer->addShape(new LinesPointIndices(app.getBoxVertices()));
    std::vector<float> color = Renderer::getNextColor();
    renderer->addShape(new Shape(app.getBoxVertices(), GeometricPrimitive::LinePointClosed,
                                 color[0], color[1], color[2]));

    drawNoInput(app.getPolygon().getPoints(), app.getPolygonsIndices());
}

void Console::drawGrid(){
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

void Console::createElement(Element& element){
    renderer->removeAllShapes();

    element.createElement();

    const std::vector<Vector2f>& points = element.getPoints();
    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices = element.getPolygonsIndices();

    for (unsigned int i = 0; i < polygonsIndices.size(); i++){
        const std::vector<unsigned int>& indices = *polygonsIndices[i];

    //        Shape* shapeNuova = new LinesPointIndices(app.getPolygon().getPoints(), indices);
        std::vector<float> color = Renderer::getNextColor();
        Shape* shapeNuova = new Shape(points, indices, GeometricPrimitive::LinePointClosed,
                                      color[0], color[1], color[2]);
        renderer->addShape(shapeNuova);
    }

    if (!debug){
        drawNoInput(points, polygonsIndices);
    }
}

void Console::createMesh(Element& element){
    drawDebug = false;

    renderer->removeAllShapes();

    float windowBorder = 0.2f;
    float thickBorder  = 0.1f;

    float remaining = 2 - 2 * windowBorder;

    float elemenWidth = remaining / numberX;
    float elementHeight = remaining / numberY;

    const std::vector<std::vector<Vector2f>> verticesBorders = GetVerticesBorders(remaining, remaining, thickBorder, thickBorder);

    const std::vector<Vector2f>& verticesBorder = verticesBorders[numberBorder];

    std::vector<float> yellow = Renderer::getColor(RendColor::Yellow);
    Shape* shapeNuova = new Shape(verticesBorder, GeometricPrimitive::LinePointClosed,
                                  yellow[0], yellow[1], yellow[2]);
    renderer->addShape(shapeNuova);

//    unsigned int numberX = 2;
//    unsigned int numberY = 2;
    Mesh mesh = Mesh(element, verticesBorder, numberX, numberY, elemenWidth, elementHeight);

    const unsigned int numberElements = mesh.getNumberElements();

    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& indices = mesh.getIndices();
    unsigned int numberPolygons = mesh.getNumberPolygons();

    for (unsigned int q = 0; q < numberElements; q++){
        const std::vector<Vector2f>& vertices = mesh.getVertices(q);
        std::vector<float> nextColor = Renderer::getNextColor();
        for (unsigned int i = 0; i < numberPolygons; i++){
            Shape* shapeNuova = new Shape(vertices, *indices[i], GeometricPrimitive::LinePointClosed,
                                          nextColor[0], nextColor[1], nextColor[2]);
            renderer->addShape(shapeNuova);
        }
    }
    drawNoInput(mesh.getAllVertices(), indices);
    renderer->removeAllShapes();

    std::vector<float> colorInside = Renderer::getColor(RendColor::Green);
    std::vector<float> colorOutside = Renderer::getColor(RendColor::Red);

    std::vector<IndicesElement> indicesElement = numberBorder == 0 ? mesh.cut() : mesh.cutConcave();

//    for (unsigned int i = 1; i < 2; i++){
    for (unsigned int i = 0; i < indicesElement.size(); i++){
        const std::vector<Vector2f>& vertices = mesh.getVertices(i);
        const std::vector<std::shared_ptr<std::vector<unsigned int>>>& insideIndices = *indicesElement[i].indicesInside;
        const std::vector<std::shared_ptr<std::vector<unsigned int>>>& outsideIndices = *indicesElement[i].indicesOutside;
        std::cout << "Inside:\n";
        for (unsigned int n = 0; n < insideIndices.size(); n++){
            std::vector<unsigned int>& indi = *insideIndices[n];
            for (unsigned int il = 0; il < indi.size(); il++){
                std::cout << indi[il] << " ";
            }
            std::cout << "\n";
            Shape* shapeNuova = new Shape(vertices, *insideIndices[n], GeometricPrimitive::LinePointClosed,
                                          colorInside[0], colorInside[1], colorInside[2]);
            renderer->addShape(shapeNuova);
        }
        std::cout << "Outside:\n";
        for (unsigned int n = 0; n < outsideIndices.size(); n++){
            std::vector<unsigned int>& indi = *outsideIndices[n];
            for (unsigned int il = 0; il < indi.size(); il++){
                std::cout << indi[il] << " ";
            }
            std::cout << "\n";
            Shape* shapeNuova = new Shape(vertices, *outsideIndices[n], GeometricPrimitive::LinePointClosed,
                                          colorOutside[0], colorOutside[1], colorOutside[2]);
            renderer->addShape(shapeNuova);
        }
    }

    Shape* shapeBorder = new Shape(verticesBorder, GeometricPrimitive::LinePointClosed,
                                  yellow[0], yellow[1], yellow[2]);
    renderer->addShape(shapeBorder);

    drawNoInput(mesh.getAllVertices(), indicesElement);
    //    drawNoInput(mesh.getVertices(1), indicesElement[1]);
}

std::vector<std::vector<Vector2f>> Console::GetVerticesBorders(float width, float height, float borderThickX, float borderThickY){
    float halfWidth  = width  / 2.0f;
    float halfHeight = height / 2.0f;

    float quarterWidth  = halfWidth  / 2.0f;
    float quarterHeight = halfHeight / 2.0f;

//    float eightWidth  = quarterWidth  / 2.0f;
    float eightHeight = quarterHeight / 2.0f;

    std::vector<std::vector<Vector2f>> result = {
        {   {halfWidth, 0.0f},
            {width - borderThickX, halfHeight},
            {halfWidth, height - borderThickY},
            {0.0f, halfHeight}
        },
        {   {quarterWidth, 0.0f},
            {halfWidth, halfHeight - eightHeight},
            {width - quarterWidth, eightHeight},
            {width - borderThickX, halfHeight},
            {width - quarterWidth, height - borderThickY},
            {halfWidth, halfHeight + eightHeight},
            {quarterWidth, height - eightHeight},
            {0.0f, halfHeight}
        }
    };

    for (std::vector<Vector2f>& arrayVec : result){
        for (Vector2f& vec : arrayVec){
            vec -= {halfWidth, halfHeight};
        }
    }

    return result;
}
