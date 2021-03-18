#ifndef VECTOR2F_H
#define VECTOR2F_H

class Vector2f{
public:
    float x;
    float y;
public:
    Vector2f() {}
    Vector2f(float _x, float _y);
    Vector2f(const Vector2f& copy);
    bool operator==(const Vector2f& equal) const;
    void operator=(const Vector2f& other);
    Vector2f operator+(const Vector2f& other) const;
    Vector2f operator-(const Vector2f& other) const;
    double normSquared() const;
};

#endif // VECTOR2F_H
