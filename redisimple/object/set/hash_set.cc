#include "hash_set.h"

#include <memory>

#include "redisimple/log/error.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple/util/random.h"
using redisimple::util::randint;

namespace redisimple::object::structure {
HashSetEntry::HashSetEntry() : object_(nullptr), next_(nullptr) {}
HashSetEntry::HashSetEntry(std::unique_ptr<RedisimpleObject>& object)
    : object_(object.release()), next_(nullptr) {}

HashSetTable::HashSetTable(int size)
    : entry_num_(0), table_size_(size), size_mask_(size - 1) {
  table_.reset(new HashSetEntry[size]);
  // FIXME: does memset necessary?
  memset(table_.get(), 0, size);
}

int HashSetTable::add(unsigned int bucket,
                      std::unique_ptr<RedisimpleObject>& object) {
  if (table_[bucket].object_ == nullptr) {
    table_[bucket].object_.reset(object.release());
    ++entry_num_;
    return 1;
  } else {
    HashSetEntry* ptr = &(table_[bucket]);
    // loop until ptr points to the matched entry or nullptr;
    while (ptr->next_) {
      if (object->compare(ptr->object_.get()) == 0) return 0;
      ptr = ptr->next_.get();
    }
    ptr->next_.reset(new HashSetEntry(object));
    ++entry_num_;
    return 1;
  }
}

// return a random value in set;
RedisimpleObject* HashSetTable::random_value(unsigned int bucket) {
  // treat the list as cycle, loop until get the random entry
  int rand_val = randint(), entry_cnt = 0;
  HashSetEntry* ptr = &(table_[bucket]);
  if (ptr->object_ == nullptr) return nullptr;
  while (rand_val) {
    --rand_val;
    ++entry_cnt;
    if (ptr->next_ == nullptr && rand_val) {
      ptr = &(table_[bucket]);
      rand_val %= entry_cnt;
    } else {
      ptr = ptr->next_.get();
    }
  }
  return ptr->object_.get();
}
// pop a random value and delete it
std::unique_ptr<RedisimpleObject> HashSetTable::pop_value(unsigned int bucket) {
  // the algorithm is similar to that in random_value
  int rand_val = randint(), entry_cnt = 0;
  HashSetEntry* ptr = &(table_[bucket]);
  if (ptr->object_ == nullptr) {
    // the bucket has no entry
    return nullptr;
  }
  --entry_cnt;
  if (ptr->next_ == nullptr) {
    // the bucket only has one entry
    return std::unique_ptr<RedisimpleObject>(ptr->object_.release());
  }
  while (rand_val) {
    --rand_val;
    ++entry_cnt;
    if (ptr->next_ == nullptr && rand_val) {
      ptr = &(table_[bucket]);
      rand_val %= entry_cnt;
    } else {
      ptr = ptr->next_.get();
    }
  }
  // pop next entry after `ptr`
  // if `ptr` point to last entry, remove the first one
  if (ptr->next_ == nullptr) {
    std::unique_ptr<RedisimpleObject> tmp(table_[bucket].object_.release());
    table_[bucket].object_.reset(table_[bucket].next_->object_.release());
    table_[bucket].next_.reset(table_[bucket].next_->next_.release());
    // return tmp;  // is this ok??
    return std::unique_ptr<RedisimpleObject>(tmp.release());
  } else {
    std::unique_ptr<RedisimpleObject> tmp(ptr->next_->object_.release());
    ptr->next_.reset(ptr->next_->next_.release());
    return std::unique_ptr<RedisimpleObject>(tmp.release());
  }
}

int HashSetTable::exist(unsigned int bucket, RedisimpleObject* object) {
  HashSetEntry* ptr = &(table_[bucket]);
  if (ptr->object_ == nullptr) return 0;
  while (ptr) {
    if (object->compare(ptr->object_.get()) == 0) return 1;
    ptr = ptr->next_.get();
  }
  return 0;
}

int HashSetTable::delete_target(unsigned int bucket, RedisimpleObject* object) {
  if (table_[bucket].object_->compare(object) == 0) {
    if (table_[bucket].next_ != nullptr) {
      table_[bucket].object_.reset(table_[bucket].next_->object_.release());
      table_[bucket].next_.reset(table_[bucket].next_->next_.release());
    } else {
      table_[bucket].object_ = nullptr;
    }
    --entry_num_;
    return 1;
  }
  HashSetEntry* ptr = &(table_[bucket]);
  while (ptr->next_ != nullptr) {
    if (ptr->next_->object_->compare(object) == 0) {
      ptr->next_.reset(ptr->next_->next_.release());
      --entry_num_;
      return 1;
    }
    ptr = ptr->next_.get();
  }
  return 0;
}

// move entry to the new bucket (used during rehash)
// this is faster than add;
// make sure the next of `entry` is nullptr
void HashSetTable::add_without_check(int bucket,
                                     std::unique_ptr<HashSetEntry>& entry) {
  // if the bucket is empty, take the entry as the first one
  // otherwise, the entry will be added as the second entry in bucket
  HashSetEntry* head = &(table_[bucket]);
  if (head->object_ == nullptr) {
    head->object_.reset(entry->object_.release());
  } else {
    entry->next_.reset(head->next_.release());
    head->next_.reset(entry.release());
  }
}

HashSet::HashSet()
    : hash_table_(new HashSetTable(8)), expand_table_(), rehash_index_(-1) {}

RST HashSet::structure_type() { return REDISIMPLE_STRUCTURE_HASH; }
int HashSet::compare(RedisimpleObject* ro) { return log::ERROR_NOT_DEFINED; }
std::unique_ptr<RedisimpleObject> HashSet::duplicate() { return nullptr; }
int HashSet::size() {
  if (rehash_index_ >= 0)
    return hash_table_->entry_num_ + expand_table_->entry_num_;
  else
    return hash_table_->entry_num_;
}
int HashSet::hash() { return log::ERROR_NOT_DEFINED; }
// TODO: implement serialize and deserialize;
int HashSet::serialize(char* out_buf, int& offset, int& cnt) {
  return log::ERROR_NOT_DEFINED;
}
int HashSet::deserialize(char* argv[], int& offset, int& argc) {
  return log::ERROR_NOT_DEFINED;
}

int HashSet::add(std::unique_ptr<RedisimpleObject>& object) {
  int hash_val = object->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashSetTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  int result = target_table->add(bucket, object);
  check_load_factor();
  return result;
}
// return a random value in set;
RedisimpleObject* HashSet::random_value() {
  int rand_val = randint();
  int rand_bucket = rand_val & hash_table_->size_mask_;
  if (size() == 0) return nullptr;
  RedisimpleObject* result = nullptr;
  HashSetTable* target_table;
  while (result == nullptr) {
    if (rehash_index_ <= rand_bucket) {
      target_table = hash_table_.get();
    } else {
      target_table = expand_table_.get();
      rand_bucket = rand_val & expand_table_->size_mask_;
    }
    result = target_table->random_value(rand_bucket);
  }
  check_load_factor();
  return result;
}
// pop a random value and delete it
std::unique_ptr<RedisimpleObject> HashSet::pop_value() {
  int rand_val = randint();
  int rand_bucket = rand_val & hash_table_->size_mask_;
  if (size() == 0) return nullptr;
  std::unique_ptr<RedisimpleObject> result;
  HashSetTable* target_table;
  while (result == nullptr) {
    if (rehash_index_ <= rand_bucket) {
      target_table = hash_table_.get();
    } else {
      target_table = expand_table_.get();
      rand_bucket = rand_val & expand_table_->size_mask_;
    }
    result = target_table->pop_value(rand_bucket);
  }
  check_load_factor();
  return result;
}
int HashSet::exist(RedisimpleObject* object) {
  int hash_val = object->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashSetTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  int result = target_table->exist(bucket, object);
  check_load_factor();
  return result;
}
int HashSet::delete_target(RedisimpleObject* object) {
  int hash_val = object->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashSetTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  int result = target_table->delete_target(bucket, object);
  check_load_factor();
  return result;
}

// check load factor of hash_table, decide whether start rehash
void HashSet::check_load_factor() {
  if (rehash_index_ >= 0) {
    rehash();
    return;
  }
  float load_factor = (float)hash_table_->entry_num_ / hash_table_->table_size_;
  // TODO: add conditions cosidering BGSAVE & BGREWRITEAOF
  if (load_factor > 2) {
    rehash_index_ = 0;
    expand_table_.reset(new HashSetTable(hash_table_->table_size_ << 1));
    rehash();
  } else if (load_factor < 0.1 && hash_table_->table_size_ > 8) {
    rehash_index_ = 0;
    expand_table_.reset(new HashSetTable(hash_table_->table_size_ >> 1));
    rehash();
  }
}
// create a new thread to handle rehash
void HashSet::rehash() {
  int end_idx =
      std::min(Config::rehash_stride + rehash_index_, hash_table_->table_size_);
  for (; rehash_index_ < end_idx; ++rehash_index_) {
    // move entries from head to tail
    HashSetEntry& head = hash_table_->table_[rehash_index_];
    std::unique_ptr<HashSetEntry> p(head.next_.release());
    // add first entry to expand table
    int bucket = head.object_->hash() & expand_table_->size_mask_;
    hash_table_->add(bucket, head.object_);
    // add following entries to expand table
    while (p != nullptr) {
      std::unique_ptr<HashSetEntry> tmp(p->next_.release());
      bucket = p->object_->hash() & expand_table_->size_mask_;
      expand_table_->add_without_check(bucket, p);
      p.reset(tmp.release());
    }
  }
  if (rehash_index_ == hash_table_->table_size_) {
    // rehash finished
    hash_table_.reset(expand_table_.release());
    rehash_index_ = -1;
  }
}

}  // namespace redisimple::object::structure