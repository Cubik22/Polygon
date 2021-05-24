#include "Mesh.h"
#include "Logger.h"

Mesh::Mesh(const Element& _element, const std::vector<Vector2f> _verticesBorder,
           const unsigned int& _numberX, const unsigned int& _numberY) :
    verticesBorder(_verticesBorder), numberX(_numberX), numberY(_numberY){

    Polygon::createBoundingBoxVariables(verticesBorder, width, height, xMin, yMin);

    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& _indices = _element.getPolygonsIndices();
    numberPolygons = _indices.size();
    unsigned int indicesSize = _indices.size();
    for (unsigned int i = 0; i < indicesSize; i++){
        indices.push_back(_indices[i]);
        //std::cout << indices[i]->size() << "\n";
    }

    const std::vector<Vector2f>& _vertices = _element.getPoints();
    float elementWidth = _element.getWidth();
    float elementHeight = _element.getHeight();
    float elementXMin = _element.getXMin();
    float elementYMin = _element.getYMin();

    if (!debug){
        for (unsigned int x = 0; x < numberX; x++){
            for (unsigned int y = 0; y < numberY; y++){
                vertices.push_back(Polygon::translateVerticesPointer(_vertices,
                                   xMin - elementXMin + x * elementWidth, yMin - elementYMin + y * elementHeight));
            }
        }
    } else{
        vertices.push_back(Polygon::translateVerticesPointer(_vertices,
                           xMin - elementXMin + xDebug * elementWidth, yMin - elementYMin + yDebug * elementHeight));
    }
}

unsigned int Mesh::getNumberPolygons() const
{
    return numberPolygons;
}

float Mesh::getWidth() const
{
    return width;
}

float Mesh::getHeight() const
{
    return height;
}

float Mesh::getXMin() const
{
    return xMin;
}

float Mesh::getYMin() const
{
    return yMin;
}

unsigned int Mesh::getNumberX() const
{
    return numberX;
}

unsigned int Mesh::getNumberY() const
{
    return numberY;
}

const std::vector<Vector2f>& Mesh::getVertices(unsigned int x, unsigned int y) const
{
    if (x >= numberX){
        LOG(LogLevel::ERROR) << "x selected is greater than number of x, returned 0 instead";
        x = 0;
    }
    if (y >= numberY){
        LOG(LogLevel::ERROR) << "y selected is greater than number of y, returned 0 instead";
        y = 0;
    }
    return *vertices[x * numberY + y];
}

const std::vector<Vector2f>& Mesh::getVertices(unsigned int i) const
{
    if (i >= numberX * numberY){
        LOG(LogLevel::ERROR) << "i selected is greater than number of vertices, returned 0 instead";
        return *vertices[0];
    }
    return *vertices[i];
}

const std::vector<std::shared_ptr<std::vector<unsigned int>>>& Mesh::getIndices() const{
    return indices;
}

std::vector<IndicesElement> Mesh::cut(){
    std::vector<IndicesElement> elements;
    for (unsigned int x = 0; x < numberX; x++){
        for (unsigned int y = 0; y < numberY; y++){
            elements.push_back(cutElement(getVerticesPrivate(x, y), indices));
            //std::cout << "finito x: " << x << " y: " << y << "\n";
        }
    }
    return elements;
}

// STATIC

bool Mesh::debug = false;

unsigned int Mesh::xDebug = 0;
unsigned int Mesh::yDebug = 0;

void Mesh::setDebugMode(bool mode){
    debug = mode;
}

void Mesh::setXDebug(unsigned int xD){
    xDebug = xD;
}

void Mesh::setYDebug(unsigned int yD){
    yDebug = yD;
}

// PRIVATE

void printElement(std::vector<Vector2f>& verticesElement){
    std::cout << "vertices element\n";
    for (unsigned int l = 0; l < verticesElement.size(); l++){
        std::cout << l << ": x: " << verticesElement[l].x + 0.8 << " y: " << verticesElement[l].y + 0.8 << "\n";
    }
}

