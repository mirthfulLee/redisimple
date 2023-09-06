#include "skiplist.h"

#include <memory>

#include "redisimple/config.h"
#include "redisimple/util/hash.h"
#include "redisimple/util/random.h"
using redisimple::Config;
using redisimple::util::murmurhash2;
using redisimple::util::random_skiplist_node_level;
namespace redisimple::object::structure {

SkiplistNode::SkiplistNode()
    : score_(0), object_(nullptr), level_info_(nullptr) {}
SkiplistNode::SkiplistNode(double score, std::unique_ptr<RDS>& object,
                           unsigned int level)
    : score_(score),
      object_(object.release()),
      level_info_(new SkiplistLevel[level]) {}

Skiplist::Skiplist() : length_(0), level_(1), tail_(nullptr) {
  std::unique_ptr<RDS> empty_head;
  head_.reset(new SkiplistNode(0, empty_head, Config::skiplist_max_level));
}
Skiplist::~Skiplist() {
  SkiplistNode* ptr = head_->level_info_.get()[0].forward_;
  while (ptr) {
    SkiplistNode* tmp = ptr;
    ptr = ptr->level_info_.get()[0].forward_;
    delete[] tmp;
  }
}
int Skiplist::insert_node(double score, std::unique_ptr<RDS>& object) {
  // the last node of each level whose rank is lower than new node
  // their span and forward will be updated after insertion
  SkiplistNode* update_nodes[Config::skiplist_max_level];
  SkiplistNode *cur = head_.get(), *next;
  unsigned int rank[Config::skiplist_max_level];
  // top-down searching
  for (int i = level_ - 1; i >= 0; --i) {
    rank[i] = i == (level_ - 1) ? 0 : rank[i + 1];
    next = cur->level(i).forward_;
    // tranverse along forward
    while (next && (next->score_ < score ||
                    (next->score_ == score &&
                     next->object_->compare(object.get()) < 0))) {
      rank[i] += cur->level(i).span_;
      cur = next;
      next = cur->level(i).forward_;
    }
    update_nodes[i] = cur;
  }
  unsigned int rand_level = random_skiplist_node_level();
  if (rand_level > level_) {
    // initialize unused level_info and update_node
    for (int i = level_; i < rand_level; ++i) {
      rank[i] = 0;
      update_nodes[i] = head_.get();
      update_nodes[i]->level(i).span_ = length_;
    }
    level_ = rand_level;
  }
  std::unique_ptr<SkiplistNode> new_node(
      new SkiplistNode(score, object, rand_level));
  // update span and forward pointer;
  for (int i = 0; i < rand_level; ++i) {
    new_node->level(i).forward_ = update_nodes[i]->level(i).forward_;
    update_nodes[i]->level(i).forward_ = new_node.get();
    new_node->level(i).span_ =
        update_nodes[i]->level(i).span_ - (rank[0] - rank[i]);
    update_nodes[i]->level(i).span_ = (rank[0] - rank[i]) + 1;
  }
  // if rand_level < level, still need to update span of high level node
  for (int i = rand_level; i < level_; ++i) {
    ++update_nodes[i]->level(i).span_;
  }
  // set the backward pointer
  new_node->backward_ =
      (update_nodes[0] == head_.get()) ? nullptr : update_nodes[0];
  if (new_node->level(0).forward_ != nullptr) {
    new_node->level(0).forward_->backward_ = new_node.get();
  } else {
    tail_ = new_node.get();
  }
  ++length_;
  return rank[0];
}

int Skiplist::delete_node(SkiplistNode* node, SkiplistNode** update_nodes) {
  // update forward and span for each level
  for (int i = 0; i < level_; ++i) {
    if (update_nodes[i]->level(i).forward_ == node) {
      update_nodes[i]->level(i).span_ += node->level(i).span_ - 1;
      update_nodes[i]->level(i).forward_ = node->level(i).forward_;
    } else {
      --update_nodes[i]->level(i).span_;
    }
  }
  // update backward (target is not the tail)or tail(target is the tail)
  if (node->level(0).forward_ != nullptr) {
    node->level(0).forward_->backward_ = node->backward_;
  } else {
    tail_ = node->backward_;
  }
  // update level if needed
  while (level_ > 1 && head_->level(level_).forward_ == nullptr) --level_;
  delete[] node;
  --length_;
  return 1;
}

int Skiplist::delete_node(double score, std::unique_ptr<RDS>& object) {
  SkiplistNode* update_nodes[Config::skiplist_max_level];
  SkiplistNode *cur = head_.get(), *next;
  unsigned int rank[Config::skiplist_max_level];
  // top-down searching
  for (int i = level_ - 1; i >= 0; --i) {
    rank[i] = i == (level_ - 1) ? 0 : rank[i + 1];
    next = cur->level(i).forward_;
    // tranverse along forward
    while (next && (next->score_ < score ||
                    (next->score_ == score &&
                     next->object_->compare(object.get()) < 0))) {
      rank[i] += cur->level(i).span_;
      cur = next;
      next = cur->level(i).forward_;
    }
    update_nodes[i] = cur;
  }
  cur = next;
  if (cur && next->score_ == score &&
      next->object_->compare(object.get()) == 0) {
    delete_node(cur, update_nodes);
  } else {
    return 0;
  }
}

int Skiplist::get_rank(double score, std::unique_ptr<RDS>& object) {
  SkiplistNode *cur = head_.get(), *next;
  int rank = 0;
  for (int i = level_ - 1; i >= 0; ++i) {
    next = cur->level(i).forward_;
    // tranverse along forward
    while (next && (next->score_ < score ||
                    (next->score_ == score &&
                     next->object_->compare(object.get()) < 0))) {
      rank += cur->level(i).span_;
      cur = next;
      next = cur->level(i).forward_;
    }
  }
  return rank;
}

SkiplistNode* Skiplist::get_node_by_rank(int rank) {
  if (rank > length_ || rank <= 0) return nullptr;
  SkiplistNode* cur = head_.get();
  for (int i = level_ - 1; i >= 0; ++i) {
    // tranverse along forward
    while (rank && rank >= cur->level(i).span_) {
      rank -= cur->level(i).span_;
      cur = cur->level(i).forward_;
    }
  }
  return cur;
}

int Skiplist::has_node_in_range(double low_limit, double high_limit) {
  if (length_ == 0 || low_limit > tail_->score_) return 0;
  SkiplistNode *cur = head_.get(), *next;
  for (int i = level_ - 1; i >= 0; ++i) {
    next = cur->level(i).forward_;
    // tranverse along forward
    while (next && next->score_ < low_limit) {
      cur = next;
      next = cur->level(i).forward_;
    }
  }
  if (next != nullptr && next->score_ <= high_limit)
    return 1;
  else
    return 0;
}

SkiplistNode* Skiplist::first_in_range(double low_limit, double high_limit) {
  if (length_ == 0 || low_limit > tail_->score_) return nullptr;
  SkiplistNode *cur = head_.get(), *next;
  for (int i = level_ - 1; i >= 0; ++i) {
    // tranverse along forward
    while (cur->level(i).forward_ &&
           cur->level(i).forward_->score_ < low_limit) {
      cur = next;
    }
  }
  next = cur->level(0).forward_;
  if (next != nullptr && next->score_ <= high_limit)
    return next;
  else
    return nullptr;
}
SkiplistNode* Skiplist::last_in_range(double low_limit, double high_limit) {
  if (length_ == 0 || low_limit > tail_->score_) return nullptr;
  SkiplistNode *cur = head_.get(), *next;
  for (int i = level_ - 1; i >= 0; ++i) {
    // tranverse along forward
    while (cur->level(i).forward_ &&
           cur->level(i).forward_->score_ <= high_limit) {
      cur = cur->level(i).forward_;
    }
  }
  if (cur != head_.get() && cur->score_ >= low_limit)
    return cur;
  else
    return nullptr;
}

int Skiplist::delete_nodes_by_score(double low_limit, double high_limit) {
  SkiplistNode* update_nodes[Config::skiplist_max_level];
  SkiplistNode *cur = head_.get(), *next;
  int removed_cnt = 0;
  // top-down searching
  for (int i = level_ - 1; i >= 0; --i) {
    next = cur->level(i).forward_;
    // tranverse along forward
    while (next && next->score_ < low_limit) {
      cur = next;
      next = cur->level(i).forward_;
    }
    update_nodes[i] = cur;
  }
  cur = next;
  while (cur && cur->score_ <= high_limit) {
    next = cur->level(0).forward_;
    delete_node(cur, update_nodes);
    cur = next;
    ++removed_cnt;
  }
  return removed_cnt;
}
int Skiplist::delete_nodes_by_rank(int low_limit, int high_limit) {
  if (low_limit > length_ || high_limit < 0 || low_limit > high_limit) return 0;
  SkiplistNode* update_nodes[Config::skiplist_max_level];
  SkiplistNode *cur = head_.get(), *next;
  int removed_cnt = 0, rank = 0;
  // top-down searching
  for (int i = level_ - 1; i >= 0; --i) {
    // tranverse along forward
    while (rank + cur->level(i).span_ < low_limit) {
      rank += cur->level(i).span_;
      cur = cur->level(i).forward_;
    }
    update_nodes[i] = cur;
  }
  cur = cur->level(0).forward_;
  while (cur && rank <= high_limit) {
    next = cur->level(0).forward_;
    delete_node(cur, update_nodes);
    cur = next;
    ++rank;
    ++removed_cnt;
  }
  return removed_cnt;
}

void Skiplist::clear() {
  SkiplistNode* ptr = head_->level_info_.get()[0].forward_;
  while (ptr) {
    SkiplistNode* tmp = ptr;
    ptr = ptr->level_info_.get()[0].forward_;
    delete[] tmp;
  }
  std::unique_ptr<RDS> empty_head;
  head_.reset(new SkiplistNode(0, empty_head, Config::skiplist_max_level));
}

int Skiplist::compare(RDS* sl) {
  if (sl->structure_type() != REDISIMPLE_STRUCTURE_SKIPLIST) return 1;
  Skiplist* skiplist = dynamic_cast<Skiplist*>(sl);
  if (skiplist->length_ != length_) return 1;
  SkiplistNode *n1 = head_->level(0).forward_,
               *n2 = skiplist->head_->level(0).forward_;
  while (n1 != nullptr) {
    if (n1->object_->compare(n2->object_.get())) return 1;
    n1 = n1->level(0).forward_;
    n2 = n2->level(0).forward_;
  }
  return 0;
}

RedisimpleStructureType Skiplist::structure_type() {
  return REDISIMPLE_STRUCTURE_SKIPLIST;
}

// TODO: support duplicate of skiplist;
std::unique_ptr<RDS> Skiplist::duplicate() { return nullptr; }

int Skiplist::size() { return length_; }

int Skiplist::hash() {
  if (length_ == 0) return 0;
  int hash_val = murmurhash2(&length_, sizeof(length_), Config::random_seed);
  hash_val &= head_->level(0).forward_->object_->hash();
  hash_val &= tail_->object_->hash();
  return hash_val;
}

}  // namespace redisimple::object::structure