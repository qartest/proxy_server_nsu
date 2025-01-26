#pragma once

#include <thread>
#include <condition_variable>
#include <atomic>
#include "cache_entry.hpp"

namespace cache{
    class cache
    {
    private:
       
       std::unordered_map<std::string, std::shared_ptr<cache_entry>> cache_map;

       static constexpr std::chrono::milliseconds gc_intterval{500};

       std::chrono::seconds cache_time;
       size_t max_cache_size;
       size_t min_cache_size;
       size_t current_cache_size = 0;

       std::condition_variable gc_cv;
       std::shared_mutex cache_mutex;
       
       std::atomic_bool gc_wake_up = false;
       std::atomic_bool stop_gc = false;

        std::mutex gc_mutex;
        std::thread gc_thread;
        void garbage_collector();

        void delete_all_cache();

    public:

        std::shared_ptr<cache_entry> add_entry(const std::string& key, size_t size);
        std::shared_ptr<cache_entry> get_entry(const std::string& key);

        bool remove_entry(const std::string& key);

        cache(std::chrono::seconds expiration_time, size_t max_cache, size_t min_cache);
        ~cache();
    };
    
    
}