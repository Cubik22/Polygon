#include "VertexBuffer.h"
#include "GL/glew.h"

VertexBuffer::VertexBuffer(){
    glGenBuffers(1, &rendererID);
}

VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1, &rendererID);
}

void VertexBuffer::sendData(const void *data, unsigned int size, bool dynamic){
    bind();
    if (dynamic){
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    } else{
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }
}

void VertexBuffer::bind() const{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void VertexBuffer::unbind() const{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
