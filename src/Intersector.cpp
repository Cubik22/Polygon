#include "Intersector.h"
#include "math.h"
#include "Logger.h"

Intersector::Intersector() : toleranceParallelism{1.0E-4}, toleranceOnVertex{1.0E-4}, intersectionPoint{0.0, 0.0} {
    reset();
}

void Intersector::setSegment1(const Vector2f &_r1, const Vector2f &_r2){
    reset();

    r1 = _r1;
    r2 = _r2;
}

void Intersector::setSegment2(const Vector2f &_s1, const Vector2f &_s2){
    reset();

    s1 = _s1;
    s2 = _s2;
}

void Intersector::setToleranceParallelism(double tolerance){
    if (tolerance > 0){
        toleranceParallelism = tolerance;
    } else{
        LOG(LogLevel::WARN) << "Tolerance Parallelism not set: tolerance cannot be negative";
    }
}

void Intersector::setToleranceOnVertex(float tolerance){
    if (tolerance > 0){
        toleranceOnVertex = tolerance;
    } else{
        LOG(LogLevel::WARN) << "Tolerance On Vertex not set: tolerance cannot be negative";
    }
}

Vector2f Intersector::getIntersectionPoint(){
    if (!pointIntersectionCalculated){
        calculateLinesDeterminantIntersection();
    }
    return intersectionPoint;
}

IntersectionType Intersector::getIntersectionType(){
    if (!intersectionCalculated){
        return calculateIntersection();
    }
    return intersectionType;
}

LineSegmentIntersection Intersector::getLineSegmentIntersection(){
    if (!lineSegmentCalculated){
        return calculateLineSegmentIntersection();
    }
    return lineSegmentIntersection;
}

RelativePosition Intersector::getRelativePosition(){
    if (!relativePositionCalculated){
        return calculateRelativePosition();
    }
    return relativePosition;
}

IntersectionType Intersector::calculateIntersection(bool isFirstLine, bool isSecondLine){
    intersectionCalculated = true;
    calculateLinesDeterminantIntersection();
    if (abs(determinant) > toleranceParallelism){
        double toleranceVertexSquared = toleranceOnVertex * toleranceOnVertex;
        Vector2f differencer1 = intersectionPoint - r1;
        Vector2f differencer2 = intersectionPoint - r2;
        Vector2f differences1 = intersectionPoint - s1;
        Vector2f differences2 = intersectionPoint - s2;

        bool firstOnSegment = differencer1.normSquared() < toleranceVertexSquared || differencer2.normSquared() < toleranceVertexSquared;
        bool secondOnSegment = differences1.normSquared() < toleranceVertexSquared || differences2.normSquared() < toleranceVertexSquared;

        if (!isFirstLine && isSecondLine){
            if (differencer1.x * differencer2.x + differencer1.y * differencer2.y > toleranceOnVertex){
                intersectionType = IntersectionType::OutsideSegment;
                return intersectionType;
            }
            if (firstOnSegment){
                intersectionType = IntersectionType::FirstOnVertex;
                return intersectionType;
            }
        } else if (!isSecondLine && isFirstLine){
            if (differences1.x * differences2.x + differences1.y * differences2.y > toleranceOnVertex){
                intersectionType = IntersectionType::OutsideSegment;
                return intersectionType;
            }
            if (secondOnSegment){
                intersectionType = IntersectionType::SecondOnVertex;
                return intersectionType;
            }
        } else if (!isFirstLine && !isSecondLine){
            if ((differences1.x * differences2.x + differences1.y * differences2.y > toleranceOnVertex) ||
                (differencer1.x * differencer2.x + differencer1.y * differencer2.y > toleranceOnVertex)){
                intersectionType = IntersectionType::OutsideSegment;
                return intersectionType;
            }
            if (firstOnSegment && secondOnSegment){
                intersectionType = IntersectionType::BothOnVertex;
                return intersectionType;
            } else if (firstOnSegment){
                intersectionType = IntersectionType::FirstOnVertex;
                return intersectionType;
            } else if (secondOnSegment){
                intersectionType = IntersectionType::SecondOnVertex;
                return intersectionType;
            }
        }
        intersectionType = IntersectionType::InsideSegment;
    } else{
        intersectionType = IntersectionType::Parallel;
    }
    return intersectionType;
}

