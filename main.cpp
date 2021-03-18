#include <iostream>
#include "Intersector.h"
#include "Polygon.h"
#include "Renderer.h"
#include "Window.h"
#include "Application.h"

#define WIDTH 600
#define HEIGHT 600

#define BLACK 0.0, 0.0, 0,0
#define WHITE 1.0, 1.0, 1.0

std::ostream& operator<< (std::ostream& ostrem, const Vector2f& point){
    return ostrem << "x: " << point.x << " y: " << point.y;
}

std::ostream& operator<< (std::ostream& ostrem, const IntersectionType& type){
    if (type == IntersectionType::Parallel){
        return ostrem << "Parallel";
    } else if (type == IntersectionType::InsideSegment){
        return ostrem << "Inside segment";
    } else if (type == IntersectionType::OutsideSegment){
        return ostrem << "Outside segment";
    }
    return ostrem;
}

int main(){
    Intersector inter;
    inter.setSegment1({0, 2}, {0, 0});
    inter.setSegment2({2, 1}, {1, 1});
    inter.calculateIntersection(true, false);
    IntersectionType type = inter.getIntersectionType();
    Vector2f point = inter.getIntersectionPoint();
    std::cout << "type: " << type << std::endl;
    std::cout << "point: " << point << std::endl;

    Window::initiGLFW();

    Window window("Polygon", WIDTH, HEIGHT);

    Renderer::initGLEW();

    Renderer renderer;
    renderer.setPolygonColorFloat(WHITE);

    std::vector<float> vertices = {
        -0.50f,  -0.50f,
         0.00f,  -0.75f,
         0.60f,  -0.10f,
         0.30f,   0.60f
                         };

    std::vector<unsigned int> indicesTriangles = {
        0, 1, 2,
        2, 3, 0};

    std::vector<unsigned int> indicesLines = {
        0, 1, 1, 2, 2, 3, 3, 0 };

    std::vector<unsigned int> indicesPoint = { 0, 1, 2, 3};


    //renderer.addShape(new Triangles(vertices, indicesTriangles));
    //renderer.addShape(new Lines(vertices, indicesLines));
    //renderer.addShape(new Points(vertices, indicesPoint));

    Application app;

    bool end = false;

    while (!window.shouldClose() && !end){
        renderer.clear();
        window.processImput();
        bool added = app.addVertex({(float)window.getXMouse(), (float)window.getYMouse()});
        renderer.replaceShape(0, new Lines(app.getVertices(), app.getIndices()));

        renderer.drawShapes();

        if (!window.isLeftClick() && added){
            app.removeLastVertex();
        }

        if (window.isEnterPressed()){
            end = true;
            app.closePolygon();
        }

        window.swapBuffer();
        window.waitEvents();
    }

    renderer.replaceShape(0, new Lines(app.getVertices(), app.getIndices()));
    renderer.clear();
    renderer.drawShapes();
    window.swapBuffer();
    renderer.clear();
    renderer.drawShapes();

    while (!window.shouldClose()){
        //window.swapBuffer();
        window.waitEvents();
    }

    Window::terminateGLFW();

    return 0;

//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
}