void printIndices(std::vector<unsigned int>& indices){
    std::cout << "vertices indices\n";
    for (unsigned int l = 0; l < indices.size(); l++){
        std::cout << l << ": " << indices[l] << "\n";
    }
}

IndicesElement Mesh::cutElement(std::vector<Vector2f>& verticesElement,
                                const std::vector<std::shared_ptr<std::vector<unsigned int>>>& startIndices){
    //printElement(verticesElement);
    //LDEB << "cut element";
    unsigned int numberVerticesBorder = verticesBorder.size();
//    std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned int>>>> startIndicesPoi =
//                    std::make_shared<std::vector<std::shared_ptr<std::vector<unsigned int>>>>(startIndices);
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned int>>>> indicesInside =
                    std::make_shared<std::vector<std::shared_ptr<std::vector<unsigned int>>>>();
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned int>>>> indicesOutside =
                    std::make_shared<std::vector<std::shared_ptr<std::vector<unsigned int>>>>();
    for (unsigned int i = 0; i < numberPolygons; i++){
        Polygon poly = Polygon(verticesElement, *(startIndices[i]));
        poly.setSegment(verticesBorder[numberVerticesBorder - 1], verticesBorder[0]);
//        poly.setSegment(verticesBorder[0], verticesBorder[1]);
        poly.createNetworkMesh(verticesElement);
        //printElement(verticesElement);
        //poly.printNetworkWithCoordinates(LogLevel::ERROR);
        poly.cutInsideOutside(*indicesInside, *indicesOutside);
        //poly.printNetworkWithCoordinates(LogLevel::ERROR);
    }

//    for (unsigned int n = 1; n < 2; n++){
    for (unsigned int n = 0; n < numberVerticesBorder - 1; n++){
        std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned int>>>> tmpIndicesInside =
                        std::make_shared<std::vector<std::shared_ptr<std::vector<unsigned int>>>>();
        std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned int>>>> tmpIndicesOutside =
                        std::make_shared<std::vector<std::shared_ptr<std::vector<unsigned int>>>>();
        unsigned int numberIndicesInside = indicesInside->size();
        unsigned int numberIndicesOutside = indicesOutside->size();
        for (unsigned int i = 0; i < numberIndicesInside; i++){
            Polygon poly = Polygon(verticesElement, *((*indicesInside)[i]));
            poly.setSegment(verticesBorder[n], verticesBorder[n + 1]);
            poly.createNetworkMesh(verticesElement);
            poly.cutInsideOutside(*tmpIndicesInside, *tmpIndicesOutside);
        }

        for (unsigned int i = 0; i < numberIndicesOutside; i++){
            Polygon poly = Polygon(verticesElement, *((*indicesOutside)[i]));
            poly.setSegment(verticesBorder[n], verticesBorder[n + 1]);
            poly.createNetworkMesh(verticesElement);
            //poly.createNetwork();
            poly.cutIndices(*tmpIndicesOutside);
        }
        indicesInside = tmpIndicesInside;
        indicesOutside = tmpIndicesOutside;
    }
    //std::cout << "number of vertices: " << verticesElement.size() << "\n";
    return {indicesInside, indicesOutside};
}

std::vector<Vector2f>& Mesh::getVerticesPrivate(unsigned int x, unsigned int y) const
{
    if (x >= numberX){
        LOG(LogLevel::ERROR) << "x selected is greater than number of x, returned 0 instead";
        x = 0;
    }
    if (y >= numberY){
        LOG(LogLevel::ERROR) << "y selected is greater than number of y, returned 0 instead";
        y = 0;
    }
    return *vertices[x * numberY + y];
}

std::vector<Vector2f>& Mesh::getVerticesPrivate(unsigned int i) const
{
    if (i >= numberX * numberY){
        LOG(LogLevel::ERROR) << "i selected is greater than number of vertices, returned 0 instead";
        return *vertices[0];
    }
    return *vertices[i];
}
