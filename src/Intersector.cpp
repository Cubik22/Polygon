#include "Intersector.h"
#include "math.h"

void Intersector::calculateLines(){
    a1 = r2.y - r1.y;
    b1 = r1.x - r2.x;
    c1 = a1 * r1.x + b1 * r1.y;
    a2 = s2.y - s1.y;
    b2 = s1.x - s2.x;
    c2 = a2 * s1.x + b2 * s1.y;
}

Intersector::Intersector() : calculated{false}, toleranceParallelism{1.0E-5}, toleranceOnVertex{1.0E-4}, intersectionPoint{0.0, 0.0} {}

void Intersector::setSegment1(const Vector2f &_r1, const Vector2f &_r2){
    r1 = _r1;
    r2 = _r2;
    calculated = false;
}

void Intersector::setSegment2(const Vector2f &_s1, const Vector2f &_s2){
    s1 = _s1;
    s2 = _s2;
    calculated = false;
}

void Intersector::setToleranceParallelism(double tolerance){
    if (tolerance > 0){
        toleranceParallelism = tolerance;
    }
}

void Intersector::setToleranceOnVertex(float tolerance){
    if (tolerance > 0){
        toleranceOnVertex = tolerance;
    }
}

Vector2f Intersector::getIntersectionPoint(){
    if (!calculated){
        calculateIntersection();
    }
    return intersectionPoint;
}

IntersectionType Intersector::getIntersectionType(){
    if (!calculated){
        return calculateIntersection();
    }
    return intersectionType;
}

IntersectionType Intersector::calculateIntersection(bool isFirstLine, bool isSecondLine){
    calculated = true;
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
