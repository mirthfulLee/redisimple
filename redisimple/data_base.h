#ifndef REDISIMPLE_DATA_BASE_H_
#define REDISIMPLE_DATA_BASE_H_
#include <memory>

#include "object/hash/hash_object.h"
#include "object/list/linked_list.h"
#include "object/redisimple_object.h"
#include "object/string/string_object.h"
using redisimple::object::HashObject;
using redisimple::object::RedisimpleObject;
using redisimple::object::StringObject;
using redisimple::object::structure::LinkedListNode;
namespace redisimple {
// DataBase Derives from HashObject
// But it does not override the function of HashObject
// So it could directly use the HashObject API like get, set etc.
class DataBase : public HashObject {
 public:
  DataBase(int index, std::unique_ptr<StringObject>& name,
           std::unique_ptr<StringObject>& password);

 public:
  int authentecate(StringObject*);
  friend class DataBaseList;

 private:
  int index_;
  std::unique_ptr<StringObject> name_;
  std::unique_ptr<StringObject> password_;
  // TODO: expire function
};
typedef LinkedListNode<DataBase> DBNode;
class DataBaseList {
 public:
  DataBaseList();

 public:
  DataBase* get_db_with_name(StringObject* name);
  DataBase* get_db_with_index(int index);
  int delete_db_with_name(StringObject* name);
  int delete_db_with_index(int index);
  int add_db(std::unique_ptr<DataBase> db);

 private:
  std::unique_ptr<DBNode> head_;
  int db_num_;
};
}  // namespace redisimple
#endif  // REDISIMPLE_DATA_BASE_H_