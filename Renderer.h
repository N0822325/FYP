#ifndef RENDERER
  #define RENDERER

  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>
  #include <GL/glx.h>
  #include <GL/glu.h>

  #include <X11/X.h>
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>

  #include <vector>
  #include <iostream>

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
    //Stride += sizeof(GL_FLOAT);
  }

  std::vector<VertexArrayNode> getList()
  {
    return List;
  }
  unsigned int getStride()
  {
    return Stride;
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
      glEnableVertexAttribArray(i);

      glVertexAttribPointer(i, node.count, node.type,
        node.normalised, layout.getStride(), (void*)offset);

      offset += node.count * sizeof(node.type);
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

  void setShader()
  {
    const char *vertexShaderSource =
      "#version 300 es\n"
      "layout (location = 0) in vec3 position;\n"
      "out vec4 vertexColour;\n"
      "void main()\n"
      "{\n"
      "     gl_Position = vec4(position, 1.0);\n"
      "     vertexColour = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n"
      "}\n";

    const char *fragmentShaderSource =
      "#version 300 es\n"
      "precision mediump float;\n"
      "in vec4 vertexColour;\n"
      "out vec4 colour;\n"
      "void main()\n"
      "{\n"
      "     colour = vertexColour;\n"
      "}\n";

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
  // unsigned int GetUniformLocation()
  // {
  //
  // }
};


class Renderer
{
public:

  void linkDisplay(Display* XDisplay, Window* XWindow, XVisualInfo* vi)
  {
    glContext = glXCreateContext(XDisplay, vi, NULL, GL_TRUE);
    glXMakeCurrent(XDisplay, *XWindow, glContext);
    glEnable(GL_DEPTH_TEST);
  }

  void draw(VertexArray& va, VertexBuffer& vb, ElementBuffer& eb, Shader* shader)
  {
    shader->bind();
    va.bind();
    eb.bind();
    glDrawElements(GL_TRIANGLES, eb.getCount(), GL_UNSIGNED_INT, nullptr);
  }


private:
  GLXContext glContext;
};


class objt
{
private:
  VertexArray va;
  VertexArrayList ls;
  VertexBuffer vb;
  ElementBuffer eb;
  Shader* shader;
  Renderer* renderer;

public:
  objt(float vertices[], unsigned int indices[], Shader* shader_, Renderer* renderer_)
    : vb(vertices, 4 * 3 * sizeof(float)), eb(indices, 6)
  {

    shader = shader_;
    renderer = renderer_;

    ls.push(3);
    va.addBuffer(vb, ls);

  }

  void draw()
  {
    renderer->draw(va, vb, eb, shader);
  }
};


#endif /* end of include guard: RENDERER */
