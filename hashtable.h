#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cstddef>
#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
//#include <cassert> // useful for debugging!

// A bucket's status tells you whether it's empty, occupied, 
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() : num_elements(0), num_deleted(0), buckets(20) {}

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {
        // TODO
        //hash and compress key into valid index i using Hasher
        //probe index i
        size_t index = probe(key);
        if (index != static_cast<size_t>(-1)) {
            return buckets[index].val;
        }
        return insert_new(key, V());
    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        size_t index = probe(key);
        if(index != static_cast<size_t>(-1)) {
            return false;//means it exists, search did NOT fail
        }
        insert_new(key,val);
        return true;
    }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        size_t index = probe(key);
        if(index == static_cast<size_t>(-1)) {
            return 0;
        }

        buckets[index].status = Status::Deleted;
        --num_elements;
        ++num_deleted;
        return 1;
    }

private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;

    // void rehash_and_grow() {
    //    std::vector<Bucket> copy = buckets;
    //    buckets.clear();
    //    buckets.resize(copy.size()*2);
    //    num_elements = 0;
    //    //if bucket filled insert, else no.
    //     for (auto i : copy) {
    //         if (i.status == Status::Occupied)
    //             insert(i.key, i.val);
    //     }
    // }
    void rehash_and_grow() {
        size_t new_size = buckets.size() * 2;
        std::vector<Bucket> old_buckets = std::move(buckets);
        buckets = std::vector<Bucket>(new_size);

        num_elements = 0;
        num_deleted = 0;

        for (const auto& bucket : old_buckets) {
            if (bucket.status == Status::Occupied) {
                insert(bucket.key, bucket.val);
            }
        }
    }

    // You can add methods here if you like.
    // TODO
    size_t probe(const K& key) {
        if (buckets.empty()) return static_cast<size_t>(-1); // Safeguard
        Hasher hasher;
        size_t index = hasher(key) % buckets.size();
        size_t original_index = index;
        
        while (buckets[index].status != Status::Empty) {
            if (buckets[index].status == Status::Occupied && buckets[index].key == key) {
                return index;
            }
            index = (index + 1) % buckets.size();
            if (index == original_index) break;
        }
        return static_cast<size_t>(-1);
    }

    V& insert_new(const K& key, const V& val) {
        if(num_elements + num_deleted >= buckets.size()/2) {
            rehash_and_grow();
        }
        Hasher hasher;
        size_t index = hasher(key) % buckets.size();
        size_t og = index;
        while(buckets[index].status == Status::Occupied) {
            index = (index+1) % buckets.size();
            if(index == og) {break;}
        }
        buckets[index] = {Status::Occupied, key, val};
        num_elements++;
        return buckets[index].val;
    }
};

#endif // HASHTABLE_H