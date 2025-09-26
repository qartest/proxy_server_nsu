cmake_minimum_required(VERSION 3.28)

project(ProxyServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    main.cpp
    my_exception.cpp
    proxy.cpp
)

add_executable(proxy_server main.cpp utils/my_exception.cpp proxy/proxy.cpp utils/parser_http.cpp rezult/rezult_request.cpp thread/thread_pool.cpp thread/task.cpp rezult/rezult_response.cpp cache/cache_entry.cpp cache/data_load_manager.cpp cache/cache.cpp)

target_compile_options(proxy_server PRIVATE -Wall -Wextra -pedantic)