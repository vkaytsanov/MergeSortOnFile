struct ThreadPool {
	std::function<void(uint64_t)> main_working_function;
	std::thread				 main_thread;
	std::atomic<bool>		 main_thread_pause;
	std::atomic<bool>		 main_thread_alive;
	std::atomic<bool>		 main_function_ready;
	std::mutex				 mtx;
	std::condition_variable  main_cv;
	uint64_t argument;
	ThreadPool() :
		main_thread(&ThreadPool::worker, this),
		main_thread_pause(true), 
		main_thread_alive(true),
		main_function_ready(false),
		argument(0){};
	~ThreadPool() {
		main_thread_alive = false;
		main_cv.notify_all();
		main_thread.join();
	}
	bool isReady() { return !main_thread_pause; }
	bool setWork(const std::function<void(uint64_t)>& working_function, const uint64_t& args) {
		if (!main_function_ready && main_thread_pause.exchange(false)) {
			main_working_function = working_function;
			main_function_ready = true;
			main_cv.notify_one();
			this->argument = args;
			return true;
		}
		else {
			return false;
		}
	}
	void worker() {
		std::unique_lock<std::mutex> lock(mtx);
		while (main_thread_alive) {
			if (!main_thread_pause && main_function_ready) {
				main_working_function(argument);
				main_thread_pause = true;
				main_function_ready = false;

			}
			else {
				//std::this_thread::sleep_for(std::chrono::milliseconds(5));
				main_cv.wait(lock);
			}
		}
	}
};