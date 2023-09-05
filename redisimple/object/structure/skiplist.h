#ifndef REDISIMPLE_OBJECT_STRUCTURE_ZIPLIST_H_
#define REDISIMPLE_OBJECT_STRUCTURE_ZIPLIST_H_
#include <memory>

#include "redisimple_data_structure.h"
namespace redisimple::object::structure {
namespace {
typedef RedisimpleDataStructure RDS;
}  // namespace
class SkiplistNode {
 public:
  double score_;
  std::unique_ptr<RDS> object_;
  SkiplistNode();
  SkiplistNode(double score, std::unique_ptr<RDS>& object, unsigned int level);
  friend class Skiplist;

 private:
  class SkiplistLevel {
   public:
    SkiplistLevel() : forward_(nullptr), span_(0) {}
    friend class Skiplist;

   private:
    SkiplistNode* forward_;
    unsigned int span_;
  };
  std::unique_ptr<SkiplistLevel[]> level_info_;
  SkiplistNode* backward_;
  SkiplistLevel& level(unsigned int i) { return level_info_.get()[i]; }
};
// skiplist to store score & data object in order;
// support different object with same score or same object with different score
// the nodes are ranked from 1 with ascending score
class Skiplist : public RDS {
 public:
  Skiplist();
  ~Skiplist();
  int insert_node(double score, std::unique_ptr<RDS>& object);
  int delete_node(double score, std::unique_ptr<RDS>& object);
  int get_rank(double score, std::unique_ptr<RDS>& object);
  SkiplistNode* get_node_by_rank(int rank);
  // both low limit and high limit is not excluded
  int has_node_in_range(double low_limit, double high_limit);
  SkiplistNode* first_in_range(double low_limit, double high_limit);
  SkiplistNode* last_in_range(double low_limit, double high_limit);
  int delete_nodes_by_score(double low_limit, double high_limit);
  int delete_nodes_by_rank(int low_limit, int high_limit);
  void clear();
  // check whether two skiplist is same;
  int compare(RDS*);
  RedisimpleStructureType structure_type();
  std::unique_ptr<RDS> duplicate();
  int size();
  int hash();

 private:
  // head is an empty node for storing the start of each level index
  std::unique_ptr<SkiplistNode> head_;
  SkiplistNode* tail_;
  unsigned int length_;
  unsigned int level_;
  int delete_node(SkiplistNode* node, SkiplistNode** update_nodes);
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_ZIPLIST_H_