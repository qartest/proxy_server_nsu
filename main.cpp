#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <csignal>
#include <memory>
#include <unistd.h>
#include <string_view>

#include "proxy/proxy.hpp"
std::string_view HELP_INFO = 
"Proxy flags:\n"
"--help - information about proxy flags;\n"
"--port=<int> - the port on which the proxy will listen (default value is 8080);\n"
"--max-client-threads=<int> - the number of threads allocated for client processing (default value is 4);\n"
"--cache-initial-size=<int(MB)> - the initial size of the cache (default value is 1 MB);\n"
"--cache-max-size=<int(MB)> - the maximum size of the cache (default value is 10 MB);\n"
"--cache-ttl=<int(seconds)> - the maximum TTL of cache record (default value is 5 seconds);\n";


size_t PORT = 8080;
size_t THREADS = 4;
size_t CACHE_MIN_SIZE = 1;
size_t CACHE_MAX_SIZE = 10;
size_t CACHE_TTL = 5;
std::shared_ptr<Proxy::Proxy> PROXY;

void sig_handler(int signal){
    if(signal == SIGINT){
        std::cout << "SIGINT" << std::endl;
    } else if(signal == SIGTERM){
        std::cout << "SIGSTERM" << std::endl;
    }
    PROXY.reset();
    std::exit(0);    
}
int initHandlers(){
    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;


    if(sigaction(SIGINT, &sa, nullptr) == -1){
        std::cerr << "Failed SIGINT handler" << std::endl;
        return 1;
    }

    if(sigaction(SIGTERM, &sa, nullptr) == -1){
        std::cerr << "Failed SIGTERM handler" << std::endl;
        return 1;
    }

    sigset_t set;
    sigemptyset(&set);

    sigaddset(&set, SIGPIPE);

    if(sigprocmask(SIG_BLOCK, &set, nullptr) == -1){
        std::cerr << "Failed to block SIGPIPE" << std::endl;
        return 1;
    }

    return 0;
}


int parseInput(int argc, char** argv){
    for(int i = 1; i < argc; ++i){
        int value;
        std::string arg(argv[i]);
        if(arg == "--help"){
            std::cout << "HELP_INFO" << std::endl;
        }else if(arg.find("--port=") == 0){
            value = std::stoi(std::string(arg.substr(7)));
            if(value < 0 || value > 65535){
                std::cout << "Ploxoy Port" << std::endl;
                return 1;
            }
            PORT = static_cast<size_t>(value);

        }else if(arg.find("--max-client-threads=") == 0){
            value = std::stoi(std::string(arg.substr(21)));
            if(value <= 0){
                std::cout << "Malo Threads, Davay Bol'she" << std::endl;
                return 1;
            }
            THREADS = static_cast<size_t>(value);

        } else if(arg.find("--cache-initial-size=") == 0){
            value = std::stoi(std::string(arg.substr(21)));
            if(value <= 0){
                std::cout << "Malo Nachal'nogo Cache,  Davay Bol'she" << std::endl;
                return 1;
            }
            CACHE_MIN_SIZE = static_cast<size_t>(value);

        } else if(arg.find("--cache-max-size=") == 0){
            value = std::stoi(std::string(arg.substr(17)));
            if(value <= 0){
                std::cout << "Malo Maksimal'nogo Cache, Davay Bol'she" << std::endl;
                return 1;
            }
            CACHE_MAX_SIZE = static_cast<size_t>(value);

        }else if(arg.find("--cache-ttl=") == 0){
            value = std::stoi(std::string(arg.substr(12)));
            if (value <= 0) {
                std::cout << "Malo vremeni, Davay Bol'she" << std::endl;
                return 1;
            } 

            CACHE_TTL = static_cast<size_t>(value);           
        } else {
            std::cout << "NOT RIGHT ARGUMENTS!!!!!!!!!" << std::endl;
            std::cout << "I DON'T TAKE THIS ARGUMENTS" << std::endl;
            return 1;
        }
    }
    if(CACHE_MAX_SIZE <= CACHE_MIN_SIZE){
        std::cout << "VERY BIG MIN_CASH" << std::endl;
        return 1;
    }
    return 0;

}
int main(int argc, char** argv){
    if(parseInput(argc, argv)){
        std::cout << "Problem with INPUT" << std::endl;
        return 1;
    }

    if(initHandlers()){
        std::cout << "problema s signals" << std::endl;
        return 1;
    }

    try{
        PROXY = std::make_shared<Proxy::Proxy>(PORT, THREADS, CACHE_MIN_SIZE, CACHE_MAX_SIZE, CACHE_TTL);
        std::cout << "SERVER WORK" << std::endl;
        PROXY ->start();
    }catch(error::MyException& e){
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}





