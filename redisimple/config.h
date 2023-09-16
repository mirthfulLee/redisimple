#ifndef REDISIMPLE_CONFIG_H_
#define REDISIMPLE_CONFIG_H_
namespace redisimple {
class Config {
 public:
  // should be exponent of 2;
  static unsigned int thread_num;
  // random int used during hash;
  static unsigned int random_seed;
  // move <rehash_stride> bucket to new hash table each rash step;
  static unsigned int rehash_stride;
  // the possibility for skiplist node to get higher level
  static float skiplist_p;
  static unsigned int skiplist_max_level;
  static unsigned int large_sds_size;
  static unsigned int more_free_space;
  static unsigned int event_list_size;
  static unsigned int redisimple_port;
  static unsigned int buffer_size;
  Config(Config &other) = delete;
  void operator=(Config &) = delete;
  static Config *get_instance();
  static void load_config(char *config_file);

 private:
  static Config *instance_;
  Config() {}
};
}  // namespace redisimple
#endif  // REDISIMPLE_CONFIG_H_