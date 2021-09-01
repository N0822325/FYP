#ifndef ENGINE
  #define ENGINE

  #include "Renderer.h"
  #include "Objects.h"

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
    // Windows API
  #endif

  #if defined NETWORKING
    #include "Networking.h"
  #endif

  #if defined SOUND
    #include "Libraries/Sound.h"
  #endif

  #include <iostream>
  #include <string>
  #include <thread>
  #include <unistd.h>
  #include <time.h>


//---------------------------------------------------------//

class Engine
{

protected:
  void construct(uint16_t width, uint16_t height, std::string name, unsigned int FPS);
  void start();

  virtual void userInitialse(){};
  virtual void userUpdate(float deltaTime){};
  virtual void userInput(XEvent events){};
  virtual void userDraw(){};

  void setBackgroundColour(float colour[4]);
  void setCamera(Camera* newCam);

  Object* Line(int x1_, int y1_, int z1_, int width, int height, int depth, float *col);
  Object* Plane(int x1, int y1, int z1, int width, int height, float* col, Texture* texture);
  Object* Cube(int x1, int y1, int z1, int width, int height, int depth, float* col, Texture* texture);
  // Object* Sphere();

  // int getKeys();
  int* getMousePos(XEvent* events);
  // int getMouseState();

#if defined SOUND
  void addSound(std::string file) {
    soundEngine.addSound(file);
  }
#endif

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

  std::string WindowName;
  int WindowWidth;
  int WindowHeight;
  float bgColour[3] = { 0,0,0 };

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
  //

  std::vector<Object*> ObjectList;

  bool Initialised = false;

  float FPSLimit = 1.0/60.0; //---1/FPS---//
  float timeCounter = 0;
  float elapsedTime = 0;
  std::chrono::time_point<std::chrono::system_clock> TimePoint1, TimePoint2;

  int MousePos[2] = {0,0};


  #if defined SOUND
    SoundEngine soundEngine;
  #endif

};

//---------------------------------------------------------//



void Engine::construct(uint16_t width, uint16_t height, std::string name = "", unsigned int FPS = 60)
{
  WindowWidth = width;
  WindowHeight = height;
  WindowName = name;

  OrthCamera* defaultCam = new OrthCamera();
  defaultCam->setProjection(0.0f,width,0.0f,height,-10.0f,10.0f);
  defaultCam->setCamera(glm::vec3(0,0,0));
  setCamera(defaultCam);

  if (FPS == 0) { FPS = UINT_MAX; }
  FPSLimit = 1.0/(float)FPS;
}
void Engine::start()
{

  XInitThreads();

  std::thread main = std::thread
    (&Engine::Initialse, this);

  std::thread fps = std::thread
    (&Engine::DisplayFPSThread, this);

#if defined SOUND
  std::thread sound = std::thread
    (&SoundEngine::soundLoop, &soundEngine);
#endif

  main.join();
  fps.join();

#if defined SOUND
  sound.join();
#endif

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
                      KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
                      | PointerMotionMask;

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
srand( time( NULL ) );

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

  // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

  glClearColor( bgColour[0], bgColour[1], bgColour[2], 1.0f );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  EventHandler();
  userUpdate(timeCounter);

  Draw();

  glXSwapBuffers(XDisplay, XWindow);

}

void Engine::Draw()
{
  userDraw();
}


