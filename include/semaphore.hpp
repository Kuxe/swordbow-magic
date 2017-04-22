#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>
#include <atomic>

/** General semaphore with N permissions **/
class Semaphore {
	const size_t num_permissions;
	size_t avail;
	std::mutex m;
	std::condition_variable cv;
public:
	Semaphore(const size_t& num_permissions) : num_permissions(num_permissions), avail(num_permissions) { }

	void acquire() {
		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, [this] { return avail > 0; });
		avail--;
		lk.unlock();
	}

	void release() {
		m.lock();
		avail++;
		m.unlock();
		cv.notify_one();
	}

	size_t available() const {
		return avail;
	}
};

#endif //SEMAPHORE_HPP