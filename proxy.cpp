#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "proxy.hpp"

namespace Proxy{
    Proxy::Proxy(
            size_t port,
            size_t maxThreads,
            size_t minCacheSize,
            size_t maxCacheSize,
            size_t cacheTtl
        ){  
            shutBool = false;
            if(port == 0 || port > 65535){
                throw error::MyException("ploxoy port");
            }

            DescriptorSocket = socket(AF_INET, SOCK_STREAM, 0);

            if(DescriptorSocket < 0){
                throw error::MyException("Ne udalos' cozdat' socket");
            }

            sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port);
            server_addr.sin_addr.s_addr = INADDR_ANY;
            
            my_cache = std::make_shared<cache::cache>(std::chrono::seconds(cacheTtl), maxCacheSize, minCacheSize);

            std::cout << DescriptorSocket << "  " << port << std::endl;


            if(bind(DescriptorSocket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1){
                close(DescriptorSocket);
                throw error::MyException("Bind posh'ol po pope");
            }

            thread_pool = std::make_unique<thread::thread_pool>(maxThreads);

            std::cout << "Server opened on port: " << port << std::endl;
        }

    void Proxy :: start(){
        if(listen(DescriptorSocket, SOMAXCONN) == -1){
            close(DescriptorSocket);
            throw error::MyException("listen don't work");
        }

        while(true){
            int clientSocket = accept(DescriptorSocket, NULL, NULL);
            if(clientSocket > 0){
                thread_pool -> add_task(std::make_unique<task::task>(clientSocket, my_cache, shutBool));
            }
        }
    }

    void Proxy :: shutdown() {

    }

    Proxy :: ~Proxy(){

    }   
}