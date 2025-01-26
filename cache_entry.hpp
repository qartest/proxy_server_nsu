#pragma once

#include <unordered_map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <iostream>
#include <atomic>

#include "data_load_manager.hpp"

namespace cache{
    class cache_entry
    {
    public:
        std::shared_ptr<char[]> data;
        size_t size;

        std::shared_mutex entry_mutex;
        std::shared_ptr< manager::data_load_manager> data_manager;

        cache_entry(size_t size);
        ~cache_entry() = default;
    };

    
}