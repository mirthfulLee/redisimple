#ifndef REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_
#define REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_
#include <memory>
#include <vector>

#include "redisimple_data_structure.h"
namespace redisimple::object::structure {
class LinkedListNode {
 public:
  LinkedListNode() : prev_(nullptr), next_(nullptr), value_(nullptr) {}
  LinkedListNode(std::unique_ptr<RedisimpleDataStructure>& value)
      : prev_(nullptr), next_(nullptr), value_(value.release()) {}
  LinkedListNode(std::unique_ptr<RedisimpleDataStructure>& value,
                 LinkedListNode* prev, LinkedListNode* next)
      : prev_(prev), next_(next), value_(value.release()) {}
  ~LinkedListNode();
  LinkedListNode*& prev() { return prev_; }
  LinkedListNode*& next() { return next_; }
  std::unique_ptr<RedisimpleDataStructure>& value() { return value_; }
  friend class LinkedList;

 private:
  LinkedListNode *prev_, *next_;
  std::unique_ptr<RedisimpleDataStructure> value_;
};
class LinkedList : public RedisimpleDataStructure {
 public:
  ~LinkedList();
  int length() { return len_; }
  LinkedListNode* head() { return head_; }
  LinkedListNode* tail() { return tail_; }
  void push_back(std::unique_ptr<RedisimpleDataStructure>& value);
  void push_front(std::unique_ptr<RedisimpleDataStructure>& value);
  int insert(int index, std::unique_ptr<RedisimpleDataStructure>& value);
  void pop_back();
  void pop_front();
  // remove the Node that containing the value matchs target
  void remove(std::unique_ptr<RedisimpleDataStructure>& target);
  void delete_node(int index);
  // trim node not in range[start, stop]
  void trim(int start, int stop);
  void set(int index, std::unique_ptr<RedisimpleDataStructure>& value);
  std::unique_ptr<RedisimpleDataStructure>& index(int i);
  // return the vector containing value ptrs of node in range
  std::unique_ptr<std::vector<RedisimpleDataStructure*>>& range(int start,
                                                                int stop);
  // copy the entire linked list;
  LinkedList* duplicate();

 private:
  // head point to the first Node, tail point to the last Node
  LinkedListNode *head_, *tail_;
  int len_;
  // function to duplicate node value
  RedisimpleDataStructure* (*duplicate_)(
      std::unique_ptr<RedisimpleDataStructure>& value);
  // function to free node value??? no need when using unique_ptr?
  void (*free_)(std::unique_ptr<RedisimpleDataStructure>& value);
  // function to compare value
  int (*match)(std::unique_ptr<RedisimpleDataStructure>& a,
               std::unique_ptr<RedisimpleDataStructure>& b);
  LinkedListNode* get_node(int index);
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_