#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

//���������������� �������
template<typename T>
class ThreadSafeQueue
{
public:
	// �����������
	ThreadSafeQueue() {}

	//���������� �������� � �������
	void Push(const T& item) {
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push(item);
		lock.unlock();
		condition_.notify_one();		// ��������� ��������� �����, ���� ���� �����
	}

	//�������� �������� �� �������
	bool Try_pop(T& item) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (queue_.empty()) {
			return false;	// ������� �����
		}
		item = queue_.front();
		queue_.pop();
		return true;
	}

	//�� �� ��� � Try_pop, �� ���� ������� �����, ���� ���� �� �������� ������� (��������� �����)
	void Wait_and_pop(T& item) {
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.empty()) {
			condition_.wait(lock);
		}
		item = queue_.front();
		queue_.pop();
	}

	// ���������, ����� �� �������
	bool IsEmpty() const {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.empty();
	}

	// �������� ������� ������ �������
	size_t Size() const {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.size();
	}

	// �������, ���� ������� �� ������ �� ������
	void WaitForData() {
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait(lock, [this] { return !queue_.empty(); });
	}

private:
	std::queue<T> queue_;               // ������� ���������
	mutable std::mutex mutex_;         // ������� ��� ������������� �������
	std::condition_variable condition_; // �������� ���������� ��� �������� ������
};

