#ifndef INTERSECTOR_H
#define INTERSECTOR_H

class Point{
public:
    double x;
    double y;
public:
    Point() {}
    Point(double _x, double _y) : x{_x}, y{_y} {}
    Point(const Point& copy) : x{copy.x}, y{copy.y} {}
    bool operator==(const Point& equal) const{
        return x == equal.x && y == equal.y;
    }
    void operator=(const Point& other){
        x = other.x;
        y = other.y;
    }
};

enum class IntersectionType{
    Parallel = 0,
    OutsideSegment = 1,
    InsideSegment = 2
};

class Intersector{
private:
    bool calculated;
    double tolerance;
    IntersectionType intersectionType;
    Point intersectionPoint;
    double a1, b1, c1;
    double a2, b2, c2;
    Point r1;
    Point r2;
    Point s1;
    Point s2;
    void calculateLines(){
        a1 = r2.y - r1.y;
        b1 = r1.x - r2.x;
        c1 = a1 * r1.x + b1 * r1.y;
        a2 = s2.y - s1.y;
        b2 = s1.x - s2.x;
        c2 = a2 * s1.x + b2 * s1.y;
    }
public:
    Intersector() : calculated{false}, tolerance{1.0E-7}, intersectionPoint{0.0, 0.0} {}
    void setLine(const Point& _r1, const Point& _r2){
        r1 = _r1;
        r2 = _r2;
        calculated = false;
    }
    void setSegments(const Point& _s1, const Point& _s2){
        s1 = _s1;
        s2 = _s2;
        calculated = false;
    }
    void setTolerance(const double& _tolerance){
        tolerance = _tolerance;
    }
    Point getIntersectionPoint(){
        if (!calculated){
            calculateIntersection();
        }
        return intersectionPoint;
    }
    IntersectionType getIntersectionType(){
        if (!calculated){
            return calculateIntersection();
        }
        return intersectionType;
    }
    IntersectionType calculateIntersection(){
        calculated = true;
        calculateLines();
        double determinant = a1 * b2 - a2 * b1;
        if (determinant > tolerance || determinant < -tolerance){
            intersectionPoint.x = (b2 * c1 - b1 * c2) / determinant;
            intersectionPoint.y = (a1 * c2 - a2 * c1) / determinant;
            double xMax, yMax, xMin, yMin;
            if (s1.x > s2.x){
                xMax = s1.x;
                xMin = s2.x;
            } else{
                xMax = s2.x;
                xMin = s1.x;
            }
            if (s1.y > s2.y){
                yMax = s1.y;
                yMin = s2.y;
            } else{
                yMax = s2.y;
                yMin = s1.y;
            }
            if (intersectionPoint.x <= xMax && intersectionPoint.x >= xMin && intersectionPoint.y <= yMax && intersectionPoint.y >= yMin){
                intersectionType = IntersectionType::InsideSegment;
            } else{
                intersectionType = IntersectionType::OutsideSegment;
            }
        } else{
            intersectionType = IntersectionType::Parallel;
        }
        return intersectionType;
    }
};

#endif // INTERSECTOR_H
