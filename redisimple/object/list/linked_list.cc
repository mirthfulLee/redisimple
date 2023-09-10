#include "linked_list.h"

#include <memory>
#include <vector>

#include "redisimple/object/redisimple_object.h"

namespace redisimple::object::structure {
template <typename T>
LinkedListNode<T>::LinkedListNode()
    : prev_(nullptr), next_(nullptr), value_(nullptr) {}
// store the value in the Node (move the owernship to Node)
template <typename T>
LinkedListNode<T>::LinkedListNode(std::unique_ptr<T>& value)
    : prev_(nullptr), next_(nullptr), value_(value.release()) {}
template <typename T>
LinkedListNode<T>::LinkedListNode(std::unique_ptr<T>& value,
                                  LinkedListNode<T>* prev,
                                  std::unique_ptr<LinkedListNode<T>>& next)
    : prev_(prev), next_(next.release()), value_(value.release()) {}

RST LinkedList::structure_type() { return REDISIMPLE_STRUCTURE_LINKEDLIST; }
// return 0 means the nodes of two list have same order and value;
// otherwise return 1;
int LinkedList::compare(RedisimpleObject* ro) {
  if (ro->structure_type() != REDISIMPLE_STRUCTURE_LINKEDLIST) return 1;
  LinkedList* list = dynamic_cast<LinkedList*>(ro);
  if (len_ != list->len_) return 1;
  ObjectNode *cur_a = head_.get(), *cur_b = list->head_.get();
  while (cur_a != nullptr) {
    int cmp = cur_a->value_->compare(cur_b->value_.get());
    if (cmp != 0) {
      return cmp;
    }
    cur_a = cur_a->next_.get();
    cur_b = cur_b->next_.get();
  }
  return 0;
}
std::unique_ptr<RedisimpleObject> LinkedList::duplicate() {
  std::unique_ptr<LinkedList> new_list(new LinkedList());
  ObjectNode* cur = head_.get();
  while (cur != nullptr) {
    std::unique_ptr<RedisimpleObject> value = cur->value_->duplicate();
    new_list->push_back(value);
  }
  return new_list;
}
int LinkedList::size() { return len_; }
int LinkedList::hash() {
  int hash_val = len_;
  if (head_ != nullptr) {
    hash_val += head_->value_->hash() + tail_->value_->hash();
  }
  return hash_val;
}
// TODO: serialize and deserialize of linkedlist
int LinkedList::serialize(char* out_buf, int& offset, int& cnt) { return 0; }
int LinkedList::deserialize(char* argv[], int& offset, int& argc) { return 0; }

int LinkedList::push_back(std::unique_ptr<RedisimpleObject>& value) {
  std::unique_ptr<ObjectNode> new_node(new ObjectNode(value));
  new_node->prev_ = tail_;
  if (tail_ != nullptr) {
    tail_->next_.reset(new_node.release());
    tail_ = tail_->next_.get();
  } else {
    head_.reset(new_node.release());
    tail_ = head_.get();
  }
  ++len_;
  return 1;
}
int LinkedList::push_front(std::unique_ptr<RedisimpleObject>& value) {
  std::unique_ptr<ObjectNode> new_node(new ObjectNode(value));
  if (head_ != nullptr) head_->prev_ = new_node.get();
  new_node->next_.reset(head_.release());
  head_.reset(new_node.release());
  ++len_;
  if (len_ == 1) tail_ = head_.get();
  return 1;
}
int LinkedList::insert(int index, std::unique_ptr<RedisimpleObject>& value) {
  // support index < 0 which is the reversed index from tail to head
  if (index < -len_ - 1 || index > len_) return 0;
  if (index == 0 || index == -len_ - 1) {
    push_front(value);
    return 1;
  }
  if (index == len_ || index == -1) {
    push_back(value);
    return 1;
  }
  // ptr must have prev and next node;
  ObjectNode* ptr = index_node(index);
  ptr->prev_->next_.reset(new ObjectNode(value, ptr->prev_, ptr->prev_->next_));
  ++len_;
  return 1;
}
std::unique_ptr<RedisimpleObject> LinkedList::pop_front() {
  if (len_ == 0) return nullptr;
  --len_;
  std::unique_ptr<RedisimpleObject> value(tail_->value_.release());
  if (head_->next_ != nullptr) {
    head_.reset(head_->next_.release());
  } else {
    head_ = nullptr;
    tail_ = nullptr;
  }
  return value;
}
std::unique_ptr<RedisimpleObject> LinkedList::pop_back() {
  if (len_ == 0) return nullptr;
  --len_;
  std::unique_ptr<RedisimpleObject> value(tail_->value_.release());
  if (tail_->prev_ != nullptr) {
    ObjectNode* prev = tail_->prev_;
    prev->next_ = nullptr;
    tail_ = prev;
  } else {
    head_ = nullptr;
    tail_ = nullptr;
  }
  return value;
}
// remove the Node that containing the value matchs target
int LinkedList::remove(RedisimpleObject* target) {
  ObjectNode* cur = head_.get();
  while (cur != nullptr) {
    if (cur->value_->compare(target) == 0) {
      --len_;
      if (cur == head_.get()) {
        pop_front();
      } else if (cur == tail_) {
        pop_back();
      } else {
        cur->next_->prev_ = cur->prev_;
        cur->prev_->next_.reset(cur->next_.release());
      }
      return 1;
    }
  }
  return 0;
}
// trim node not in range[start, stop]
// return the number of deleted nodes
int LinkedList::trim(int start, int stop) {
  if (start < 0) start = len_ + start;
  if (stop < 0) stop = len_ + stop;
  if (stop < start) return 0;
  int result = len_ - (stop - start + 1);
  while (len_ > stop) pop_back();
  while (len_ > stop - start + 1) pop_front();
  return result;
}
// The node take over the value
int LinkedList::set(int index, std::unique_ptr<RedisimpleObject>& value) {
  ObjectNode* ptr = index_node(index);
  if (ptr) {
    ptr->value_.reset(value.release());
    return 1;
  }
  return 0;
}
RedisimpleObject* LinkedList::at(int index) {
  ObjectNode* ptr = index_node(index);
  if (ptr) return ptr->value_.get();
  return nullptr;
}
std::unique_ptr<std::vector<RedisimpleObject*>> LinkedList::range(int start,
                                                                  int stop) {
  if (start < 0) start = len_ + start;
  if (stop < 0) stop = len_ + stop;
  if (stop < start || start >= len_) return nullptr;
  if (stop >= len_) stop = len_ - 1;
  std::vector<RedisimpleObject*>* data_list =
      new std::vector<RedisimpleObject*>(stop - start + 1, nullptr);
  ObjectNode* cur = index_node(start);
  int cnt = 0;
  while (cnt < stop - start + 1) {
    (*data_list)[cnt] = cur->value_.get();
    ++cnt;
    cur = cur->next_.get();
  }
  return std::unique_ptr<std::vector<RedisimpleObject*>>(data_list);
}

ObjectNode* LinkedList::index_node(int index) {
  if (len_ == 0 || index >= len_ || index < -len_) return nullptr;
  int cur_index;
  ObjectNode* ptr;
  if ((index >= 0 && index < len_ / 2) || (index < 0 && index > -len_ / 2)) {
    ptr = head_.get();
    cur_index = 0;
    while (cur_index < index) {
      ptr = ptr->next_.get();
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
