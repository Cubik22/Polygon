#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

class VertexBuffer{
private:
    unsigned int rendererID;
public:
    VertexBuffer();
    ~VertexBuffer();
    void sendData(const void* data, unsigned int size, bool dynamic = false);
    void bind() const;
    void unbind() const;
};

#endif // VERTEXBUFFER_H
