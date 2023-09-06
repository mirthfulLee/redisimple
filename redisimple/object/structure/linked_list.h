#ifndef REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_
#define REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_
#include <memory>
#include <vector>

#include "redisimple_data_structure.h"
namespace redisimple::object::structure {
class LinkedListNode {
 public:
  LinkedListNode() : prev_(nullptr), next_(nullptr), value_(nullptr) {}
  // store the value in the Node (move the owernship to Node)
  LinkedListNode(std::unique_ptr<RDS>& value)
      : prev_(nullptr), next_(nullptr), value_(value.release()) {}
  // store the value in the Node (move the owernship to Node)
  LinkedListNode(std::unique_ptr<RDS>& value, LinkedListNode* prev,
                 LinkedListNode* next)
      : prev_(prev), next_(next), value_(value.release()) {}
  ~LinkedListNode();
  int compare(LinkedListNode* node);
  int compare(RDS* value);
  std::unique_ptr<RDS>& duplicate();
  friend class LinkedList;

 private:
  LinkedListNode *prev_, *next_;
  std::unique_ptr<RDS> value_;
};
// (push and insert) would move the owernship of value to LinkedList
class LinkedList : public RDS {
 public:
  ~LinkedList();
  int size() { return len_; }
  LinkedListNode* head() { return head_; }
  LinkedListNode* tail() { return tail_; }
  void push_back(std::unique_ptr<RDS>& value);
  void push_front(std::unique_ptr<RDS>& value);
  int insert(int index, std::unique_ptr<RDS>& value);
  void pop_back();
  void pop_front();
  // remove the Node that containing the value matchs target
  void remove(RDS* target);
  void remove(int index);
  // trim node not in range[start, stop]
  void trim(int start, int stop);
  void clear();
  // the owernship of value will be moved to index Node
  void set(int index, std::unique_ptr<RDS>& value);
  RDS* index(int i);
  // return the vector containing value ptrs of node in range
  std::unique_ptr<std::vector<RDS*>> range(int start, int stop);
  // copy the entire linked list;
  std::unique_ptr<RDS> duplicate();
  int compare(RDS* obj);
  RedisimpleStructureType structure_type() {
    return REDISIMPLE_STRUCTURE_LINKEDLIST;
  }
  int hash();

 private:
  // head point to the first Node, tail point to the last Node
  LinkedListNode *head_, *tail_;
  int len_;
  // get the pointer of index node
  // support index < 0 which means count from tail(-1) of the list
  LinkedListNode* index_node(int index);
  void delete_node(LinkedListNode* target);
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_LINKED_LIST_H_