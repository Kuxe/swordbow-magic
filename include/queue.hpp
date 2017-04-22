#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <queue>
#include <cstdio>
#include <mutex>
#include <condition_variable>
#include "semaphore.hpp"

/** A thread-safe queue which supports one read-thread and one write-thread
	manipulating the queue concurrently. In other words, the thread-safe queue can
	be used in the consumer-producer problem with one consumer and one producer. **/
template<typename T>
class Queue {
	const size_t size;
	T* const arr;
	int start = 0, end = 0;
	Semaphore write, read;
	std::mutex writemtx, readmtx;
public:

	explicit Queue(const size_t size) : size(size), arr(new T[size]), write(size), read(0) { }
	virtual ~Queue() {
		delete[] arr;
	}

	/** Adds an element to the queue. If queue is full,
		block the calling thread until queue is not full
		and then add the object, unblocking the calling thread.
		If more than one thread calls add concurrently, behaviour is undefined. **/
	void add(const T& obj) {
		std::lock_guard<std::mutex> lg(writemtx);
		write.acquire();
		arr[end++] = obj;
		end %= size;
		read.release();
	}

	/** Read first element in queue. If there is no first element
		in the queue, blocks the calling thread until there is an
		element in the queue which is then returned **/
	const T& front() {
		std::lock_guard<std::mutex> lg(readmtx);
		read.acquire();
		read.release();
		return arr[start%size];
	}

	/** Pops an element from the queue. If there is no element
		to pop, block until there is an element to pop and then pop it,
		unblocking calling thread.
		If more than one thread calls pop concurrently, behaviour is undefined **/
	void pop() {
		std::lock_guard<std::mutex> lg(readmtx);
		read.acquire();
		start++;
		start %= size;
		write.release();
	}

	/** Copies first element in queue and erases it from queue.
		If there is no element to copy, block until there is an
		element to copy and then copy it, unblocking calling thread.
		So take() is an atomic front() and pop() operation **/
	T take() {
		std::lock_guard<std::mutex> lg(readmtx);
		read.acquire();
		T retval = arr[start++];
		start %= size;
		write.release();
		return retval;
	}

	/** Check if queue is empty or not **/
	bool empty() const { return !read.available(); }

	/** Check if queue is full or not **/
	bool full() const { return !write.available(); }
};

#endif //QUEUE_HPP