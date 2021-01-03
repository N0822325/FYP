#ifndef ENGINE
  #define ENGINE

  #define GL_GLEXT_PROTOTYPES

  #if defined(__linux__)
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glu.h>

    #include <X11/X.h>
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
  #endif

  #if defined(_WIN32)
    //WHOMEGALUL
  #endif

  #include "Logger.h"
  #include "Renderer.h"
  #include <iostream>
  #include <string>
  #include <thread>
  #include <unistd.h>
  #include <time.h>

//---------------------------------------------------------//

class Engine
{

public:

  //Engine(){};
  // Engine(uint16_t width, uint16_t height);
  // Engine(uint16_t width, uint16_t height, Logger* logs);
  // ~Engine();


protected:
  void construct(uint16_t width, uint16_t height, std::string name, Logger* logs);
  void start();
  virtual void userInitialse(){};
  virtual void userUpdate(){};


public:
  objt* Quad(int x1, int y1, int width, int height, int col, bool full);

  // void Draw(int x, int y, int col);
  // void DrawLine(int x1, int y1, int x2, int y2, int col);
  //void DrawRect(int x1, int y1, int x2, int y2, int col, bool full);
  //void DrawTri(int x, int y, int w, int h, int col, bool full);
  // void DrawCircle(int x, int y, int r, int col, bool full);

  //void DrawSprite(int x, int y, Sprite s, xScale xs, yScale ys);
  //void DrawString(int x, int y, std::string text, int col, xScale xs, yScale ys)

  // void DrawLine(int x1, int y1, int z1, int x2, int y2, int z2, int col);
  // void DrawCube(int x, int y, int w, int h, int d, int col, bool full);
  // void DrawPyramid(int x, int y, int w, int h, int col, bool full);
  // void DrawSphere(int x, int y, int r, int col);


  // int getKeys();
  int* getMousePos(XEvent* events);
  // int getMouseState();


private:
  void Initialse();
  void MainThread();
  void CoreUpdate();
  void Draw();
  void EventHandler();

  void InitWindow(uint16_t width, uint16_t height);
  void CloseWindow();
  //void UpdateWindow(){};
  //void ClearWindow(){};


  void CalcFrameTime();
  void DisplayFPSThread();

  Logger* Logs;

  std::string WindowName;
  int WindowWidth;
  int WindowHeight;

  #if defined(__linux__)
    Display* XDisplay;
    Window XWindow;
    Window XRootWindow;
    int XWindowNumber;

    XVisualInfo *vi;
    XWindowAttributes gwa;
    XSetWindowAttributes swa;
  #endif

  //GL
  Renderer renderer;
  Shader shader;
  void InitShaders();
  Colormap cmap;
  GLXContext glContext;
  GLint attributes[5] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

  std::vector<objt*> ObjectList;
  //

  bool Initialised = false;

  float FPSLimit = 1.0/60.0; //---1/FPS---//
  float timeCounter = 0;
  float elapsedTime = 0;
  std::chrono::time_point<std::chrono::system_clock> TimePoint1, TimePoint2;

  int MousePos[2] = {0,0};

};

//---------------------------------------------------------//



void Engine::construct(uint16_t width, uint16_t height, std::string name = "", Logger* logs = nullptr)
{
  WindowWidth = width;
  WindowHeight = height;
  WindowName = name;
  Logs = logs;
}
void Engine::start()
{

  XInitThreads();

  std::thread main = std::thread
    (&Engine::Initialse, this);


  std::thread fps = std::thread
    (&Engine::DisplayFPSThread, this);

  main.join();
  fps.join();

}
void Engine::Initialse()
{
  TimePoint1 = std::chrono::system_clock::now();
  TimePoint2 = std::chrono::system_clock::now();
  srand(time(NULL));

  InitWindow(WindowWidth, WindowHeight);
  InitShaders();

  userInitialse();
  MainThread();
}


