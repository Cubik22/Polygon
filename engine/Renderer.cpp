#include "Renderer.h"
#include "GL/glew.h"
#include "Logger.h"
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

Renderer::Renderer() : shader(getVertexShader(), getFragmentShader()), textShader(getTextVertexShader(), getTextFragmentShader()) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initFreetype();
}

void Renderer::addShape(const Shape* shape){
    shapes.push_back(shape);
}

void Renderer::replaceShape(unsigned int position, const Shape* shape){
    if (position < shapes.size()){
        if (shapes[position]){
            delete shapes[position];
        } else{
            LOG(LogLevel::WARN) << "shape " << position << " was nullptr";
        }
        shapes[position] = shape;
    } else{
        addShape(shape);
    }
}

void Renderer::removeLastShape(){
    if (shapes.size() > 0){
        if (shapes[shapes.size() - 1]){
            delete shapes[shapes.size() - 1];
        } else{
            LOG(LogLevel::WARN) << "last shape " << shapes.size() - 1 << " was nullptr";
        }
        shapes.pop_back();
    }
}

void Renderer::removeAllShapes(){
    while (shapes.size() > 0){
        removeLastShape();
    }
}

void Renderer::drawShapes() const{
    shader.bind();
    for (unsigned int i = 0; i < shapes.size(); i++){
        const Shape* shape = shapes[i];
        if (!shape || shape == nullptr){
            LOG(LogLevel::DEBUG) << "shape " << i << " is nullptr";
            continue;
        }
        setPolygonColorFloat(shape->getRGB());
        shape->draw();
    }
}

void Renderer::clear() const{
    glClear(GL_COLOR_BUFFER_BIT);
}

unsigned int Renderer::getNumberShapes() const{
    return shapes.size();
}

