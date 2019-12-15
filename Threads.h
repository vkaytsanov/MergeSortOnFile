struct ThreadPool {
	std::function<void(std::vector<uint64_t>&, uint64_t, uint64_t)> main_working_function;
	std::thread				 main_thread;
	std::atomic<bool>		 main_thread_pause;
	std::atomic<bool>		 main_thread_alive;
	std::atomic<bool>		 main_function_ready;
	std::mutex				 mtx;
	std::condition_variable  main_cv;
	std::vector<uint64_t> argument0;
	uint64_t argument1;
	uint64_t argument2;
	ThreadPool() :
		main_thread(&ThreadPool::worker, this),
		main_thread_pause(true), 
		main_thread_alive(true),
		main_function_ready(false),
		argument1(0),
		argument2(0){};
	~ThreadPool() {
		main_thread_alive = false;
		main_cv.notify_all();
		main_thread.join();
	}
	bool isReady() { return !main_thread_pause; }
	bool setWork(const std::function<void(std::vector<uint64_t>&, uint64_t, uint64_t)>& working_function, const std::vector<uint64_t>& args0, const uint64_t& args1, const uint64_t& args2) {
		if (!main_function_ready && main_thread_pause.exchange(false)) {
			main_working_function = working_function;
			main_function_ready = true;
			this->argument0 = args0;
			this->argument1 = args1;
			this->argument2 = args2;
			main_cv.notify_all();
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
				main_working_function(argument0, argument1, argument2);
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