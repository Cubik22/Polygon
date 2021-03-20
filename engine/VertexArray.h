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

public:
    VertexBufferLayout();
    ~VertexBufferLayout();
    void pushElement(unsigned int _type, unsigned int _count, unsigned char _normalized = 0);
    const std::vector<VertexBufferElement>& getElements() const;
    unsigned int getStride() const;

private:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;
};

class VertexArray{

public:
    VertexArray();
    ~VertexArray();
    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
    void bind() const;
    void unbind() const;

private:
    unsigned int rendererID;
};

#endif // VERTEXARRAY_H
