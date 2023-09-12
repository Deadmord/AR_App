#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

//���������������� �������
template<typename T>
class ThreadSafeQueue
{
private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;

public:

	//���������� �������� � �������
	void Push(T const& value) {
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push(value);
		lock.unlock();
		cond_.notify_one();
	}

	//�������� �������� �� �������
	bool Try_pop(T& image) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (queue_.empty()) {
			return false;
		}
		image = queue_.front();
		queue_.pop();
		return true;
	}

	//�� �� ��� � Try_pop, �� ���� ������� �����, ���� ���� �� �������� ������� (��������� �����)
	void Wait_and_pop(T& image) {
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.empty()) {
			cond_.wait(lock);
		}
		image = queue_.front();
		queue_.pop();
	}

};

