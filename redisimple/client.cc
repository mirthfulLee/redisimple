#include "client.h"

#include <sys/socket.h>
#include <sys/types.h>

#include <cstddef>
#include <memory>

#include "config.h"
#include "redisimple/object/hash/hash_object.h"
#include "redisimple/util/protocol.h"
namespace redisimple {
Client::Client(int fd, int flag)
    : fd_(fd), flags_(flag), authentecated_(false) {
  // TODO: init expire time
  buffer_size_ = Config::buffer_size;
  in_buffer_.reset(new char[buffer_size_]);
  out_buffer_.reset(new char[buffer_size_]);
  in_index_ = 0;
  out_index_ = 0;
}
// read request from socket buffer (in kernel) to in_buffer_ gradually
int Client::read_and_execute() {
  // read request with recv
  ssize_t retval = 1;
  int step_size = 64;
  while (retval > 0) {
    // FIXME: set MSG_DONTWAIT flag or not?
    retval = recv(fd_, in_buffer_.get() + in_index_, step_size, MSG_DONTWAIT);
    in_index_ += retval;
  }

  // resolve request
  int offset;
  std::unique_ptr<object::ListObject> request =
      util::resolve_list(in_buffer_.get(), offset);
  if (offset != in_index_) {
    // move unused data to the front of input buffer
    for (int i = 0; i + offset < in_index_; ++i) {
      in_buffer_[i] = in_buffer_[offset + i];
    }
  }
  in_index_ -= offset;

  // exeute request

  // deserialize result to out_buffer_

  return 0;
}
int Client::write() {
  while (out_index_) {
    int retval = send(fd_, out_buffer_.get(), out_index_, MSG_DONTWAIT);
    out_index_ -= retval;
  }
  return 0;
}
int Client::fd_matched(int fd) { return fd == fd_; }

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