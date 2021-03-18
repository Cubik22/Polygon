#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <list>
#include <iterator>
#include <exception>
#include "Intersector.h"

class Polygon{
private:
    std::vector<Vector2f> points;
    std::list<unsigned int> indices;
    Vector2f p1;
    Vector2f p2;
public:
    Polygon();
    Polygon(const std::vector<Vector2f>& _points, const std::list<unsigned int>& _indices);
    void setSegment(const Vector2f& _p1, const Vector2f& _p2);
    const Vector2f& getPoint(unsigned int index) const;
    unsigned int getNumberIndices() const;
    void addPoint(const Vector2f& point, unsigned int previous);
};

#endif //POLYGON_H
