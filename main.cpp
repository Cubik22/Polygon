#include <iostream>
#include "Intersector.h"
#include "Polygon.h"
#include "Renderer.h"
#include "Window.h"
#include "Application.h"

#define WIDTH 1200
#define HEIGHT 800

std::ostream& operator<< (std::ostream& ostrem, const std::vector<unsigned int>& vector){
    for (unsigned int i = 0; i < vector.size(); i++){
        ostrem << vector[i] << " ";
    }
    return ostrem;
}

void drawFillingDoubleBuffers(const Window& window, const Renderer& renderer){
    renderer.clear();
    renderer.drawShapes();
    window.swapBuffer();
    renderer.clear();
    renderer.drawShapes();
}

int main(){

    Window::initiGLFW();

    Window window("Polygon", WIDTH, HEIGHT);

    Renderer::initGLEW();

    Renderer renderer;
    renderer.setPolygonColorFloat(WHITE);

    std::cout << std::endl;

    Application app;

    bool end = false;

    while (!window.shouldClose() && !end){
        renderer.clear();
        window.processImput();
        if (window.isBackClick() || window.isEscClick()){
            app.removeLastVertex();
        }
        bool added = app.addVertex({(float)window.getXMouse(), (float)window.getYMouse()});
        renderer.replaceShape(0, new Lines(app.getVertices(), app.getIndices()));

        renderer.drawShapes();

        if (!window.isLeftClick() && added){
            app.removeLastVertex();
        }

        if (window.isEnterClick()){
            if (app.closePolygon()){
                end = true;
            } else{
                std::cout << "Cannot close polygon\n";
            }
        }

        window.swapBuffer();
        window.waitEvents();
    }

    app.createMainPolygon();
    const Polygon& polygon = app.getPolygon();
    renderer.replaceShape(0, new LinesPointIndices(polygon.getPoints(), polygon.getIndices()));
    drawFillingDoubleBuffers(window, renderer);

    end = false;
    while (!window.shouldClose() && !end){
        renderer.clear();
        window.processImput();

        if (window.isBackClick() || window.isEscClick()){
            app.removeSegmentPoint();
        }

        app.addSegmentPoint({(float)window.getXMouse(), (float)window.getYMouse()});
        if (app.getSegmentSize() == 1){
            renderer.replaceShape(1, new Lines(app.getSegmentPoints(), {}));
        } else if (app.getSegmentSize() == 2){
            renderer.replaceShape(1, new Lines(app.getSegmentPoints(), {0, 1}));
        }

        renderer.drawShapes();

        if (!window.isLeftClick()){
            app.removeSegmentPoint();
        }

        if (app.getSegmentSize() == 2){
            end = true;
        }

        window.swapBuffer();
        window.waitEvents();
    }

    //renderer.replaceShape(1, new Lines(app.getSegmentPoints(), {0, 1}));
    renderer.removeLastShape();
    renderer.removeLastShape();
    app.cutMainPolygon();
    const std::vector<std::vector<unsigned int>*>& polygonsIndices = app.getPolygonsIndices();
    std::cout << "----------------------------------\n";
    std::cout << "Number of polygons: " << polygonsIndices.size() << "\n";
    for (unsigned int i = 0; i < polygonsIndices.size(); i++){
        std::cout << *polygonsIndices[i] << "\n";
        Shape* shapeNuova = new LinesPointIndices(polygon.getPoints(), *polygonsIndices[i]);
        renderer.addShape(shapeNuova);
    }
    drawFillingDoubleBuffers(window, renderer);

    while (!window.shouldClose()){
        window.waitEvents();
    }

    Window::terminateGLFW();

    return 0;

//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
}

//    Intersector inter;
//    inter.setSegment1({0, 2}, {0, 0});
//    inter.setSegment2({2, 1}, {1, 1});
//    inter.calculateIntersection(true, false);
//    IntersectionType type = inter.getIntersectionType();
//    Vector2f point = inter.getIntersectionPoint();
//    std::cout << "type: " << type << std::endl;
//    std::cout << "point: " << point << std::endl;

//    std::vector<float> vertices = {
//        -0.50f,  -0.50f,
//         0.00f,  -0.75f,
//         0.60f,  -0.10f,
//         0.30f,   0.60f
//                         };

//    std::vector<unsigned int> indicesTriangles = {
//        0, 1, 2,
//        2, 3, 0};

//    std::vector<unsigned int> indicesLines = {
//        0, 1, 1, 2, 2, 3, 3, 0 };

//    std::vector<unsigned int> indicesPoint = { 0, 1, 2, 3};

//    renderer.addShape(new Triangles(vertices, indicesTriangles));
//    renderer.addShape(new Lines(vertices, indicesLines));
//    renderer.addShape(new Points(vertices, indicesPoint));
