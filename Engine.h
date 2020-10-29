#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


class Engine
{

public:

  Engine() { }

public:

  void InitWindow();
  void UpdateWindow();
  void CloseWindow();

  #if defined(__linux__)
    Display* XDisplay;
    Window XWindow;
    Window XRootWindow;
    int XWindowNumber;
  #endif


private:

  int ScreenWidth;
  int ScreenHeight;

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

  void Clear(int col);

  void Update();

public:

  int getKeys();
  int getMousePos();
  int getMouseState();

};



void Engine::InitWindow()
{

  #if defined(__linux__)
    XDisplay = XOpenDisplay(NULL);
    XRootWindow = DefaultRootWindow(XDisplay);
    XWindowNumber = DefaultScreen(XDisplay);

    unsigned long black,white;
    black = BlackPixel(XDisplay,0);
    white = WhitePixel(XDisplay,0);

    XWindow = XCreateSimpleWindow(XDisplay, XRootWindow, 0, 0, 500, 500, 0, white, black);
    XSetStandardProperties(XDisplay,XWindow,"FYP","",None,NULL,0,NULL);
    XSelectInput(XDisplay,XWindow,ExposureMask|ButtonPressMask|KeyPressMask);

    XMapRaised(XDisplay,XWindow);
    XMapWindow(XDisplay,XWindow);
    XFlush(XDisplay);
  #endif


  #if defined(_WIN32)
    //WHOMEGALUL
  #endif

}
