#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H


class IndexBuffer{

public:
    // constructory initialize rendererID
    IndexBuffer();
    // destructor delete rendererID
    ~IndexBuffer();
    //default copy constructor is ok

    // sending data to buffer
    // count is the number of elements you are sending (only support unsigned int)
    // size is count * sizeof(unsigned int)
    // last parameter specify how data are stored on the GPU
    // dynamic = false -> if you don't intend to change buffer data data often (default)
    // dynamic = true  -> if you plan on sending data to the buffer often
    void sendData(const unsigned int* data, unsigned int _count, bool dynamic = false);
    void bind() const;
    void unbind() const;
    unsigned int getCount() const;

private:
    // rendererID is for binding OpenGL ELEMENT_ARRAY_BUFFER
    unsigned int rendererID;
    // count is the number of elements in the buffer (given that they are always unsigned int)
    // size is count * sizeof(unsigned int)
    unsigned int count;
};

#endif // INDEXBUFFER_H
