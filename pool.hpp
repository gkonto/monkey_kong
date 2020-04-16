#ifndef POOL_HPP
#define POOL_HPP

#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <utility>
#include <iostream>

#define USE_POOL
//#define DBG

/*
 * Very simple memory pool.
 *
 * This memory pool allocates arenas where each arena holds up to arena_size
 * elements. All the arenas of the pool are singly-linked.
 *
 * Each element in an arena has a pointer to the next free item, except the last
 * one which is set to null. This forms another singly-linked list of available
 * storage called free_list. The free_list points to the first free storage
 * item, or null if the current arena is full.
 *
 * When an object is allocated, if the arena is not full, the storage pointed
 * by free_list is returned. Before returning, free_list is updated to point
 * to the next free item. If the arena is full, free_list is null so an arena is
 * created first which will point to the currently (full) arena. Then the
 * free_list
 * is updated to point the new storage of the arena.
 *
 * When a pointer is freed, we recover the element of the arena (using pointer
 * arithmetic) and then we put this item at the beginning of the free list.
 *
 */

template <typename T> 
struct Pool 
{
    public:
        // Creates a new pool that will use arenas of arena_size.
        Pool(size_t arena_size)
            : arena_size(arena_size), arena(new Arena(arena_size)),
            free_list(arena->get_storage()) 
        {
#ifdef DBG
            std::cout << "[DBG MSG] Creating New Pool " << this << std::endl;
#endif
        }

        ~Pool()
        {
#ifdef DBG
            std::cout << "[DBG MSG] Destructing Pool" << this << std::endl;
#endif
        }

        // Allocates an object in the current arena.
        template <typename... Args> 
        T *alloc(Args &&... args) 
        {
            if (free_list == nullptr) {
#ifdef DBG
                std::cout << "[DBG MSG] Arena full! " << std::endl;
#endif
                // If the current arena is full, create a new one.
                std::unique_ptr<Arena> new_arena(new Arena(arena_size));
                // Link the new arena to the current one.
                new_arena->set_next_arena(std::move(arena));
                // Make the new arena the current one.
                arena.reset(new_arena.release());
                // Update the free_list with the storage of the just created arena.
                free_list = arena->get_storage();
            }

            // Get the first free item.
            Item *current_item = free_list;
            // Update the free list to the next free item.
            free_list = current_item->get_next_item();

            // Get the storage for T.
            T *result = current_item->get_storage();
            // Construct the object in the obtained storage.
            new (result) T(std::forward<Args>(args)...);

            return result;
        }

        void free(T *t) {
            // Destroy the object.
            t->T::~T();

            // Convert this pointer to T to its enclosing pointer of an item of the
            // arena.
            Item *current_item = Item::storage_to_item(t);

            // Add the item at the beginning of the free list.
            current_item->set_next_item(free_list);
            free_list = current_item;
        }

private:
    // Represents an element in the arena. This is a union so this element either
    // stores a T or just is a free item which points to the next free item (if
    // any).
    union Item {
        public:
            // Methods for the list of free items.
            Item *get_next_item() const { return next; }
            void set_next_item(Item *n) { next = n; }

            // Methods for the storage of the item.
            T *get_storage() { return reinterpret_cast<T *>(datum); }

            // Given a T* cast it to a Item*
            static Item *storage_to_item(T *t) {
                Item *current_item = reinterpret_cast<Item *>(t);
                return current_item;
            }

        private:
            using StorageType = char[sizeof(T)];

            // Points to the next freely available item.
            Item *next;
            // Storage of the item. Note that this is a union
            // so it is shared with the pointer "next" above.
            StorageType datum alignas(alignof(T));
    };

    // Arena of items. This is just an array of items and a pointer
    // to another arena. All arenas are singly linked between them.
    struct Arena {
        public:
            // Creates an arena with arena_size items.
            Arena(size_t arena_size) : storage(new Item[arena_size]) {
                for (size_t i = 1; i < arena_size; i++) {
                    storage[i - 1].set_next_item(&storage[i]);
                }
                storage[arena_size - 1].set_next_item(nullptr);
                static int id = 0;
                id_ = id++;
#ifdef DBG
                std::cout << " [DBG_MSG] Allocating new Arena with id " << id_ << std::endl;
#endif
            }

            ~Arena() {
#ifdef DBG
                std::cout << "[DBG_MSG] Destructing arena id " << id_ << std::endl;
#endif
            }

            // Returns a pointer to the array of items. This is used by the arena
            // itself. This is only used to update free_list during initialization
            // or when creating a new arena when the current one is full.
            Item *get_storage() const { return storage.get(); }

            // Sets the next arena. Used when the current arena is full and
            // we have created this one to get more storage.
            void set_next_arena(std::unique_ptr<Arena> &&n) {
                assert(!next);
                next.reset(n.release());
            }

        private:
            // Storage of this arena.
            std::unique_ptr<Item[]> storage;
            // Pointer to the next arena.
            std::unique_ptr<Arena> next;
            // Arena id for dbg purposes.
            int id_;
    };

    // Size of the arenas created by the pool.
    size_t arena_size;
    // Current arena. Changes when it becomes full and we want to allocate one
    // more object.
    std::unique_ptr<Arena> arena;
    // List of free elements. The list can be threaded between different arenas
    // depending on the deallocation pattern.
    Item *free_list;
};

#endif // MINIPOOL_H
