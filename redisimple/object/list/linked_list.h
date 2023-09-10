#ifndef REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_
#define REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_
#include <memory>
#include <vector>

#include "list_object.h"
#include "redisimple/object/redisimple_object.h"
namespace redisimple::object::structure {
// define as template so it could be reused. such as DBlist
template <typename T>
class LinkedListNode {
 public:
  LinkedListNode();
  // store the value in the Node (move the owernship to Node)
  LinkedListNode(std::unique_ptr<T>& value);
  // store the value in the Node (move the owernship to Node)
  LinkedListNode(std::unique_ptr<T>& value, LinkedListNode<T>* prev,
                 std::unique_ptr<LinkedListNode>& next);

 public:
  LinkedListNode* prev_;
  std::unique_ptr<LinkedListNode> next_;
  std::unique_ptr<T> value_;
};
typedef LinkedListNode<RedisimpleObject> ObjectNode;
// (push and insert) would move the owernship of value to LinkedList
class LinkedList : public ListObject {
 public:
  ~LinkedList();

 public:
  RST structure_type();
  int compare(RedisimpleObject* ro);
  std::unique_ptr<RedisimpleObject> duplicate();
  int size();
  int hash();
  int serialize(char* out_buf, int& offset, int& cnt);
  int deserialize(char* argv[], int& offset, int& argc);

 public:
  int push_front(std::unique_ptr<RedisimpleObject>&);
  int push_back(std::unique_ptr<RedisimpleObject>&);
  int insert(int, std::unique_ptr<RedisimpleObject>&);
  std::unique_ptr<RedisimpleObject> pop_front();
  std::unique_ptr<RedisimpleObject> pop_back();
  int remove(RedisimpleObject*);
  int remove(int index);

  int trim(int left, int right);
  int set(int, std::unique_ptr<RedisimpleObject>&);
  RedisimpleObject* at(int);
  std::unique_ptr<std::vector<RedisimpleObject*>> range(int start, int stop);

 private:
  // head point to the first Node, tail point to the last Node
  std::unique_ptr<ObjectNode> head_;
  ObjectNode* tail_;
  int len_;
  // get the pointer of index node
  // support index < 0 which means count from tail(-1) of the list
  ObjectNode* index_node(int index);
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_