LineSegmentIntersection Intersector::calculateLineSegmentIntersection(){
    lineSegmentCalculated = true;
    calculateLinesDeterminantIntersection();
    if (abs(determinant) > toleranceParallelism){
        double toleranceVertexSquared = toleranceOnVertex * toleranceOnVertex;

        Vector2f difference1 = intersectionPoint - s1;
        Vector2f difference2 = intersectionPoint - s2;

        bool firstOnSegment  = difference1.normSquared() < toleranceVertexSquared;
        bool secondOnSegment = difference2.normSquared() < toleranceVertexSquared;

        if (difference1.x * difference2.x + difference1.y * difference2.y > toleranceOnVertex){
            lineSegmentIntersection = LineSegmentIntersection::OutsideSegment;
            return lineSegmentIntersection;
        }

        if (firstOnSegment){
            lineSegmentIntersection = LineSegmentIntersection::FirstVertex;
            return lineSegmentIntersection;
        }
        if (secondOnSegment){
            lineSegmentIntersection = LineSegmentIntersection::SecondVertex;
            return lineSegmentIntersection;
        }

        lineSegmentIntersection = LineSegmentIntersection::InsideSegment;

    } else{
        lineSegmentIntersection = LineSegmentIntersection::Parallel;
    }
    return lineSegmentIntersection;
}

RelativePosition Intersector::calculateRelativePosition(){
    return calculateRelativePosition(toleranceParallelism);
}

RelativePosition Intersector::calculateRelativePosition(double tolerance){
    relativePositionCalculated = true;
    Vector2f v1 = r2 - r1;
    Vector2f v2 = s2 - s1;
    double dotProduct = v1.x * v2.y - v1.y * v2.x;
    if (dotProduct > tolerance){
        relativePosition = RelativePosition::Positive;
    } else if (dotProduct < -tolerance){
        relativePosition = RelativePosition::Negative;
    } else{
        relativePosition = RelativePosition::Parallel;
    }
    return relativePosition;
}

void Intersector::reset(){
    intersectionPoint.x = 0.0f;
    intersectionPoint.y = 0.0f;

    pointIntersectionCalculated = false;
    intersectionCalculated      = false;
    lineSegmentCalculated       = false;
    relativePositionCalculated  = false;
}

void Intersector::calculateLinesDeterminantIntersection(){
    pointIntersectionCalculated = true;
    a1 = r2.y - r1.y;
    b1 = r1.x - r2.x;
    c1 = a1 * r1.x + b1 * r1.y;
    a2 = s2.y - s1.y;
    b2 = s1.x - s2.x;
    c2 = a2 * s1.x + b2 * s1.y;

    determinant = a1 * b2 - a2 * b1;
    if (abs(determinant) > toleranceParallelism){
        intersectionPoint.x = (b2 * c1 - b1 * c2) / determinant;
        intersectionPoint.y = (a1 * c2 - a2 * c1) / determinant;
    } else{
        intersectionPoint.x = 0.0f;
        intersectionPoint.y = 0.0f;
    }
}

std::ostream& operator<<(std::ostream& ostream, const IntersectionType& type){
    if (type == IntersectionType::Parallel){
        ostream << "Parallel";
    } else if (type == IntersectionType::OutsideSegment){
        ostream << "Outside segment";
    } else if (type == IntersectionType::InsideSegment){
        ostream << "Inside segment";
    } else if (type == IntersectionType::FirstOnVertex){
        ostream << "First on vertex";
    } else if (type == IntersectionType::SecondOnVertex){
        ostream << "Second on vertex";
    } else if (type == IntersectionType::BothOnVertex){
        ostream << "Both on vertex";
    }
    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const LineSegmentIntersection& type){
    if (type == LineSegmentIntersection::Parallel){
        ostream << "Parallel";
    } else if (type == LineSegmentIntersection::OutsideSegment){
        ostream << "Outside segment";
    } else if (type == LineSegmentIntersection::InsideSegment){
        ostream << "Inside segment";
    } else if (type == LineSegmentIntersection::FirstVertex){
        ostream << "First vertex";
    } else if (type == LineSegmentIntersection::SecondVertex){
        ostream << "Second vertex";
    }
    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const RelativePosition& type){
    if (type == RelativePosition::Parallel){
        ostream << "Parallel";
    } else if (type == RelativePosition::Positive){
        ostream << "Positive";
    } else if (type == RelativePosition::Negative){
        ostream << "Negative";
    }
    return ostream;
}
