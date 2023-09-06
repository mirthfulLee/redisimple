#include "integer.h"

#include <algorithm>

#include "redisimple/config.h"
#include "redisimple/util/hash.h"
#include "redisimple_data_structure.h"

using redisimple::Config;
namespace redisimple::object::structure {
int Integer::compare(RDS* value) {
  Integer* vptr = dynamic_cast<Integer*>(value);
  if (vptr->value_ == value_)
    return 0;
  else
    return vptr->value_ > value_ ? 1 : -1;
}
RedisimpleStructureType Integer::structure_type() {
  return REDISIMPLE_STRUCTURE_INT;
}
std::unique_ptr<RDS> Integer::duplicate() {
  return std::unique_ptr<Integer>(new Integer(value_));
}
int Integer::size() { return sizeof(int); }
int Integer::hash() {
  return redisimple::util::murmurhash2(&value_, sizeof(int),
                                       Config::get_instance()->random_seed);
}
}  // namespace redisimple::object::structure