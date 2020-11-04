#ifndef ENGINE
  #define ENGINE

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
  #include <iostream>
  #include <string>
  #include <thread>
  #include <unistd.h>
  #include <time.h>

  void DrawAQuad() {

  }

//---------------------------------------------------------//

class Engine
{

public:

  //Engine(){};
  // Engine(uint16_t width, uint16_t height);
  // Engine(uint16_t width, uint16_t height, Logger* logs);
  // ~Engine();

public:

  void construct(uint16_t width, uint16_t height, std::string name, Logger* logs);
  void start();
  virtual void userCreate(){};
  virtual void userUpdate(){};

  // void Draw(int x, int y, int col);
  // void DrawLine(int x1, int y1, int x2, int y2, int col);
  void DrawRect(int x1, int y1, int x2, int y2, int col, bool full);
  // void DrawTri(int x, int y, int w, int h, int col, bool full);
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
  void EventHandler();

  void InitWindow(uint16_t width, uint16_t height);
  void CloseWindow();
  //void UpdateWindow(){};
  //void ClearWindow(){};


  void CalcFrameTime();
  void DisplayFPS();

  Logger* Logs;

  std::string WindowName;
  int WindowWidth;
  int WindowHeight;

  #if defined(__linux__)
    Display* XDisplay;
    Window XWindow;
    Window XRootWindow;
    int XWindowNumber;
  #endif

  //GL
  Colormap cmap;
  XVisualInfo *vi;
  XWindowAttributes gwa;
  XSetWindowAttributes swa;
  GLXContext glContext;
  GLint attributes[5] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  //


  float FPSLimit = 0.01;
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
  std::thread t = std::thread
    (&Engine::Initialse, this);
  t.join();
}
void Engine::Initialse()
{
  TimePoint1 = std::chrono::system_clock::now();
  TimePoint2 = std::chrono::system_clock::now();
  InitWindow(WindowWidth, WindowHeight);
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
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | PointerMotionMask;

    XWindow = XCreateWindow
      (XDisplay, XRootWindow, 0, 0, WindowWidth, WindowHeight, 0,
        vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

    //XMapRaised(XDisplay,XWindow);
    XMapWindow(XDisplay,XWindow);
    XStoreName(XDisplay, XWindow, WindowName.c_str());
    //XFlush(XDisplay);

    glContext = glXCreateContext(XDisplay, vi, NULL, GL_TRUE);
    glXMakeCurrent(XDisplay, XWindow, glContext);
    glEnable(GL_DEPTH_TEST);
  #endif


  #if defined(_WIN32)
    //WHOMEGALUL
  #endif

}

void Engine::CloseWindow()
{

  #if defined(__linux__)
    XDestroyWindow(XDisplay,XWindow);
    XCloseDisplay(XDisplay);
  #endif

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
  DisplayFPS();
  EventHandler();
  userUpdate();
}


void Engine::EventHandler()
{
  XEvent events;
  while(XPending(XDisplay))
  {
    XNextEvent(XDisplay, &events);

    if (events.type == Expose)
    {
      XGetWindowAttributes(XDisplay, XWindow, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
      DrawRect(0,0,0,0,0,1);
      glXSwapBuffers(XDisplay, XWindow);
    }
    if (events.type == KeyPress)
    {
      //Key Pressed
    }
    else if (events.type == KeyRelease)
    {
      //Key Released
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

  #if defined(DEBUG)
    using namespace std;
    std::string value = "X: " + to_string(MousePos[0]) + " | "
                      + "Y: " + to_string(MousePos[1]);
    Logs->log(3, &value);
  #endif

  return MousePos;
}



void Engine::DrawRect(int x1, int y1, int x2, int y2, int col, bool full)
{
  glBegin(GL_QUADS);
   glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
   glColor3f(0., 1., 0.); glVertex3f( .75, -.75, 0.);
   glColor3f(0., 0., 1.); glVertex3f( .75,  .75, 0.);
   glColor3f(1., 1., 0.); glVertex3f(-.75,  .75, 0.);
  glEnd();
}





void Engine::CalcFrameTime()
{
  TimePoint2 = std::chrono::system_clock::now();
  std::chrono::duration<float> a = TimePoint2 - TimePoint1;
  elapsedTime = a.count();
  TimePoint1 = TimePoint2;
}

void Engine::DisplayFPS()
{
  int FPS = (1 / (elapsedTime + FPSLimit)) + 1;
  if ( FPS < 1000 ) // Lmao
  {
    std::string title = WindowName + " | FPS: " + std::to_string(FPS);
    XStoreName(XDisplay, XWindow, title.c_str());
  }
}

#endif /* end of include guard: ENGINE */
