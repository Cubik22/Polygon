#include "Polygon.h"

Polygon::Polygon(){}

Polygon::Polygon(const std::vector<Vector2f> &_points, const std::list<unsigned int> &_indices) :
    points{_points}, indices{_indices} {
    if (points.size() != indices.size()){
        throw std::runtime_error("Points vector and indices list should have the same size");
    }
}

void Polygon::setSegment(const Vector2f &_p1, const Vector2f &_p2){
    p1 = _p1;
    p2 = _p2;
}

const Vector2f &Polygon::getPoint(unsigned int index) const{
    return points[index];
}

unsigned int Polygon::getNumberIndices() const{
    return indices.size();
}

void Polygon::addPoint(const Vector2f &point, unsigned int previous){
    points.push_back(point);
    bool find = false;
    for (std::list<unsigned int>::iterator it = indices.begin(); it != indices.end(); it++){
        if (find == true){
            indices.insert(it, getNumberIndices());
            return;
        }
        if (*it == previous){
            find = true;
        }
    }
    indices.push_back(getNumberIndices());
}
