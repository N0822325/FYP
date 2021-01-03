#include "Engine.h"

// shaders no workey


class Poggus : public Engine
{
public:
  Poggus(int width, int height)
  {
    construct(width, height, "Poggus");
    start();
  }

  void userInitialse() override
  {
    Quad(10,10,480,480,0,1);
  }

  void userUpdate() override
  {

  }

};


int main()
{
  Poggus sadChamp(500,500);

  return 1;
}
