#pragma once
#include <memory>
#include <sys/types.h>   
#include <sys/socket.h>  
#include <unistd.h> 
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h>
#include <fcntl.h>

#include "../rezult/rezult_request.hpp"
#include "../rezult/rezult_response.hpp"
#include "../utils/parser_http.hpp"
#include "../cache/cache.hpp"

namespace task{
    class task
    {
    private:
        int client;
        int server = -1;

        const int startBufferSize = 16 * 1024;

        bool& shutdown;
        std::shared_ptr<cache::cache> my_cache;

        ssize_t readData(int fd, std::shared_ptr<char[]> buffer, ssize_t size);
        ssize_t send_data(int fd, char* buffer, ssize_t size);
        bool connect_to_server(int port, std::string_view host, int* sock);

        void read_and_write_without_cache(std::shared_ptr<char[]> buffer, ssize_t data_from_server);
        void write_with_cash(std::shared_ptr<char[]> buffer, ssize_t data_from_server, std::shared_ptr<cache::cache_entry> cache_entry);
        void read_from_cache_without_write(std::shared_ptr<cache::cache_entry> now_cache);
        void read_from_cache_with_write(std::shared_ptr<cache::cache_entry> now_cache);

        bool set_non_blocking(int socket);
    public:

        void run();
        task(int client1, std::shared_ptr<cache::cache> cache, bool& shut);
        ~task();

        
    };
    
}