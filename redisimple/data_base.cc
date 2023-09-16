#include "data_base.h"

namespace redisimple {
DataBase::DataBase(int index, std::unique_ptr<StringObject>& name,
                   std::unique_ptr<StringObject>& password)
    : index_(index) {
  name_.reset(name.release());
  password_.reset(password.release());
}
int DataBase::authentecate(StringObject* password) {
  return password->compare(password_.get()) == 0;
}

DataBaseList* DataBaseList::get_db_list_ref() {
  if (instance_ == nullptr) instance_.reset(new DataBaseList());
  return instance_.get();
}
std::unique_ptr<DataBaseList> DataBaseList::instance_;

DataBase* DataBaseList::get_db_with_name(StringObject* name) {
  DBNode* ptr = head_.get();
  while (ptr) {
    if (ptr->value_->name_->compare(name) == 0) return ptr->value_.get();
    ptr = ptr->next_.get();
  }
  return nullptr;
}
DataBase* DataBaseList::get_db_with_index(int index) {
  DBNode* ptr = head_.get();
  while (ptr) {
    if (ptr->value_->index_ == index) return ptr->value_.get();
    ptr = ptr->next_.get();
  }
  return nullptr;
}
// TODO: implement delete db from db_list
int DataBaseList::delete_db_with_name(StringObject* name) { return 0; }
int DataBaseList::delete_db_with_index(int index) { return 0; }

int DataBaseList::add_db(std::unique_ptr<DataBase> db) {
  if (head_ == nullptr) {
    head_.reset(new DBNode(db));
    tail_ = head_.get();
    ++db_num_;
    return 1;
  } else {
    tail_->next_.reset(new DBNode(db));
    tail_->next_->prev_ = tail_;
    tail_ = tail_->next_.get();
    ++db_num_;
    return 1;
  }
}
DataBaseList::DataBaseList() : db_num_(0) {}

}  // namespace redisimple