#include "Vector2f.h"
#include <math.h>

Vector2f::Vector2f() {}

Vector2f::Vector2f(float _x, float _y) : x{_x}, y{_y} {}

Vector2f::Vector2f(const Vector2f &copy) : x{copy.x}, y{copy.y} {}

bool Vector2f::operator==(const Vector2f &equal) const{
    return x == equal.x && y == equal.y;
}

void Vector2f::operator=(const Vector2f &other){
    x = other.x;
    y = other.y;
}

Vector2f Vector2f::operator+(const Vector2f &other) const{
    return {x + other.x, y + other.y};
}

Vector2f Vector2f::operator-(const Vector2f &other) const{
    return {x - other.x, y - other.y};
}

double Vector2f::dot(const Vector2f& other) const{
    return x * other.x + y * other.y;
}

double Vector2f::cross(const Vector2f& other) const{
    return x * other.y - y * other.x;
}

double Vector2f::normSquared() const{
    return x * x + y * y;
}

const double Vector2f::TOLERANCE = 1.0E-7;

bool Vector2f::AreDoublesEqual(double a, double b, double tolerance){
    return abs(a - b) < tolerance;
}

std::ostream& operator<< (std::ostream& ostrem, const Vector2f& point){
    return ostrem << "x: " << point.x << " y: " << point.y;
}
