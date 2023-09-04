#ifndef REDISIMPLE_CONFIG_H_
#define REDISIMPLE_CONFIG_H_
namespace redisimple {
class Config {
 public:
  // should be exponent of 2;
  static unsigned int thread_num;
  // random int
  static unsigned int random_seed;
  // move <rehash_stride> bucket to new hash table each rash step;
  static unsigned int rehash_stride;
  Config(Config &other) = delete;
  void operator=(Config &) = delete;
  static Config *get_instance() { return instance_; }
  void load_config(char *config_file) {
    instance_ = new Config();
    thread_num = 4;
    random_seed = 0x25871456;
    rehash_stride = 8;
  }

 private:
  static Config *instance_;
  Config() {}
};
}  // namespace redisimple
#endif  // REDISIMPLE_CONFIG_H_