#include "client.h"

#include <cstddef>
#include <memory>

namespace redisimple {

int ClientList::add_client(std::unique_ptr<Client>& new_client) {
  std::unique_ptr<ClientNode> new_node(new ClientNode(new_client));
  if (head_ == nullptr) {
    head_.reset(new_node.release());
    tail_ = head_.get();
  } else {
    new_node->prev_ = tail_;
    tail_->next_.reset(new_node.release());
  }
  ++client_num_;
  return 1;
}
int ClientList::delete_client_with_fd(int fd) {
  if (head_->value_->fd_matched(fd)) {
    // delete the head node
    head_.reset(head_->next_.release());
    if (head_ == nullptr) {
      tail_ = nullptr;
    } else {
      head_->prev_ = nullptr;
    }
    --client_num_;
    return 1;
  } else if (tail_->value_->fd_matched(fd)) {
    // node num > 1 && delete the tail node
    tail_->prev_->next_ = nullptr;
    --client_num_;
    return 1;
  } else {
    ClientNode* ptr = head_.get();
    while (ptr != nullptr) {
      if (ptr->value_->fd_matched(fd)) {
        ptr->next_->prev_ = ptr->prev_;
        ptr->prev_->next_.reset(ptr->next_.release());
        --client_num_;
        return 1;
      }
      ptr = ptr->next_.get();
    }
  }
  return 0;
}
// int delete_expired_client();
Client* ClientList::get_client_with_fd(int fd) {
  ClientNode* ptr = head_.get();
  while (ptr != nullptr) {
    if (ptr->value_->fd_matched(fd)) return ptr->value_.get();
    ptr = ptr->next_.get();
  }
  return nullptr;
}
}  // namespace redisimple