#include "Segment.h"

Segment::Segment(const Vector2f& _s1, const Vector2f& _s2) : s1{_s1}, s2{_s2} {}

const Vector2f Segment::getS1() const{
    return s1;
}

const Vector2f Segment::getS2() const{
    return s2;
}
