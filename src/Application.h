#ifndef APPLICATION_H
#define APPLICATION_H

#include "Polygon.h"
#include "Vector2f.h"
#include <vector>

class Application{
private:
    std::vector<Vector2f> vertices;
    std::vector<unsigned int> indices;
public:
    Application();
    unsigned int getNumberVertices() const;
    const std::vector<Vector2f> getVertices() const;
    const std::vector<unsigned int> getIndices() const;
    bool addVertex(const Vector2f& vertex);
    void closePolygon();
    void removeLastVertex();
};

#endif // APPLICATION_H
