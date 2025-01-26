#include "cache_entry.hpp"

namespace cache{
    cache_entry::cache_entry(size_t size) : size(size){
        if(size > 0){
            data = std::shared_ptr<char[]>(new char[size], std::default_delete<char[]>());
        }
        data_manager = std::make_shared<manager::data_load_manager>();
    }

}