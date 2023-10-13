#pragma once
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeValue
{
public:
    ThreadSafeValue() : itemAvailable_(false) {}

    // Copy constructor
    ThreadSafeValue(const ThreadSafeValue& other) {
        std::unique_lock<std::mutex> lock(other.mutex_);
        if (other.itemAvailable_) {
            item_ = other.item_;
            itemAvailable_ = true;
        }
    }

    // Copy assignment operator
    ThreadSafeValue& operator=(const ThreadSafeValue& other) {
        if (this == &other) {
            return *this;
        }
        std::unique_lock<std::mutex> myLock(mutex_);
        std::unique_lock<std::mutex> otherLock(other.mutex_);
        if (other.itemAvailable_) {
            item_ = other.item_;
            itemAvailable_ = true;
        }
        return *this;
    }

    // Move constructor
    ThreadSafeValue(T&& item)
        : item_(std::move(item)), itemAvailable_(true) {}

    // Move assignment operator
    ThreadSafeValue& operator=(T&& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        item_ = std::move(item);
        itemAvailable_ = true;
        lock.unlock();
        condition_.notify_one();
        return *this;
    }

    // Add an item
    void push(T&& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        item_ = std::move(item);
        itemAvailable_ = true;
        lock.unlock();
        condition_.notify_one();
    }

    // Remove an item
    bool tryPop(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!itemAvailable_) {
            return false;
        }
        item = std::move(item_);
        itemAvailable_ = false;
        return true;
    }

    // Same as tryPop, but waits if the item is not available
    void waitAndPop(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!itemAvailable_) {
            condition_.wait(lock);
        }
        item = std::move(item_);
        itemAvailable_ = false;
    }

    // Get the current value if available
    bool tryGet(T& item) const {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!itemAvailable_) {
            return false;
        }
        item = item_;
        return true;
    }

    //то же что и Try_Get, но если элемента нет, ждет пока не появится
    void waitAndGet(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!itemAvailable_) {
            condition_.wait(lock);
        }
        item = item_;
    }

    // Same as tryGet, but waits if the item is not available
    bool isEmpty() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return !itemAvailable_;
    }

    // Check if the item is available
    void waitForData() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return itemAvailable_; });
    }

private:
    T item_;                            // One item
    bool itemAvailable_;                // Flag indicating item availability
    mutable std::mutex mutex_;          // Mutex for synchronizing access
    std::condition_variable condition_; // Conditional variable for waiting for data
};

