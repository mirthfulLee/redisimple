#ifndef REDISIMPLE_CLIENT_H_
#define REDISIMPLE_CLIENT_H_
#include <memory>

#include "redisimple/data_base.h"
#include "redisimple/object/list/linked_list.h"
namespace redisimple {

// Each client link to an socket that linked to socket server

class Client {
 public:
  Client(int fd, int flag = 0);

 public:
  // param-mask means the status of socket
  // read data from socket
  // get argv & argc (tokenize in_buffer)
  // chose handler with argv[0]
  // execute handler
  int read_and_execute();
  int write();
  int fd_matched(int fd);

 private:
  int fd_;
  DataBase* selected_db_;
  bool authentecated_;
  int expire_time_;
  int flags_;
  unsigned int buffer_size_;
  // [0, in_index_) in in_buffer_ has been used for socket read
  unsigned int in_index_;
  // [0, out_index_) in out_buffer has been used for socket write
  unsigned int out_index_;
  std::unique_ptr<char[]> in_buffer_;
  std::unique_ptr<char[]> out_buffer_;
  std::unique_ptr<char*[]> argv_;
  int argc_;
  // TODO: processor
 private:
  int read_data();
  int input_resolve();
  int chose_processor();
  int start_process();
};
typedef LinkedListNode<Client> ClientNode;
class ClientList {
 public:
  ClientList();

 public:
  int add_client(std::unique_ptr<Client>& new_client);
  int delete_client_with_fd(int fd);
  // int delete_expired_client();
  Client* get_client_with_fd(int fd);

 private:
  std::unique_ptr<ClientNode> head_;
  ClientNode* tail_;
  int client_num_;
};
}  // namespace redisimple
#endif  // REDISIMPLE_CLIENT_H_