void Renderer::drawText(const std::string& text, float x, float y, float scale, float r, float g, float b){
// activate corresponding render state
    textShader.bind();

    textShader.setUniform3f("textColor", r, g, b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.BearingX * scale/ ch.SizeX;
        float ypos = y - (ch.SizeY - ch.BearingY) * scale / ch.SizeY;

        float w = ch.SizeX * scale / ch.SizeX;
        float h = ch.SizeY * scale / ch.SizeY;

//        float xpos = x + ch.BearingX * scale;
//        float ypos = y - (ch.SizeY - ch.BearingY) * scale;

//        float w = ch.SizeX * scale;
//        float h = ch.SizeY * scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

//        std::cout << "vert: ";
//        for (unsigned int i = 0; i < 4; i++){
//            std::cout << vertices[0][i] << " ";
//        }
//        std::cout << "\n";

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale / ch.SizeX;
        // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Renderer::PRINT_INFO = true;

void Renderer::initGLEW(){
    if (glewInit() != GLEW_OK){
        //LoggerStatic::Error("problems while initializing Glew");
        LOG(LogLevel::ERROR) << "problems while initializing Glew";
        exit(-1);
    } else{
        //LoggerStatic::Message("Glew initialized correctly");
        LOG(LogLevel::INFO) << "Glew initialized correctly";
    }

    if (Renderer::PRINT_INFO){
        //LoggerStatic::Message((const std::string&)"VERSION: " + (const char*)glGetString(GL_VERSION));
        LOG(LogLevel::INFO) << "VERSION: " << glGetString(GL_VERSION);

        // Returns the vendor
        auto vendor = (const char*)glGetString(GL_VENDOR);
        // Returns a hint to the model
        auto model = (const char*)glGetString(GL_RENDERER);

        //LoggerStatic::Message((const std::string&)"VENDOR: " + vendor);
        LOG(LogLevel::INFO) << "VENDOR: " << vendor;
        //LoggerStatic::Message((const std::string&)"MODEL: " + model);
        LOG(LogLevel::INFO) << "MODEL: " << model;
    }

    // enable setting gl_PointSize in Vertex Shader
    glEnable(GL_PROGRAM_POINT_SIZE);

    //set the default line width to 3
    glLineWidth(3);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::initFreetype(){
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        exit(-1);
    }

    // find path to font
    std::string font_name = "fonts/Hack-Regular.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        exit(-1);
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        exit(-1);
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                face->glyph->bitmap.width, face->glyph->bitmap.rows,
                face->glyph->bitmap_left, face->glyph->bitmap_top,
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void Renderer::setLineWidth(unsigned int width){
    glLineWidth(width);
}

std::vector<float> Renderer::getNextColor(){
    Renderer::NumberColor++;
    return getLastColor();
}

std::vector<float> Renderer::getLastColor(){
    unsigned int colorsSize = Colors.size();
    return {Colors[(3 * Renderer::NumberColor)     % colorsSize] / 255.0f,
            Colors[(3 * Renderer::NumberColor + 1) % colorsSize] / 255.0f,
            Colors[(3 * Renderer::NumberColor + 2) % colorsSize] / 255.0f};
}

std::vector<float> Renderer::getColor(RendColor color){
    unsigned int colorsSize = Colors.size();
    return {Colors[(3 * (unsigned int)color)     % colorsSize] / 255.0f,
            Colors[(3 * (unsigned int)color + 1) % colorsSize] / 255.0f,
            Colors[(3 * (unsigned int)color + 2) % colorsSize] / 255.0f};
}

// PRIVATE

void Renderer::setPolygonColorFloat(float r, float g, float b) const{
    shader.bind();
    shader.setUniform4f("u_Color", r, g, b, 1.0f);
}

void Renderer::setPolygonColorFloat(std::vector<float> rgb) const{
    if (rgb.size() != 3){
        LOG(LogLevel::ERROR) << "float vector color should have size 3";
        return;
    }
    shader.bind();
    shader.setUniform4f("u_Color", rgb[0], rgb[1], rgb[2], 1.0f);
}

void Renderer::setPolygonColorRGB(unsigned int r, unsigned int g, unsigned int b) const{
    shader.bind();
    shader.setUniform4f("u_Color", r / 255.0, g / 255.0, b / 255.0, 1.0f);
}

void Renderer::setPolygonColorRGB(std::vector<unsigned int> rgb) const{
    if (rgb.size() != 3){
        LOG(LogLevel::ERROR) << "unsigned int vector color should have size 3";
        return;
    }
    shader.bind();
    shader.setUniform4f("u_Color", rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0, 1.0f);
}

unsigned int Renderer::NumberColor = 0;

std::vector<unsigned int> Renderer::Colors = {
    255,   0,   0,      //red
      0, 255,   0,      //lime
      0,   0, 255,      //blue
    255, 255,   0,      //yellow
      0, 255, 255,      //cyan
    255, 250, 205,      //lemon
    255,   0, 255,      //magenta
    139,   0,   0,      //dark red
      0, 128,   0,      //green
    138,  43, 226,      //blue violet
    255,  20, 147       //deep pink
};

const char *Renderer::getVertexShader(){
    static const char* vertexShader =
            "#version 330 core\n"
            "layout(location = 0) in vec2 position;\n"
            "void main(){\n"
            "   gl_PointSize = 5.0;\n"
            "   gl_Position = vec4(position, 0, 1);\n"
            "};";
    return vertexShader;
}

const char *Renderer::getFragmentShader(){
    static const char* fragmentShader =
            "#version 330 core\n"
            "layout(location = 0) out vec4 color;\n"
            "uniform vec4 u_Color;\n"
            "void main(){\n"
            "   color = u_Color;\n"
            "};";
    return fragmentShader;
}

const char *Renderer::getTextVertexShader(){
    static const char* textVertexShader =
            "#version 330 core\n"
            "layout (location = 0) in vec4 vertex;\n"
            "out vec2 TexCoords;\n"
            "void main(){\n"
            "    gl_Position = vec4(vertex.xy, 0.0, 1.0);\n"
            "    TexCoords = vertex.zw;\n"
            "};";
    return textVertexShader;
}

const char *Renderer::getTextFragmentShader(){
    static const char* textFragmentShader =
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "void main(){\n"
        "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "    color = vec4(textColor, 1.0) * sampled;\n"
        "};";
    return textFragmentShader;
}

