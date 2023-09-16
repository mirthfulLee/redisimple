#include "config.h"
namespace redisimple {
unsigned int Config::thread_num;
// random int used during hash;
unsigned int Config::random_seed;
// move <rehash_stride> bucket to new hash table each rash step;
unsigned int Config::rehash_stride;
// the possibility for skiplist node to get higher level
float Config::skiplist_p;
unsigned int Config::skiplist_max_level;
unsigned int Config::large_sds_size;
unsigned int Config::more_free_space;
unsigned int Config::event_list_size;
unsigned int redisimple_port;
unsigned int buffer_size;
Config *Config::instance_;
Config *Config::get_instance() { return instance_; }
void Config::load_config(char *config_file) {
  instance_ = new Config();
  thread_num = 4;
  random_seed = 0x25871456;
  rehash_stride = 8;
  skiplist_p = 0.25;
  skiplist_max_level = 16;
  large_sds_size = 1024;
  more_free_space = 1024;
  event_list_size = 10000;
  redisimple_port = 65432;
  buffer_size = 4096;
}
}  // namespace redisimple