#include "linked_list.h"
#include "redisimple_data_structure.h"

namespace redisimple::object::structure {
LinkedListNode::~LinkedListNode() { value_ = nullptr; }
LinkedList::~LinkedList() {
  // delete all nodes
  LinkedListNode *cur = head_, *next = head_;
  while (cur != nullptr) {
    next = cur->next();
    delete[] cur;
    cur = next;
  }
}
void LinkedList::push_back(std::unique_ptr<RedisimpleDataStructure>& value) {
  LinkedListNode* new_node = new LinkedListNode(value);
  new_node->prev_ = tail_;
  if (tail_) tail_->next_ = new_node;
  tail_ = new_node;
  ++len_;
}
void LinkedList::push_front(std::unique_ptr<RedisimpleDataStructure>& value) {
  LinkedListNode* new_node = new LinkedListNode(value);
  new_node->next_ = head_;
  if (head_) head_->prev_ = new_node;
  head_ = new_node;
  ++len_;
}
int LinkedList::insert(int index,
                       std::unique_ptr<RedisimpleDataStructure>& value) {
  // support index < 0 which is the reversed index from tail to head
  if (index < -len_ || index > len_) return 0;
  if (index == 0 || index == -len_) {
    push_front(value);
    return 1;
  }
  if (index == len_) {
    push_back(value);
    return 1;
  }
  LinkedListNode* ptr = get_node(index);
  ptr->prev_ = new LinkedListNode(value, ptr->prev_, ptr);
  ++len_;
  return 1;
}
void LinkedList::pop_back() {
  if (tail_) {
    LinkedListNode* old_tail = tail_;
    tail_ = tail_->prev_;
    delete [] old_tail;
    --len_;
  }
}
void LinkedList::pop_front() {
  if (head_) {
    LinkedListNode* old_head = head_;
    head_ = head_->next_;
    delete [] old_head;
    --len_;
  }
}
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
LinkedListNode* LinkedList::get_node(int index) {
  if (len_ == 0 || index >= len_ || index < -len_) return nullptr;
  int cur_index;
  LinkedListNode* ptr;
  if ((index >= 0 && index < len_ / 2) || (index < 0 && index > -len_ / 2)) {
    ptr = head_;
    cur_index = 0;
    while (cur_index < index) {
      ptr = ptr->next_;
      ++cur_index;
    }
  } else {
    index = index > 0 ? index - len_ : index;
    cur_index = -1;
    ptr = tail_;
    while (cur_index > index) {
      ptr = ptr->prev_;
      --cur_index;
    }
  }
  return ptr;
}

}  // namespace redisimple::object::structure
