#ifndef INTERSECTOR_H
#define INTERSECTOR_H

#include "Vector2f.h"

enum class IntersectionType{
    Parallel = 0,
    OutsideSegment = 1,
    InsideSegment = 2,
    OnVertex = 3
};

class Intersector{
private:
    bool calculated;
    double toleranceParallelism;
    float toleranceOnVertex;
    IntersectionType intersectionType;
    Vector2f intersectionPoint;
    double a1, b1, c1;
    double a2, b2, c2;
    Vector2f r1;
    Vector2f r2;
    Vector2f s1;
    Vector2f s2;
    void calculateLines();
public:
    Intersector();
    void setSegment1(const Vector2f& _r1, const Vector2f& _r2);
    void setSegment2(const Vector2f& _s1, const Vector2f& _s2);
    void setToleranceParallelism(double tolerance);
    void setToleranceOnVertex(float tolerance);
    Vector2f getIntersectionPoint();
    IntersectionType getIntersectionType();
    IntersectionType calculateIntersection(bool isFirstLine = false, bool isSecondLine = false);
};

#endif // INTERSECTOR_H
