#ifndef RENDERER_H
#define RENDERER_H

#include "Shape.h"
#include <vector>
#include <map>


enum class RendColor{
    Red         = 0,
    Lime        = 1,
    Blue        = 2,
    Yellow      = 3,
    Cyan        = 4,
    Lemon       = 5,
    Magenta     = 6,
    RedDark     = 7,
    Green       = 8,
    BlueViolet  = 9,
    PinkDeep    = 10
};

// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    unsigned int SizeX;  // Size of glyph
    unsigned int SizeY;
    int BearingX;   // Offset from baseline to left/top of glyph
    int BearingY;
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class Renderer{

public:
    Renderer();

    Renderer(const Renderer&)                 = delete;
    Renderer(Renderer&&) noexcept             = delete;
    Renderer& operator=(const Renderer&)      = delete;
    Renderer& operator=(Renderer&&) noexcept  = delete;

    void addShape(const Shape* shape);
    void replaceShape(unsigned int position, const Shape* shape);
    void removeLastShape();
    void removeAllShapes();
    void drawShapes() const;
    void clear() const;

    unsigned int getNumberShapes() const;

    void drawText(const std::string& text, float x, float y, float scale, float r = 1, float g = 1, float b = 1);

    static bool PRINT_INFO;

    static void initGLEW();

    static void setLineWidth(unsigned int width);

    static std::vector<float> getNextColor();
    static std::vector<float> getLastColor();

    static std::vector<float> getColor(RendColor color);

private:
    Shader shader;
    Shader textShader;

    unsigned int VAO;
    unsigned int VBO;

    std::map<char, Character> Characters;

    std::vector<const Shape*> shapes;

    void initFreetype();

    void setPolygonColorFloat(float r, float g, float b) const;
    void setPolygonColorFloat(std::vector<float> rgb) const;
    void setPolygonColorRGB(unsigned int r, unsigned int g, unsigned int b) const;
    void setPolygonColorRGB(std::vector<unsigned int> rgb) const;

    static unsigned int NumberColor;
    static std::vector<unsigned int> Colors;

    static const char* getVertexShader();
    static const char* getFragmentShader();

    static const char* getTextVertexShader();
    static const char* getTextFragmentShader();
};

#endif // RENDERER_H
