#include "Intersector.h"
#include "math.h"


Intersector::Intersector() : intersectionCalculated{false}, relativePositionCalculated{false},
                             toleranceParallelism{1.0E-7}, toleranceOnVertex{1.0E-7}, intersectionPoint{0.0, 0.0} {}

void Intersector::setSegment1(const Vector2f &_r1, const Vector2f &_r2){
    r1 = _r1;
    r2 = _r2;
    intersectionCalculated = false;
    relativePositionCalculated = false;
}

void Intersector::setSegment2(const Vector2f &_s1, const Vector2f &_s2){
    s1 = _s1;
    s2 = _s2;
    intersectionCalculated = false;
    relativePositionCalculated = false;
}

void Intersector::setToleranceParallelism(double tolerance){
    if (tolerance > 0){
        toleranceParallelism = tolerance;
    } else{
        std::cout << "Tolerance Parallelism not set: tolerance cannot be negative\n";
    }
}

void Intersector::setToleranceOnVertex(float tolerance){
    if (tolerance > 0){
        toleranceOnVertex = tolerance;
    } else{
        std::cout << "Tolerance On Vertex not set: tolerance cannot be negative\n";
    }
}

Vector2f Intersector::getIntersectionPoint(){
    if (!intersectionCalculated){
        calculateIntersection();
    }
    return intersectionPoint;
}

IntersectionType Intersector::getIntersectionType(){
    if (!intersectionCalculated){
        return calculateIntersection();
    }
    return intersectionType;
}

RelativePosition Intersector::getRelativePosition(){
    if (!relativePositionCalculated){
        return calculateRelativePosition();
    }
    return relativePosition;
}

IntersectionType Intersector::calculateIntersection(bool isFirstLine, bool isSecondLine){
    intersectionCalculated = true;
    calculateLines();
    double determinant = a1 * b2 - a2 * b1;
    if (abs(determinant) > toleranceParallelism){
        intersectionPoint.x = (b2 * c1 - b1 * c2) / determinant;
        intersectionPoint.y = (a1 * c2 - a2 * c1) / determinant;
        double toleranceSquared = toleranceOnVertex * toleranceOnVertex;
        if (!isFirstLine){
            Vector2f difference1 = intersectionPoint - r1;
            Vector2f difference2 = intersectionPoint - r2;
            if (difference1.normSquared() < toleranceSquared || difference2.normSquared() < toleranceSquared){
                intersectionType = IntersectionType::OnVertex;
                return intersectionType;
            }
            if (difference1.x * difference2.x + difference1.y * difference2.y > toleranceOnVertex){
                intersectionType = IntersectionType::OutsideSegment;
                return intersectionType;
            }
        }
        if (!isSecondLine){
            Vector2f difference1 = intersectionPoint - s1;
            Vector2f difference2 = intersectionPoint - s2;
            if (difference1.normSquared() < toleranceSquared || difference2.normSquared() < toleranceSquared){
                intersectionType = IntersectionType::OnVertex;
                return intersectionType;
            }
            if (difference1.x * difference2.x + difference1.y * difference2.y > toleranceOnVertex){
                intersectionType = IntersectionType::OutsideSegment;
                return intersectionType;
            }
        }
        intersectionType = IntersectionType::InsideSegment;
    } else{
        intersectionType = IntersectionType::Parallel;
    }
    return intersectionType;
}

RelativePosition Intersector::calculateRelativePosition(){
    relativePositionCalculated = true;
    Vector2f v1 = r2 - r1;
    Vector2f v2 = s2 - s1;
    double dotProduct = v1.x * v2.y - v1.y * v2.x;
    if (dotProduct > toleranceParallelism){
        relativePosition = RelativePosition::Positive;
    } else if (dotProduct < -toleranceParallelism){
        relativePosition = RelativePosition::Negative;
    } else{
        relativePosition = RelativePosition::Parallel;
    }
    return relativePosition;
}

void Intersector::calculateLines(){
    a1 = r2.y - r1.y;
    b1 = r1.x - r2.x;
    c1 = a1 * r1.x + b1 * r1.y;
    a2 = s2.y - s1.y;
    b2 = s1.x - s2.x;
    c2 = a2 * s1.x + b2 * s1.y;
}

std::ostream& operator<<(std::ostream& ostrem, const IntersectionType& type){
    if (type == IntersectionType::Parallel){
        ostrem << "Parallel";
    } else if (type == IntersectionType::InsideSegment){
        ostrem << "Inside segment";
    } else if (type == IntersectionType::OutsideSegment){
        ostrem << "Outside segment";
    }
    return ostrem;
}

std::ostream& operator<<(std::ostream& ostrem, const RelativePosition& type){
    if (type == RelativePosition::Parallel){
        ostrem << "Parallel";
    } else if (type == RelativePosition::Positive){
        ostrem << "Positive";
    } else if (type == RelativePosition::Negative){
        ostrem << "Negative";
    }
    return ostrem;
}
