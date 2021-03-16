#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <list>
#include <iterator>
#include <exception>
#include "Intersector.h"

class Polygon{
private:
    std::vector<Point> points;
    std::list<unsigned int> vertices;
    Point p1;
    Point p2;
public:
    Polygon(){}
    Polygon(const std::vector<Point>& _points, const std::list<unsigned int>& _vertices) :
        points{_points}, vertices{_vertices} {
        if (points.size() != vertices.size()){
            throw std::runtime_error("Points vector and Vertices list should have the same size");
        }
    }
    void setSegment(const Point& _p1, const Point& _p2){
        p1 = _p1;
        p2 = _p2;
    }
    const Point& getPoint(unsigned int vertex) const{
        return points[vertex];
    }
    unsigned int getNumberVertices() const{
        return vertices.size();
    }
    void addPoint(const Point& point, unsigned int previous){
        points.push_back(point);
        bool find = false;
        for (std::list<unsigned int>::iterator it = vertices.begin(); it != vertices.end(); it++){
            if (find == true){
                vertices.insert(it, getNumberVertices());
                return;
            }
            if (*it == previous){
                find = true;
            }
        }
        vertices.push_back(getNumberVertices());
    }
};

#endif //POLYGON_H