void Engine::EventHandler()
{
  XEvent events;

  while(XPending(XDisplay))
  {
    XNextEvent(XDisplay, &events);

    if (events.type == Expose) // On Window Init
    { }
    if (events.type == ConfigureNotify) // On Window Property Change - i.e. resize
    {
      XGetWindowAttributes(XDisplay, XWindow, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
    }
    if(events.type == MotionNotify)
    {
      getMousePos(&events);
    }


    userInput(events);
  }

}

int* Engine::getMousePos(XEvent* events)
{
  MousePos[0] = events->xbutton.x;
  MousePos[1] = gwa.height - events->xbutton.y;

  return MousePos;
}


void Engine::setBackgroundColour(float colour[3])
{
  bgColour[0] = colour[0];
  bgColour[1] = colour[1];
  bgColour[2] = colour[2];
}

void Engine::setCamera(Camera* newCam)
{
  renderer.setCamera(newCam);
}


Object* Engine::Line(int x1_, int y1_, int z1_, int width, int height, int depth, float *col)
{
  float r = col[0];
  float g = col[1];
  float b = col[2];
  float a = col[3];

  float x1, x2, y1, y2, z1, z2;
  int largest = width > height ? width : height;
  largest = largest > depth ? largest : depth;

  x1 = -(float)width/largest; x2 = (float)width/largest;
  y1 = -(float)height/largest; y2 = (float)height/largest;
  z1 = -(float)depth/largest; z2 = (float)depth/largest;

  // x1 = -(float)width/largest; x2 = (float)width/largest;
  // y1 = -(float)height/largest; y2 = (float)height/largest;

  float vertices[] = {
//  Position      Colour        Texture Coords
    x1, y1, z1,   r, g, b, a,   1.0f, 1.0f,
    x2, y2, z2,   r, g, b, a,   0.0f, 0.0f
  };
  unsigned int indices[] = {
      0, 1, 2, // first triangle
      2, 3, 0, // second triangle
  };


  LineObj* aaaaaa = new LineObj(vertices, sizeof(vertices), indices, &shader, &renderer, x2, y2, z2 );
                                // glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2));

  aaaaaa->setTranslation(x1_,y1_,z1_);
  aaaaaa->setScale(largest/2);
  ObjectList.push_back(aaaaaa);

  return ObjectList.back();
}


Object* Engine::Plane(int x1_, int y1_, int z1_, int width, int height, float *col, Texture* texture = nullptr)
{
  float r = col[0];
  float g = col[1];
  float b = col[2];
  float a = col[3];

  float x1, x2, y1, y2;
  int largest = width > height ? width : height;

  x1 = -(float)width/largest; x2 = (float)width/largest;
  y1 = -(float)height/largest; y2 = (float)height/largest;

  float vertices[] = {
//  Position      Colour        Texture Coords
    x2, y1, 0.0f,   r, g, b, a,   1.0f, 1.0f,
    x2, y2, 0.0f,   r, g, b, a,   1.0f, 0.0f,
    x1, y2, 0.0f,   r, g, b, a,   0.0f, 0.0f,
    x1, y1, 0.0f,   r, g, b, a,   0.0f, 1.0f
  };
  unsigned int indices[] = {
      0, 1, 2, // first triangle
      2, 3, 0, // second triangle
  };

  if(texture == nullptr) { texture = new Texture(""); }
  PlaneObj* aaaaaa = new PlaneObj(vertices, sizeof(vertices), indices, &shader, &renderer, texture, x2, y2);
  aaaaaa->setTranslation(x1_,y1_,z1_);
  aaaaaa->setScale(largest/2);
  ObjectList.push_back(aaaaaa);

  return ObjectList.back();
}


Object* Engine::Cube(int x1_, int y1_, int z1_, int width, int height, int depth, float* col, Texture* texture = nullptr)
{
  float r = col[0];
  float g = col[1];
  float b = col[2];
  float a = col[3];

  float x1, x2, y1, y2, z1, z2;
  int largest = width > height ? width : height;
  largest = largest > depth ? largest : depth;

  x1 = -(float)width/largest; x2 = (float)width/largest;
  y1 = -(float)height/largest; y2 = (float)height/largest;
  z1 = -(float)depth/largest; z2 = (float)depth/largest;

  float vertices[] = {
//  Position        Colour        Texture Coords

// Front
    x2, y1, z2,   r, g, b, a,   1.0f, 1.0f,
    x2, y2, z2,   r, g, b, a,   1.0f, 0.0f,
    x1, y2, z2,   r, g, b, a,   0.0f, 0.0f,
    x1, y1, z2,   r, g, b, a,   0.0f, 1.0f,

// Back
    x1, y1, z1,   r, g, b, a,   1.0f, 1.0f,
    x1, y2, z1,   r, g, b, a,   1.0f, 0.0f,
    x2, y2, z1,   r, g, b, a,   0.0f, 0.0f,
    x2, y1, z1,   r, g, b, a,   0.0f, 1.0f,

// Right Side
    x2, y1, z1,   r, g, b, a,   1.0f, 1.0f,
    x2, y2, z1,   r, g, b, a,   1.0f, 0.0f,
    x2, y2, z2,   r, g, b, a,   0.0f, 0.0f,
    x2, y1, z2,   r, g, b, a,   0.0f, 1.0f,

// Left Side
    x1, y1, z2,   r, g, b, a,   1.0f, 1.0f,
    x1, y2, z2,   r, g, b, a,   1.0f, 0.0f,
    x1, y2, z1,   r, g, b, a,   0.0f, 0.0f,
    x1, y1, z1,   r, g, b, a,   0.0f, 1.0f,

// Bottom Side
    x2, y1, z1,   r, g, b, a,   1.0f, 1.0f,
    x1, y1, z1,   r, g, b, a,   0.0f, 1.0f,
    x1, y1, z2,   r, g, b, a,   0.0f, 0.0f,
    x2, y1, z2,   r, g, b, a,   1.0f, 0.0f,

// Top Side
    x2, y2, z2,   r, g, b, a,   1.0f, 1.0f,
    x1, y2, z2,   r, g, b, a,   0.0f, 1.0f,
    x1, y2, z1,   r, g, b, a,   0.0f, 0.0f,
    x2, y2, z1,   r, g, b, a,   1.0f, 0.0f

  };

  unsigned int indices[] = {
// front
      0, 1, 2, // first triangle
      2, 3, 0, // second triangle
// back
      4, 5, 6,
      6, 7, 4,
// right
      8, 9, 10,
      10, 11, 8,
// left
      12, 13, 14,
      14, 15, 12,
// bottom
      16, 17, 18,
      18, 19, 16,
// top
      20, 21, 22,
      22, 23, 20
  };

  if(texture == nullptr) { texture = new Texture(""); }
  CubeObj* aaaaaa = new CubeObj(vertices, sizeof(vertices), indices, &shader, &renderer, texture, x2, y2, z2);
  aaaaaa->setTranslation(x1_,y1_,z1_);
  aaaaaa->setScale(largest/2);
  ObjectList.push_back(aaaaaa);

  return ObjectList.back();
}


// Object* Engine::Sphere()
// {
//   float r = 0.0f;
//   float g = 1.0f;
//   float b = 1.0f;
//   float a = 1.0f;
//
//   int lats = 40;
//   int longs = 40;
//   int i, j;
//   std::vector<float> vertices;
//   std::vector<unsigned int> indices;
//   int indicator = 0;
//   for(i = 0; i <= lats; i++) {
//       double lat0 = glm::pi<double>() * (-0.5 + (double) (i - 1) / lats);
//      double z0  = sin(lat0);
//      double zr0 =  cos(lat0);
//
//      double lat1 = glm::pi<double>() * (-0.5 + (double) i / lats);
//      double z1 = sin(lat1);
//      double zr1 = cos(lat1);
//
//      for(j = 0; j <= longs; j++) {
//          double lng = 2 * glm::pi<double>() * (double) (j - 1) / longs;
//          double x = cos(lng);
//          double y = sin(lng);
//
//          vertices.push_back(x * zr0 * 1000);
//          vertices.push_back(y * zr0 * 1000);
//          vertices.push_back(z0 * 1000);
//          vertices.push_back(r);
//          vertices.push_back(g);
//          vertices.push_back(b);
//          vertices.push_back(a);
//          vertices.push_back(1.0f);
//          vertices.push_back(0.0f);
//
//          indices.push_back(indicator);
//          indicator++;
//
//          vertices.push_back(x * zr1 * 1000);
//          vertices.push_back(y * zr1 * 1000);
//          vertices.push_back(z1 * 1000);
//          vertices.push_back(r);
//          vertices.push_back(g);
//          vertices.push_back(b);
//          vertices.push_back(a);
//          vertices.push_back(1.0f);
//          vertices.push_back(0.0f);
//          indices.push_back(indicator);
//          indicator++;
//      }
//      indices.push_back(GL_PRIMITIVE_RESTART_FIXED_INDEX);
//    }
//
//    float* verts = &vertices[0];
//    unsigned int* indices2 = &indices[0];
//
//    for (int i = 0; i < 100; i++)
//    {
//      std::cout << verts[i] << std::endl;
//    }
//
//    SphereObj* aaaaaa = new SphereObj(verts, sizeof(verts), indices2, &shader, &renderer, new Texture(""), 1.0f);
//    ObjectList.push_back(aaaaaa);
//    aaaaaa->setScale(100000);
//
//    return ObjectList.back();
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
    int FPS = (1 / (elapsedTime + FPSLimit)) + 1;

    std::string title = WindowName + " | FPS: " + std::to_string(FPS);
    XStoreName(XDisplay, XWindow, title.c_str());

    sleep(1);
  }
}

#endif /* end of include guard: ENGINE */
