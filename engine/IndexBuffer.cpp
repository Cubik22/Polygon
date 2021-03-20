#include "IndexBuffer.h"
#include "GL/glew.h"


IndexBuffer::IndexBuffer() : count{0} {
    glGenBuffers(1, &rendererID);
}

IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1, &rendererID);
}

void IndexBuffer::sendData(const unsigned int* data, unsigned int _count, bool dynamic){
    count = _count;
    bind();
    if (dynamic){
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
    }
}

void IndexBuffer::bind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
}

void IndexBuffer::unbind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int IndexBuffer::getCount() const{
    return count;
}
