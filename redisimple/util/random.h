#ifndef REDISIMPLE_UTIL_RANDOM_H_
#define REDISIMPLE_UTIL_RANDOM_H_
#include <stdlib.h>
#include <time.h>

#include "redisimple/config.h"
using redisimple::Config;
namespace redisimple::util {
void set_random_seed() { srand((unsigned)time(NULL)); }
void set_random_seed(int seed) { srand(seed); }
int randint() { return rand(); }
unsigned int random_skiplist_node_level() {
  int level = 1;
  while ((random() &0xFFFF) < (Config::skiplist_p) && level < Config::skiplist_max_level)
    ++level;
  return level;
}
}  // namespace redisimple::util
#endif  // REDISIMPLE_UTIL_RANDOM_H_