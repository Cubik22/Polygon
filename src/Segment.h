#ifndef SEGMENT_H
#define SEGMENT_H

#include "Vector2f.h"


class Segment{

public:
    Segment(const Vector2f& _s1, const Vector2f& _s2);

    const Vector2f getS1() const;

    const Vector2f getS2() const;

private:
    Vector2f s1;
    Vector2f s2;
};

#endif // SEGMENT_H
