#ifndef REDISIMPLE_OBJECT_STRUCTURE_ZIPLIST_H_
#define REDISIMPLE_OBJECT_STRUCTURE_ZIPLIST_H_
#include <memory>

#include "redisimple_data_structure.h"
namespace redisimple::object::structure {

class Ziplist : public RedisimpleDataStructure {
 public:
  int size();
  void push_back(std::unique_ptr<RedisimpleDataStructure>& value);
  void push_front(std::unique_ptr<RedisimpleDataStructure>& value);
  void insert(int index, std::unique_ptr<RedisimpleDataStructure>& value);
  void pop_back();
  void pop_front();
  // remove the Node that containing the value matchs target
  void remove(std::unique_ptr<RedisimpleDataStructure>& target);
  void delete_node(int index);
  // trim node not in range[start, stop]
  void trim(int start, int stop);
  void set(int index, std::unique_ptr<RedisimpleDataStructure>& value);
  // copy the entire linked list;
  std::unique_ptr<RedisimpleDataStructure> duplicate();
  int hash();

 private:

};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_ZIPLIST_H_