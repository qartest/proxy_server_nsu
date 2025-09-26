#pragma once
#include <atomic>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <shared_mutex>

namespace manager{
    class data_load_manager
    {
    private:
    
    public:
        
        std::shared_mutex rw_mutex;

        std::shared_mutex time_mutex;
        std::chrono::steady_clock::time_point last_access_time;

        std::shared_mutex data_mutex;
        size_t current_data = 0;

        std::atomic_bool terminated{false};
       
        std::atomic_bool is_download{false};

        void update_time();
        std::chrono::steady_clock::time_point give_time();

        void terminated_true();
        bool is_terminated();

        size_t give_current_data();
        void set_current_data(size_t plus);

        data_load_manager();
        ~data_load_manager() = default;
    };    
}