void Engine::InitWindow(uint16_t width, uint16_t height)
{
  #if defined(__linux__)
    XDisplay = XOpenDisplay(NULL);
    XRootWindow = DefaultRootWindow(XDisplay);
    XWindowNumber = DefaultScreen(XDisplay);

    vi = glXChooseVisual(XDisplay, 0, attributes);

    cmap = XCreateColormap(XDisplay, XRootWindow, vi->visual, AllocNone);

    swa.colormap = cmap;
    swa.event_mask = ExposureMask | StructureNotifyMask |
                      KeyPressMask | ButtonPressMask | PointerMotionMask;

    XWindow = XCreateWindow
      (XDisplay, XRootWindow, 0, 0, WindowWidth, WindowHeight, 0,
        vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

    //XMapRaised(XDisplay,XWindow);
    XMapWindow(XDisplay,XWindow);
    XStoreName(XDisplay, XWindow, WindowName.c_str());
    //XFlush(XDisplay);

    renderer.linkDisplay(XDisplay, &XWindow, vi);

    Initialised = true;
  #endif


  #if defined(_WIN32)
    //WHOMEGALUL
  #endif

  //Initialised = true;
}

void Engine::InitShaders()
{

  shader.setShader();

}

void Engine::CloseWindow()
{

  #if defined(__linux__)
    XDestroyWindow(XDisplay,XWindow);
    XCloseDisplay(XDisplay);
  #endif

  Initialised = false;
}



//---------------------------------------------------------//


void Engine::MainThread()
{


  while(true)
  {
    CalcFrameTime();

    if (timeCounter >= FPSLimit) // Enters if enough time has passed (FPS Limt)
    {

      CoreUpdate();

      timeCounter = 0;
    }
    else
      { timeCounter += elapsedTime; }

  }

}

void Engine::CoreUpdate()
{

  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  EventHandler();
  userUpdate();

  Draw();
  //
  glXSwapBuffers(XDisplay, XWindow);
}

void Engine::Draw()
{

  for (objt* o : ObjectList)
  {
    o->draw();
  }

}


void Engine::EventHandler()
{
  XEvent events;
  while(XPending(XDisplay))
  {
    XNextEvent(XDisplay, &events);

    if (events.type == Expose) // On Window Init
    {

    }
    if (events.type == ConfigureNotify) // On Window Property Change - i.e. resize
    {
      XGetWindowAttributes(XDisplay, XWindow, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
    }


    if (events.type == KeyPress)
    {
      // Key Pressed
    }
    else if (events.type == KeyRelease)
    {
      // Key Released
    }
    else if(events.type == MotionNotify)
    {
      getMousePos(&events);
    }
  }
}

int* Engine::getMousePos(XEvent* events)
{
  MousePos[0] = events->xbutton.x;
  MousePos[1] = events->xbutton.y;

  return MousePos;
}



objt* Engine::Quad(int x1_, int y1_, int width, int height, int col, bool full)
{
  int windowWidth = WindowWidth/2;
  int windowHeight = WindowHeight/2;

  float x1 = ((float)(x1_)/windowWidth)-1;
  float y1 = ((float)(y1_)/windowHeight)-1;
  float x2 = ((float)(x1_+width)/windowWidth)-1;
  float y2 = ((float)(y1_+height)/windowHeight)-1;

  float vertices[] = {
    x2, y1, 0.0f, // top right         3------0
    x2, y2, 0.0f, // bottom right      |      |
    x1, y2, 0.0f, // bottom left       |      |
    x1, y1, 0.0f // top left           2------1
  };
  unsigned int indices[] = {
    0, 1, 2, // first triangle
    2, 3, 0 // second triangle
  };

  objt* aaaaaa = new objt(vertices, indices, &shader, &renderer);
  ObjectList.push_back(aaaaaa);

  return ObjectList.back();
}

// void Engine::DrawTri(int x, int y, int w, int h, int col, bool full)
// {
//
//   float verticies[] = {
//     -0.5f, -0.5f, 0.0f,
//     0.5f, -0.5f, 0.0f,
//     0.0f, 0.5f, 0.0f
//   };
//   shader.bind();
//
//   unsigned int VertexBuffer;
//
//   glGenBuffers(1, &VertexBuffer);
//   glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
//
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//   glEnableVertexAttribArray(0);
//
// }


void Engine::CalcFrameTime()
{
  TimePoint2 = std::chrono::system_clock::now();
  std::chrono::duration<float> a = TimePoint2 - TimePoint1;
  elapsedTime = a.count();
  TimePoint1 = TimePoint2;
}

void Engine::DisplayFPSThread()
{
  while(!Initialised) {}
  while(Initialised){
    sleep(1);
    int FPS = (1 / (elapsedTime + FPSLimit)) + 1;

    std::string title = WindowName + " | FPS: " + std::to_string(FPS);
    XStoreName(XDisplay, XWindow, title.c_str());
  }
}

#endif /* end of include guard: ENGINE */
