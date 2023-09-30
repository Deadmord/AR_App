#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

//потокобезопасная очередь
template<typename T>
class ThreadSafeQueue
{
public:
	// Конструктор
	ThreadSafeQueue() {}

	//добавление элемента в очередь
	void Push(const T& item) {
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push(item);
		lock.unlock();
		condition_.notify_one();		// Уведомить ожидающий поток, если есть такой
	}

	//удаление элемента из очереди
	bool Try_pop(T& item) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (queue_.empty()) {
			return false;	// Очередь пуста
		}
		item = queue_.front();
		queue_.pop();
		return true;
	}

	//то же что и Try_pop, но если очередь пуста, ждет пока не появится элемент (блокирует поток)
	void Wait_and_pop(T& item) {
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.empty()) {
			condition_.wait(lock);
		}
		item = queue_.front();
		queue_.pop();
	}

	// Проверить, пуста ли очередь
	bool IsEmpty() const {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.empty();
	}

	// Получить текущий размер очереди
	size_t Size() const {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.size();
	}

	// Ожидать, пока очередь не станет не пустой
	void WaitForData() {
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait(lock, [this] { return !queue_.empty(); });
	}

private:
	std::queue<T> queue_;               // Очередь элементов
	mutable std::mutex mutex_;         // Мьютекс для синхронизации доступа
	std::condition_variable condition_; // Условная переменная для ожидания данных
};

