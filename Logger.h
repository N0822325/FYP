#ifndef LOGGER
  #define LOGGER
  #include <iostream>
  #include <string>


class Logger
{

public:
  Logger(uint8_t level) { setVerbose(level); }

  void log(uint8_t verbose, std::string* value);

private:

  void setVerbose(uint8_t level) { verbose = level; }

  uint8_t verbose;

};



void Logger::log(uint8_t _verbose, std::string* value)
{
  if(_verbose <= verbose)
    std::cout << *value << std::endl;
}


#endif /* end of include guard: LOGGER */
