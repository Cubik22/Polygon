#include <iostream>
#include "Intersector.h"
#include "Polygon.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#define WIDTH 600
#define HEIGHT 600
#define CLEAR_COLOR 0.7725f, 0.9372f, 0.9686f, 1.0f

std::ostream& operator<< (std::ostream& ostrem, const Point& point){
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

int main(int argc, char *argv[]){
    Intersector inter;
    inter.setLine({0, 2}, {0, 0});
    inter.setSegments({2, 1}, {1, 1});
    inter.calculateIntersection();
    IntersectionType type = inter.getIntersectionType();
    Point point = inter.getIntersectionPoint();
    std::cout << "type: " << type << std::endl;
    std::cout << "point: " << point << std::endl;

    char* name = "Polygon";

    GLFWwindow *window;

        /* Initialize the library */
        if (!glfwInit()){
            exit(-1);
        }

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(WIDTH, HEIGHT, name, NULL, NULL);
        if (!window){
            glfwTerminate();
            exit(-1);
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        //constant frame rate
        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK){
                printf("ERRORE di caricamento di Glew\n");
            }
        glClearColor(CLEAR_COLOR);
        printf("VERSION: %s\n", glGetString(GL_VERSION));

            // Returns the vendor
            auto vendor = glGetString(GL_VENDOR);
            // Returns a hint to the model
            auto model = glGetString(GL_RENDERER);

            printf("VENDOR: %s\n", vendor);
            printf("MODEL: %s\n", model);

            while (!glfwWindowShouldClose(window)){
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }

    glfwTerminate();
    return 0;

//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
}
