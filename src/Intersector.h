#ifndef INTERSECTOR_H
#define INTERSECTOR_H

#include "Vector2f.h"
#include <iostream>


enum class IntersectionType{
    Parallel        = 0,
    OutsideSegment  = 1,
    InsideSegment   = 2,
    FirstOnVertex   = 3,
    SecondOnVertex  = 4,
    BothOnVertex    = 5
};

enum class LineSegmentIntersection{
    Parallel        = 0,
    OutsideSegment  = 1,
    InsideSegment   = 2,
    FirstVertex     = 3,
    SecondVertex    = 4
};

enum class RelativePosition{
    Parallel = 0,
    Positive = 1,
    Negative = 2
};

std::ostream& operator<<(std::ostream& ostream, const IntersectionType& type);

std::ostream& operator<<(std::ostream& ostream, const LineSegmentIntersection& type);

std::ostream& operator<<(std::ostream& ostream, const RelativePosition& type);

class Intersector{

public:
    Intersector();

    void setSegment1(const Vector2f& _r1, const Vector2f& _r2);
    void setSegment2(const Vector2f& _s1, const Vector2f& _s2);

    void setToleranceParallelism(double tolerance);
    void setToleranceOnVertex(float tolerance);

    Vector2f getIntersectionPoint();
    IntersectionType getIntersectionType();
    LineSegmentIntersection getLineSegmentIntersection();
    RelativePosition getRelativePosition();

    IntersectionType calculateIntersection(bool isFirstLine = false, bool isSecondLine = false);
    LineSegmentIntersection calculateLineSegmentIntersection();
    RelativePosition calculateRelativePosition();
    RelativePosition calculateRelativePosition(double tolerance);

private:
    bool pointIntersectionCalculated;
    bool intersectionCalculated;
    bool lineSegmentCalculated;
    bool relativePositionCalculated;

    double toleranceParallelism;
    float toleranceOnVertex;

    IntersectionType intersectionType;
    LineSegmentIntersection lineSegmentIntersection;
    RelativePosition relativePosition;
    Vector2f intersectionPoint;

    double determinant;
    double a1, b1, c1;
    double a2, b2, c2;
    Vector2f r1;
    Vector2f r2;
    Vector2f s1;
    Vector2f s2;

    void reset();

    void calculateLinesDeterminantIntersection();
};

#endif // INTERSECTOR_H
