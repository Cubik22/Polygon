#include "Mesh.h"
#include "Logger.h"

Mesh::Mesh(const Element& _element, const std::vector<Vector2f> _verticesBorder,
           unsigned int _numberX, unsigned int _numberY, float _elementWidth, float _elementHeight) :
    verticesBorder(_verticesBorder), numberX(_numberX), numberY(_numberY),
    elementWidth(_elementWidth), elementHeight(_elementHeight),
    numberElements(Mesh::debug ? 1 : numberX * numberY), numberPolygons(_element.getPolygonsIndices().size()){

    Polygon::createBoundingBoxVariables(verticesBorder, width, height, xMin, yMin);

    const std::vector<std::shared_ptr<std::vector<unsigned int>>>& _indices = _element.getPolygonsIndices();

    unsigned int indicesSize = _indices.size();
    for (unsigned int i = 0; i < indicesSize; i++){
        indices.push_back(_indices[i]);
        //std::cout << indices[i]->size() << "\n";
    }

    const std::vector<Vector2f>& _vertices = _element.getPoints();
    float originalWidth  = _element.getWidth();
    float originalHeight = _element.getHeight();
    float originalXMin   = _element.getXMin();
    float originalYMin   = _element.getYMin();

    Polygon::scaleXYMin(&originalXMin, &originalYMin, originalWidth, originalHeight, elementWidth, elementHeight);

    if (!debug){
        for (unsigned int x = 0; x < numberX; x++){
            for (unsigned int y = 0; y < numberY; y++){
                vertices.push_back(Polygon::translateVerticesPointer(Polygon::scaleVertices(
                                   _vertices, originalWidth, originalHeight, elementWidth, elementHeight),
                                   xMin - originalXMin + x * elementWidth, yMin - originalYMin + y * elementHeight));
            }
        }
    } else{
        vertices.push_back(Polygon::translateVerticesPointer(Polygon::scaleVertices(
                           _vertices, originalWidth, originalHeight, elementWidth, elementHeight),
                           xMin - originalXMin + xDebug * elementWidth, yMin - originalYMin + yDebug * elementHeight));
    }
}

unsigned int Mesh::getNumberPolygons() const
{
    return numberPolygons;
}

unsigned int Mesh::getNumberElements() const{
    return numberElements;
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

const std::vector<std::shared_ptr<std::vector<Vector2f>>> Mesh::getAllVertices() const{
    return vertices;
}

const std::vector<std::shared_ptr<std::vector<unsigned int>>>& Mesh::getIndices() const{
    return indices;
}

std::vector<IndicesElement> Mesh::cut(){
    std::vector<IndicesElement> elements;
    for (unsigned int i = 0; i < numberElements; i++){
        elements.push_back(cutElement(getVerticesPrivate(i), indices));
        //std::cout << "finito x: " << x << " y: " << y << "\n";
    }
    return elements;
}

std::vector<IndicesElement> Mesh::cutConcave(){
    std::vector<IndicesElement> elements;
    for (unsigned int i = 0; i < numberElements; i++){
        elements.push_back(cutElement(getVerticesPrivate(i), indices, true));
        //std::cout << "finito x: " << x << " y: " << y << "\n";
    }
    return elements;
}

// STATIC

bool Mesh::debug = false;

unsigned int Mesh::xDebug = 0;
unsigned int Mesh::yDebug = 0;

bool Mesh::forceConcave = false;

void Mesh::setDebugMode(bool mode){
    debug = mode;
}

void Mesh::setXDebug(unsigned int xD){
    xDebug = xD;
}

void Mesh::setYDebug(unsigned int yD){
    yDebug = yD;
}

void Mesh::setForceConcave(bool mode){
    forceConcave = mode;
}

// PRIVATE

void printElement(std::vector<Vector2f>& verticesElement){
    std::cout << "vertices element\n";
    for (unsigned int l = 0; l < verticesElement.size(); l++){
        std::cout << l << ": x: " << verticesElement[l].x << " y: " << verticesElement[l].y << "\n";
    }
}

void printIndices(std::vector<unsigned int>& indices){
    std::cout << "vertices indices\n";
    for (unsigned int l = 0; l < indices.size(); l++){
        std::cout << l << ": " << indices[l] << "\n";
    }
}

IndicesElement Mesh::cutElement(std::vector<Vector2f>& verticesElement,
                                const std::vector<std::shared_ptr<std::vector<unsigned int>>>& startIndices, bool concave){
    if (Mesh::forceConcave == true){
        concave = true;
    }
    unsigned int numberVerticesBorder = verticesBorder.size();
    std::vector<unsigned int> indicesBorder;
    for (unsigned int i = 0; i < numberVerticesBorder; i++){
        indicesBorder.push_back(i);
    }

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
        if (!concave){
            poly.cutInsideOutside(*indicesInside, *indicesOutside);
        } else{
            poly.cutInsideOutsideConcave(*indicesInside, *indicesOutside, verticesBorder, indicesBorder, RelativePosition::Parallel);
        }
    }

//    std::cout << "OUTSIDE INDICES\n";
//    for (unsigned int i = 0; i  < indicesOutside->size(); i++){
//        std::vector<unsigned int>& indi = *indicesOutside->at(i);
//        for (unsigned int q = 0; q < indi.size(); q++){
//            std::cout << indi[q] << " ";
//        }
//        std::cout << "\n";
//    }

    if (true){
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
                if (!concave){
                    poly.cutInsideOutside(*tmpIndicesInside, *tmpIndicesOutside);
                } else{
                    poly.cutInsideOutsideConcave(*tmpIndicesInside, *tmpIndicesOutside,
                                                 verticesBorder, indicesBorder, RelativePosition::Positive);
                }

            }

            for (unsigned int i = 0; i < numberIndicesOutside; i++){
                Polygon poly = Polygon(verticesElement, *((*indicesOutside)[i]));
                poly.setSegment(verticesBorder[n], verticesBorder[n + 1]);
                poly.createNetworkMesh(verticesElement);
                //poly.createNetwork();
                if (!concave){
                    poly.cutIndices(*tmpIndicesOutside);
                } else{
                    poly.cutInsideOutsideConcave(*tmpIndicesInside, *tmpIndicesOutside,
                                                 verticesBorder, indicesBorder, RelativePosition::Negative);
                }
            }
            indicesInside = tmpIndicesInside;
            indicesOutside = tmpIndicesOutside;
        }
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
