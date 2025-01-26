#pragma once
#include "my_exception.hpp"
#include <memory>
#include "thread_pool.hpp"
#include "cache.hpp"

namespace Proxy{
    class Proxy{
    public:
        Proxy(
            size_t port,
            size_t maxThreads,
            size_t minCacheSize,
            size_t maxCacheSize,
            size_t cacheTtl
        );

        void start();
        void shutdown();

        ~Proxy();

    private:
        int DescriptorSocket;

        std::unique_ptr<thread::thread_pool> thread_pool;
        std::shared_ptr<cache::cache> my_cache;
        bool shutBool;
    };
}