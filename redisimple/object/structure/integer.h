#ifndef REDISIMPLE_OBJECT_STRUCTURE_INTEGER_H_
#define REDISIMPLE_OBJECT_STRUCTURE_INTEGER_H_

#include "redisimple_data_structure.h"
namespace redisimple::object::structure {
class Integer : public RDS {
 public:
  Integer() : value_(0) {}
  Integer(int value) : value_(value) {}
  int compare(RDS*);
  RedisimpleStructureType structure_type();
  std::unique_ptr<RDS> duplicate();
  int size();
  int hash();

 private:
  int value_;
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_INTEGER_H_