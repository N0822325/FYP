#ifndef RENDERER
  #define RENDERER

  #define PATH "FYP/"

  #include "Camera.h"

  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>
  #include <GL/glx.h>
  #include <GL/glu.h>

  #include <glm/glm.hpp>
  #include <glm/gtc/matrix_transform.hpp>
  #include <glm/gtc/type_ptr.hpp>


  #include <X11/X.h>
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>

  #define STB_IMAGE_IMPLEMENTATION
  #include "Libraries/stb_image.h"

  #include <vector>
  #include <iostream>
  #include <fstream>

class VertexBuffer
{
private:
  unsigned int BufferID;
public:
  VertexBuffer(void* data, unsigned int size)
  {
    glGenBuffers(1, &BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, BufferID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  }
  ~VertexBuffer()
  {
    glDeleteBuffers(1, &BufferID);
  }

  void bind()
  {
    glBindBuffer(GL_ARRAY_BUFFER, BufferID);
  }
  void unBind()
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
};

class ElementBuffer
{
private:
  unsigned int BufferID;
  unsigned int Count;
public:
  ElementBuffer(unsigned int* data, unsigned int count)
  {
    Count = count;
    glGenBuffers(1, &BufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
  }
  ~ElementBuffer()
  {
    glDeleteBuffers(1, &BufferID);
  }

  void bind()
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferID);
  }
  void unBind()
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  unsigned int getCount() { return Count; }
};

struct VertexArrayNode
{
  unsigned int type;
  unsigned int count;
  bool normalised;
};
class VertexArrayList
{
private:
  std::vector<VertexArrayNode> List;
  unsigned int Stride = 0;
public:
  VertexArrayList()
  {

  }
  ~VertexArrayList()
  {

  }

  void push(unsigned int count)
  {
    List.push_back({ GL_FLOAT, count, false });
    Stride += count;
    // Stride += sizeof(GL_FLOAT);
  }

  std::vector<VertexArrayNode> getList()
  {
    return List;
  }
  unsigned int getStride()
  {
    return Stride * sizeof(GL_FLOAT);
  }

};

class VertexArray
{
private:
  unsigned int ArrayID;
public:
  VertexArray()
  {
    glGenVertexArrays(1, &ArrayID);
  }
  ~VertexArray()
  {
    glDeleteVertexArrays(1, &ArrayID);
  }

  void addBuffer(VertexBuffer& vb, VertexArrayList& layout)
  {
    bind();
    vb.bind();
    const auto& e = layout.getList();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < e.size(); i++)
    {
      const auto& node = e[i];
// std::cout << "First: " << i << std::endl << "Second: " << node.count << std::endl << "Third: " << node.type << std::endl << "Fourth: " << node.normalised << std::endl << "Fith: " << layout.getStride() << std::endl << "Sixth: " << offset << std::endl << std::endl;

      glEnableVertexAttribArray(i);
      glVertexAttribPointer(
        i,
        node.count,
        node.type,
        node.normalised,
        layout.getStride(),               // Stride
        (void*)(offset*sizeof(node.type)  // Offset
      ));

      offset += node.count;
    }
  }

  void bind()
  {
    glBindVertexArray(ArrayID);
  }
  void unBind()
  {
    glBindVertexArray(0);
  }
};




class Shader
{
private:
  unsigned int ShaderID;
  const char* VertexShaderSource;
  const char* FragmentShaderSource;
public:
  Shader()
  {

  }
  ~Shader()
  {
    glDeleteProgram(ShaderID);
  }

  std::string readShaderFile(std::string filename)
  {
    std::ifstream File(filename);
    std::string line, text;
    while(std::getline(File, line))
    {
      text += line + "\n";
    }
    return text;
  }

  void setShader()
  {
    std::string pathString(PATH);

    std::string vShader = readShaderFile(pathString + "Vertex.shader");
    const char *vertexShaderSource = vShader.c_str();

    std::string fShader = readShaderFile(pathString + "Fragment.shader");
    const char *fragmentShaderSource = fShader.c_str();


    VertexShaderSource = vertexShaderSource;
    FragmentShaderSource = fragmentShaderSource;
    ShaderID = CreateShader();
  }

