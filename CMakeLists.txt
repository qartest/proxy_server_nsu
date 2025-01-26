cmake_minimum_required(VERSION 3.28)

project(ProxyServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    main.cpp
    my_exception.cpp
    proxy.cpp
)

add_executable(proxy_server main.cpp my_exception.cpp proxy.cpp parser_http.cpp rezult_request.cpp thread_pool.cpp task.cpp rezult_response.cpp cache_entry.cpp data_load_manager.cpp cache.cpp)

target_compile_options(proxy_server PRIVATE -Wall -Wextra -pedantic)