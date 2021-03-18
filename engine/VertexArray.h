#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <vector>
#include "VertexBuffer.h"

struct VertexBufferElement{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type);
};

class VertexBufferLayout{
private:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;
public:
    VertexBufferLayout();
    ~VertexBufferLayout();
    void pushElement(unsigned int _type, unsigned int _count, unsigned char _normalized = 0);
    const std::vector<VertexBufferElement>& getElements() const;
    unsigned int getStride() const;
};

class VertexArray{
private:
    unsigned int rendererID;
public:
    VertexArray();
    ~VertexArray();
    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
    void bind() const;
    void unbind() const;
};

#endif // VERTEXARRAY_H
