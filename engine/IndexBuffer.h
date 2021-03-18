#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

class IndexBuffer{
private:
    unsigned int rendererID;
    unsigned int count;
public:
    IndexBuffer();
    ~IndexBuffer();
    //default copy constructor is ok
    void sendData(const unsigned int* data, unsigned int count, bool dynamic = false);
    void bind() const;
    void unbind() const;
    unsigned int getCount() const;
};

#endif // INDEXBUFFER_H
