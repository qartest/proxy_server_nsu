#include "thread_pool.hpp"
#include <functional>
#include <iostream>

namespace thread{
    thread_pool::thread_pool(int size){
        for(int i = 0; i < size; ++i){
            threads.emplace_back(&thread_pool::work, this);
        }
    }
    void thread_pool::work(){
        
        while(run){
            
            std::unique_ptr<task::task> task;
            {
                std::unique_lock<std::mutex> lock(mutex_tasks);

                threadManager.wait(lock, [this] { return !tasks.empty() || !run; });

                if(!run){
                    break;
                }
                task = std::move(tasks.front());
                tasks.pop();
            }
            try{
                task -> run();
            } catch (error::MyException& e){
                std::cout << "Task terminated\n" << e.what() << std::endl;
            }
            
        }

    }

    void thread_pool::clear(){
        std::unique_lock<std::mutex> lock(mutex_tasks);
        tasks = std::queue<std::unique_ptr<task::task>>();
    }

    void thread_pool::add_task(std::unique_ptr<task::task> task){
        {
            std::unique_lock<std::mutex> lock(mutex_tasks);
            tasks.push(std::move(task));
        }
        // std::cout << "Task added to queue, notifying one thread\n";
        threadManager.notify_one();
    }
    
    void thread_pool::stop(){
        run = false;
        run_task = false;
        threadManager.notify_all();
        for(auto& thread : threads){
            thread.join();
            std::cerr << "Thread terminated\n";
        }
    }

    std::atomic_bool& thread_pool::give_for_task(){
        return  run_task;
    }
    thread_pool::~thread_pool(){

    }
}