  void bind()
  {
    glUseProgram(ShaderID);
  }
  void unBind()
  {
    glUseProgram(0);
  }

  void setUniform4f(float v0, float v1, float v2, float v3)
  {
    int location = glGetUniformLocation(ShaderID, "u_Colour");
    glUniform4f(location, v0, v1, v2, v3);
  }
  void setUniformMat4f(std::string name, glm::mat4& matrix)
  {
    int location = glGetUniformLocation(ShaderID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
  }
  void setUniform1i(std::string name, int value)
  {
    int location = glGetUniformLocation(ShaderID, name.c_str());
    glUniform1i(location, value);
  }

private:
  unsigned int CompileShader(unsigned int type, const char* source)
  {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

// Program Compile Check //
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
    	GLint maxLength = 0;
    	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    	std::vector<GLchar> errorLog(maxLength);
    	glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

      for(int i = 0; i < maxLength; i++)
        { std::cout << errorLog[i]; } // Print Error
      std::cout << std::endl;
    }
// ~~~~~~~~~~~~ //

    return shader;
  }
  unsigned int CreateShader()
  {
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    unsigned vertexShader = CompileShader(GL_VERTEX_SHADER, VertexShaderSource);
    unsigned fragmentShader = CompileShader(GL_FRAGMENT_SHADER, FragmentShaderSource);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

// Shader Compile Error Check //
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
      { std::cout << "Shader Compiler Error" << std::endl; }
// ~~~~~~~~~~~~~~~~~~~~~~~~~~ //

    return shaderProgram;
  }

};


class Texture
{
private:
  Shader* shader;
  unsigned int renderID;
  std::string file;
  unsigned int texture;
  unsigned char* localBuffer;
  int width, height, bpp;


public:
  bool isTextured;
  float col[4] {1,1,1,1};

  Texture(const std::string& path)
    : renderID(0), file(path), localBuffer(nullptr), width(0), height(0), bpp(0)
  {
    if(path.empty()) { isTextured = false; return; }
    isTextured = true;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
       glGenerateMipmap(GL_TEXTURE_2D);

       // slot = totalTextures;
       // totalTextures++;
    }
    else
    {
       std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    enableBlend();
  }
  ~Texture() { glDeleteTextures(1, &texture); }

  void bind()
  {
    glActiveTexture(GL_TEXTURE0); //+ slot
    glBindTexture(GL_TEXTURE_2D, texture);
  }
  void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

  void enableBlend()
  {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  }

  Texture* get() { return this; }
  // int getSlot() { return slot; }

  int getWidth() { return width; }
  int getHeight() { return height; }
};



class Renderer
{
private:
  GLXContext glContext;
  Camera* camera;

public:

  void setCamera(Camera* newCam)
  {
    camera = newCam;
  }

  void linkDisplay(Display* XDisplay, Window* XWindow, XVisualInfo* vi)
  {
    glContext = glXCreateContext(XDisplay, vi, NULL, GL_TRUE);
    glXMakeCurrent(XDisplay, *XWindow, glContext);
    glEnable(GL_DEPTH_TEST);
  }

  void draw(VertexArray& va, VertexBuffer& vb, ElementBuffer& eb,
    Shader* shader, Texture* texture, glm::mat4 model, int drawOption = GL_TRIANGLES)
  {
    shader->bind();
    texture->bind();

    glm::mat4 projection = camera->getProjection();
    glm::mat4 view = camera->getCamera();

    glm::mat4 mvp = projection * view * model;
    shader->setUniformMat4f("transform", mvp);
    shader->setUniform1i("isTextured", texture->isTextured);
    shader->setUniform4f(texture->col[0], texture->col[1], texture->col[2], texture->col[3]);

    va.bind();
    eb.bind();
    glDrawElements(drawOption, eb.getCount(), GL_UNSIGNED_INT, nullptr);
  }

};



#endif /* end of include guard: RENDERER */
