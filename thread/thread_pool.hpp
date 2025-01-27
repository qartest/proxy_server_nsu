#include <memory.h>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "task.hpp"
#include "../utils/my_exception.hpp"

namespace thread{

    class thread_pool
    {
        
    private:
        std::vector<std::thread> threads;
        std::queue<std::unique_ptr<task::task>> tasks;

        std::mutex mutex_tasks;
        std::condition_variable threadManager;
        std::atomic_bool run{true};

        std::atomic_bool run_task{true};

        void work();
    public:
        thread_pool(int size);

        void add_task(std::unique_ptr<task::task> task);
        void clear();
        void stop();
        std::atomic_bool& give_for_task();

        // int size_task();
        ~thread_pool();
    };
        
}