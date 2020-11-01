#ifndef ENGINE
  #define ENGINE

  #if defined(__linux__)
    #include <GL/gl.h>
    #include <GL/glx.h>

    #include <X11/X.h>
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
  #endif

  #if defined(_WIN32)
    //WHOMEGALUL
  #endif

  #include "Logger.h"
  #include <string>
  #include <thread>
  #include <unistd.h>
  #include <time.h>

//---------------------------------------------------------//

class Engine
{

public:

  Engine(){};
  // Engine(uint16_t width, uint16_t height);
  // Engine(uint16_t width, uint16_t height, Logger* logs);
  // ~Engine();

public:

  void construct(uint16_t width, uint16_t height, std::string name, Logger* l);
  void start();
  virtual void userCreate(){};
  virtual void userUpdate(){};

  void Draw(int x, int y, int col);
  void DrawLine(int x1, int y1, int x2, int y2, int col);
  void DrawRect(int x1, int y1, int x2, int y2, int col, bool full);
  void DrawTri(int x, int y, int w, int h, int col, bool full);
  void DrawCircle(int x, int y, int r, int col, bool full);

  //void DrawSprite(int x, int y, Sprite s, xScale xs, yScale ys);
  //void DrawString(int x, int y, std::string text, int col, xScale xs, yScale ys)

  void DrawLine(int x1, int y1, int z1, int x2, int y2, int z2, int col);
  void DrawCube(int x, int y, int w, int h, int d, int col, bool full);
  void DrawPyramid(int x, int y, int w, int h, int col, bool full);
  void DrawSphere(int x, int y, int r, int col);


  int getKeys();
  int* getMousePos(XEvent* events);
  int getMouseState();


private:
  void MainThread();
  void CoreUpdate();
  void EventHandler();

  void InitWindow(uint16_t width, uint16_t height);
  void CloseWindow();
  void UpdateWindow();
  void ClearWindow();

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


  float FPSLimit = 0.01;
  float timeCounter = 0;
  float elapsedTime = 0;
  std::chrono::time_point<std::chrono::system_clock> TimePoint1, TimePoint2;

  int MousePos[2] = {0,0};

};

//---------------------------------------------------------//



void Engine::construct(uint16_t width, uint16_t height, std::string name = "", Logger* l = nullptr)
{
  WindowWidth = width;
  WindowHeight = height;
  WindowName = name;
  Logs = l;
}
void Engine::start()
{
  InitWindow(WindowWidth, WindowHeight);
  TimePoint1 = std::chrono::system_clock::now();
  TimePoint2 = std::chrono::system_clock::now();
  std::thread t = std::thread(&Engine::MainThread, this);
  t.join();
}
// Engine::Engine(uint16_t width, uint16_t height)
//   { InitWindow(width,height); }
// Engine::Engine(uint16_t width, uint16_t height, Logger* logs)
//   { Logs = logs; InitWindow(width,height); }
//
// Engine::~Engine()
//   { CloseWindow(); }


void Engine::InitWindow(uint16_t width, uint16_t height)
{
  #if defined(__linux__)
    XDisplay = XOpenDisplay(NULL);
    XRootWindow = DefaultRootWindow(XDisplay);
    XWindowNumber = DefaultScreen(XDisplay);

    unsigned long black,white;
    black = BlackPixel(XDisplay,0);
    white = WhitePixel(XDisplay,0);

    XWindow = XCreateSimpleWindow(XDisplay, XRootWindow, 0, 0, width, height, 0, white, black);
    XSetStandardProperties(XDisplay,XWindow,WindowName.c_str(),"",None,NULL,0,NULL);

    int InputMask = ButtonPressMask|KeyPressMask|PointerMotionMask;
    XSelectInput(XDisplay,XWindow,InputMask);

    XMapRaised(XDisplay,XWindow);
    XMapWindow(XDisplay,XWindow);
    XFlush(XDisplay);
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

void Engine::MainThread()
{

  while(true)
  {
    CalcFrameTime();

    if (timeCounter >= FPSLimit)
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
  if ( FPS < 100000 ) // Lmao
  {
    std::string title = WindowName + " | FPS: " + std::to_string(FPS);
    XStoreName(XDisplay, XWindow, title.c_str());
  }
}

void Engine::EventHandler()
{
  XEvent events;
  while(XPending(XDisplay))
  {
    XNextEvent(XDisplay, &events);

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


#endif /* end of include guard: ENGINE */
