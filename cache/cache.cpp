#include "cache.hpp"

namespace cache{
    cache::cache(std::chrono::seconds expiration_time, size_t max_cache, size_t min_cache) : cache_time(expiration_time),  max_cache_size(max_cache * 1024 * 1024), min_cache_size(min_cache * 1024  *1024){
        gc_thread = std::thread(&cache::garbage_collector, this);
    }

    void cache::garbage_collector(){
        while (!stop_gc) {

            std::unique_lock<std::mutex> lock(gc_mutex);
            gc_cv.wait_for(lock, std::chrono::milliseconds(5000), [this]() { return gc_wake_up || stop_gc; });
            gc_wake_up = false; // Сбрасываем флаг пробуждения

            if (stop_gc) {
                break;
            }

            {
            // Освобождаем место в кэше
            std::unique_lock<std::shared_mutex> cache_lock(cache_mutex);

             auto now = std::chrono::steady_clock::now();
            std::cout << "CASH HAVE " << cache_map.size() << " ATTEMPS" << std::endl;
            for (auto it = cache_map.begin(); it != cache_map.end();) {
                
                if(it->second->entry_mutex.try_lock()){

                    auto entry = it->second;
                    std::chrono::seconds different;

                    different = std::chrono::duration_cast<std::chrono::seconds>(now - entry->data_manager-> give_time());

                    if (different > cache_time) {
                        current_cache_size -= entry->size;
                        it->second->entry_mutex.unlock();
                        it = cache_map.erase(it);

                        std::cout << "GC: Removed old cache entry\n";
                    } else {
                        it->second->entry_mutex.unlock();
                        ++it;
                    }

                } else {
                    std::cout << "CAN'T TO INTO" << std::endl;
                    ++it;
                }
            }
            }
        }
    }

    std::shared_ptr<cache_entry> cache::add_entry(const std::string& key, size_t size){

        std::unique_lock<std::shared_mutex> lock(cache_mutex);
        

        auto it = cache_map.find(key);
        if (it != cache_map.end()) {
            return it->second; // Возвращаем существующую запись
        }
        
        if(size > max_cache_size - current_cache_size){
            return nullptr;
        }
        else{
            current_cache_size += size;
        }
        auto entry = std::make_shared<cache_entry>(size);
        cache_map[key] = entry;
        std::cout << "Create cash " << size << std::endl;
        return entry; 
    }

    bool cache::remove_entry(const std::string& key){
        std::unique_lock<std::shared_mutex> lock(cache_mutex);

        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return false; // Запись не найдена
        }

        if(!it->second->entry_mutex.try_lock()){
            return false;
        }
        current_cache_size -= it -> second ->size;
        it->second->entry_mutex.unlock();
        cache_map.erase(it);
        return true;
    }

    std::shared_ptr<cache_entry> cache::get_entry(const std::string& key){
        std::shared_lock<std::shared_mutex> lock(cache_mutex);

        auto it = cache_map.find(key);
        if(it == cache_map.end()){
            return nullptr;
        }

        return it->second;
    }

    void cache::delete_all_cache(){
        std::cout << "I must delete all cache there" << std::endl;
    }

    cache::~cache()
    {
        stop_gc = true;
        gc_cv.notify_all();

        if(gc_thread.joinable()){
            gc_thread.join();
        }
        delete_all_cache();
    }
}

