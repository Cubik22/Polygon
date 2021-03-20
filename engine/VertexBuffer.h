#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H


class VertexBuffer{

public:
    // constructory initialize rendererID
    VertexBuffer();
    // destructor delete rendererID
    ~VertexBuffer();

    // sending data to buffer
    // data is const void pointer so you can send every data type
    // size is the size of data you are sending
    // last parameter specify how data are stored on the GPU
    // dynamic = false -> if you don't intend to change buffer data data often (default)
    // dynamic = true  -> if you plan on sending data to the buffer often
    void sendData(const void* data, unsigned int _size, bool dynamic = false);
    void bind() const;
    void unbind() const;
    unsigned int getSize() const;

private:
    // rendererID is for binding OpenGL ARRAY_BUFFER
    unsigned int rendererID;
    // size is the size of data in the buffer
    unsigned int size;
};

#endif // VERTEXBUFFER_H
