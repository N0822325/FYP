#include <iostream>

#include "Logger.h"
#define DEBUG
Logger logg(3);

#include "Engine.h"


class Poggus : public Engine
{
public:
  Poggus(int width, int height)
  {
    construct(width, height, "Poggus", &logg);
    start();
  }

  void userInitialse() override
  {
    objt* a = Quad(10,10,480,480,0,1);
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
