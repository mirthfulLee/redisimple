#include <iostream>

#include "config.h"
#include "util/random.h"
int main() {
  char config_file[] = "redisimple.conf";
  redisimple::Config::load_config(config_file);
  std::cout << redisimple::util::randint() << std::endl;
}