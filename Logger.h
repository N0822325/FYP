#ifndef LOGGER
  #define LOGGER
  #include <iostream>
  #include <string>


class Logger
{

public:
  Logger(uint8_t level) { setVerbose(level); }

  void log(uint8_t verbose, int timestamp, std::string* value);

private:

  void setVerbose(uint8_t level) { verbose = level; }

  uint8_t verbose;

};



void Logger::log(uint8_t _verbose, int timestamp, std::string* value)
{
  if(_verbose < verbose)
    std::cout << "[" << timestamp << "] " << *value << std::endl;
}


#endif /* end of include guard: LOGGER */
