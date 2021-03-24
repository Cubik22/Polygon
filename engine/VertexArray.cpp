#include "VertexArray.h"
#include "GL/glew.h"

VertexBufferLayout::VertexBufferLayout() : stride{0} {}

VertexBufferLayout::~VertexBufferLayout() {}

unsigned int VertexBufferElement::getSizeOfType(unsigned int type){
    if (type == GL_DOUBLE){
        return 8;
    } else if (type == GL_FLOAT){
        return 4;
    } else if (type == GL_UNSIGNED_INT || type == GL_INT){
        return 4;
    } else if (type == GL_UNSIGNED_BYTE){
        return 1;
    } else {
        return -1;
    }
}

void VertexBufferLayout::pushElement(unsigned int _type, unsigned int _count, unsigned char _normalized){
    elements.push_back({_type, _count, _normalized});
}

const std::vector<VertexBufferElement>& VertexBufferLayout::getElements() const{
    return elements;
}

unsigned int VertexBufferLayout::getStride() const{
    return stride;
}

VertexArray::VertexArray(){
    glGenVertexArrays(1, &rendererID);
}

VertexArray::~VertexArray(){
    glDeleteVertexArrays(1, &rendererID);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout){
    bind();
    vb.bind();
    const std::vector<VertexBufferElement>& elements = layout.getElements();
    unsigned long long int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++){
        const VertexBufferElement& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void *)offset);
        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }
}

void VertexArray::bind() const{
    glBindVertexArray(rendererID);
}

void VertexArray::unbind() const{
    glBindVertexArray(0);
}


