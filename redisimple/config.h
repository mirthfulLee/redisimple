#ifndef REDISIMPLE_CONFIG_H_
#define REDISIMPLE_CONFIG_H_
namespace redisimple {
class Config {
 public:
  static unsigned int thread_num;
  static unsigned int random_seed;
  Config(Config &other) = delete;
  void operator=(Config &) = delete;
  static Config *get_instance() { return instance_; }
  void load_config(char* config_file) {
    instance_ = new Config();
    thread_num = 4;
    random_seed = 0x25871456;
  }

 private:
  static Config *instance_;
  Config() {}
};
}  // namespace redisimple
#endif  // REDISIMPLE_CONFIG_H_