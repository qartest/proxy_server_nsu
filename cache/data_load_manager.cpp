#include "data_load_manager.hpp"


namespace manager{
    data_load_manager::data_load_manager(){
        last_access_time = std::chrono::steady_clock::now();
    }

    void data_load_manager::update_time(){
        std::unique_lock<std::shared_mutex> lock(time_mutex);
        last_access_time = std::chrono::steady_clock::now();
    }


    void data_load_manager::terminated_true(){
        terminated = true;           // Устанавливаем флаг завершения 
    }

    bool data_load_manager::is_terminated(){
        return terminated;
    }

    size_t data_load_manager::give_current_data(){
        std::shared_lock<std::shared_mutex> lock(data_mutex);
        return current_data;
    }
    void data_load_manager::set_current_data(size_t plus){
        std::unique_lock<std::shared_mutex> lock(data_mutex);

        current_data += plus;
    }

    std::chrono::steady_clock::time_point data_load_manager::give_time(){
        std::shared_lock<std::shared_mutex> lock(time_mutex);
        return last_access_time;
    }
}