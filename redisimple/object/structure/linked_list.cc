#include "linked_list.h"

#include <memory>
#include <vector>

#include "redisimple_data_structure.h"

namespace redisimple::object::structure {
LinkedListNode::~LinkedListNode() { value_ = nullptr; }
LinkedList::~LinkedList() {
  // delete all nodes
  LinkedListNode *cur = head_, *next = head_;
  while (cur != nullptr) {
    next = cur->next_;
    delete[] cur;
    cur = next;
  }
}
void LinkedList::push_back(std::unique_ptr<RDS>& value) {
  LinkedListNode* new_node = new LinkedListNode(value);
  new_node->prev_ = tail_;
  if (tail_) tail_->next_ = new_node;
  tail_ = new_node;
  ++len_;
}
void LinkedList::push_front(std::unique_ptr<RDS>& value) {
  LinkedListNode* new_node = new LinkedListNode(value);
  new_node->next_ = head_;
  if (head_) head_->prev_ = new_node;
  head_ = new_node;
  ++len_;
}
int LinkedList::insert(int index, std::unique_ptr<RDS>& value) {
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
  LinkedListNode* ptr = index_node(index);
  ptr->prev_ = new LinkedListNode(value, ptr->prev_, ptr);
  ++len_;
  return 1;
}
void LinkedList::pop_back() {
  if (tail_) {
    LinkedListNode* old_tail = tail_;
    tail_ = tail_->prev_;
    delete[] old_tail;
    --len_;
  }
}
void LinkedList::pop_front() {
  if (head_) {
    LinkedListNode* old_head = head_;
    head_ = head_->next_;
    delete[] old_head;
    --len_;
  }
}
// remove the Node that containing the value matchs target
void LinkedList::remove(RDS* target) {
  LinkedListNode* cur = head_;
  while (cur != nullptr) {
    if (cur->value_->compare(target) == 0) {
      delete_node(cur);
      return;
    }
    cur = cur->next_;
  }
}
void LinkedList::remove(int index) { delete_node(index_node(index)); }
// trim node not in range[start, stop]
void LinkedList::trim(int start, int stop) {
  if (start < 0) start = len_ + start;
  if (stop < 0) stop = len_ + stop;
  if (stop < start) return;
  if (start >= len_) {
    clear();
    return;
  }
  // trim the part after stop
  LinkedListNode* cur = tail_;
  while (len_ > stop + 1) {
    tail_ = cur->prev_;
    delete[] cur;
    cur = tail_;
    --len_;
  }
  tail_->next_ = nullptr;
  cur = head_;
  while (len_ > stop - start + 1) {
    head_ = cur->next_;
    delete[] cur;
    cur = head_;
    --len_;
  }
  head_->prev_ = nullptr;
}
void LinkedList::clear() {
  LinkedListNode *cur = head_, *tmp;
  while (cur) {
    tmp = cur->next_;
    delete[] cur;
    cur = tmp;
  }
  len_ = 0;
  tail_ = head_ = nullptr;
}
// The node take over the value
void LinkedList::set(int index, std::unique_ptr<RDS>& value) {
  index_node(index)->value_.reset(value.release());
}
RDS* LinkedList::index(int i) { return index_node(i)->value_.get(); }
std::unique_ptr<std::vector<RDS*>> LinkedList::range(int start, int stop) {
  if (start < 0) start = len_ + start;
  if (stop < 0) stop = len_ + stop;
  if (stop < start || start >= len_) return nullptr;
  if (stop >= len_) stop = len_ - 1;
  std::vector<RDS*>* data_list =
      new std::vector<RDS*>(stop - start + 1, nullptr);
  LinkedListNode* cur = index_node(start);
  int cnt = 0;
  while (cnt < stop - start + 1) {
    (*data_list)[cnt] = cur->value_.get();
    ++cnt;
    cur = cur->next_;
  }
  return std::unique_ptr<std::vector<RDS*>>(data_list);
}
std::unique_ptr<RDS> LinkedList::duplicate() {
  LinkedList* new_list = new LinkedList();
  LinkedListNode* cur = head_;
  while (cur != nullptr) {
    std::unique_ptr<RDS> value = cur->value_->duplicate();
    new_list->push_back(value);
  }
  return std::unique_ptr<RDS>(new_list);
}
// return 0 means the nodes of two list have same order and value;
// otherwise return 1;
int LinkedList::compare(RDS* obj) {
  if (obj->structure_type() != REDISIMPLE_STRUCTURE_LINKEDLIST) return 1;
  LinkedList* list = dynamic_cast<LinkedList*>(obj);
  if (len_ != list->len_) return 1;
  LinkedListNode *cur_a = head_, *cur_b = list->head_;
  while (cur_a != nullptr) {
    if (cur_a->compare(cur_b) != 0) {
      return 1;
    }
    cur_a = cur_a->next_;
    cur_b = cur_b->next_;
  }
  return 0;
}
int LinkedList::hash() {
  if (len_ == 0) return 0;
  int hash_val = len_;
  hash_val &= head_->value_->hash();
  hash_val &= tail_->value_->hash();
  return hash_val;
}
LinkedListNode* LinkedList::index_node(int index) {
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
void LinkedList::delete_node(LinkedListNode* target) {
  if (target->prev_) target->prev_->next_ = target->next_;
  if (target->next_) target->next_->prev_ = target->prev_;
  delete[] target;
  --len_;
}
}  // namespace redisimple::object::structure
