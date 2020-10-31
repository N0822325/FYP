#define DEBUG

#include <iostream>
#include "Engine.h"
#include "Logger.h"

Logger logg(1);


class Poggus : public Engine
{
public:
  Poggus(int width, int height)
  {
    construct(width, height, &logg, "Poggus");
    start();
  }

  void OnUserUpdate() override
  {
    //getMousePos();
  }

};


int main()
{
  Poggus sadChamp(500,500);

  int a; std::cin >> a;

  return 1;
}
