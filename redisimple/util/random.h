#ifndef REDISIMPLE_UTIL_RANDOM_H_
#define REDISIMPLE_UTIL_RANDOM_H_
#include <stdlib.h>
#include <time.h>
namespace redisimple::util {
void set_random_seed() { srand((unsigned)time(NULL)); }
void set_random_seed(int seed) { srand(seed); }
int randint() { return rand(); }
}  // namespace redisimple::util
#endif  // REDISIMPLE_UTIL_RANDOM_H_