#include "task.hpp"
#include "iostream"
#include "my_exception.hpp"



namespace task{
    task::task(int client1, std::shared_ptr<cache::cache> cache, bool& shut) : shutdown(shut){
        client = client1;
        my_cache = cache;
    }

    bool task::set_non_blocking(int socket){
        int flags = fcntl(socket, F_GETFL, 0);
        if (flags == -1) {
            return false;
        }

        if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
            return false;
        }

        return true;
    }
    
    void task::run(){
        std::shared_ptr<char[]> buffer = std::shared_ptr<char[]>(new char[startBufferSize], std::default_delete<char[]>());

        if(!set_non_blocking(client)){
            std::cout << "Client not non_blocking" << std::endl;
            throw error::MyException("Cl");
        }
        
        ssize_t first_read_data = readData(client, buffer, startBufferSize);
        // std::cout << "READ CLIENT" << std::endl;

        if(first_read_data < 0){
            std::cout << "Ne prochital cho-ho" << std::endl;
            return;
        }

        if(first_read_data != startBufferSize){
            buffer.get()[first_read_data] = '\0';
        }

        std::string_view data = buffer.get();

        std::unique_ptr<answer::rezult_request> answer = parser::request::parser(data);

        if(!(answer -> get_GET() && answer -> get_version() && (answer -> get_host() != ""))){
            // std::cout << "Ploxo s zaprosom" << std::endl << "get is " << answer -> get_GET() << std::endl << "version is " << answer->get_version() << std::endl << "host is " << answer ->get_host() << std::endl;
            throw error::MyException("Bad request");
        }

        std::string key = std::string(answer ->get_host()).append(answer -> get_uri());

        std::shared_ptr<cache::cache_entry> now_cache;

        if((now_cache = my_cache ->  get_entry(key)) != nullptr){
            
            std::unique_lock<std::shared_mutex> entry_lock(now_cache -> entry_mutex);
            bool is_download;
            {
                is_download = now_cache -> data_manager -> is_download;
            }
        
            if(is_download){
                // std::cout << "READ FROM CACHE WITHOT WRITE" << std::endl;
                read_from_cache_without_write( now_cache);
            }
            else{
                // std::cout << "READ FROM CACHE WITH WRITE" << std::endl;
                read_from_cache_with_write(now_cache);
            }
            // std::cout << "RETAKE CASH" << std::endl;
            
        }
         else{

            if(!connect_to_server(80, answer -> get_host(), &server)){
                std::cout << "Ne poluchiloc' podkluychit'cay" << std::endl;
                throw error::MyException("Big cock");
            }

            if(!send_data(server, buffer.get(), first_read_data)){
                std::cout << "Problem s otpravkoy severu" << std::endl;
                throw error::MyException("1 send to server problem");
            }

            if(!set_non_blocking(server)){
                std::cout << "server not non_blocking" << std::endl;
                throw error::MyException("server");
            }

            ssize_t data_from_server = readData(server, buffer, startBufferSize);

            if(data_from_server <= 0){
                throw error::MyException("Server don't answer first time");
            }

            if(data_from_server != startBufferSize){
                buffer[data_from_server] = '\0';
            }
            
            std::string_view data1 = buffer.get();

            std::unique_ptr<answer::rezult_response> server_answer = parser::response::parser(data1);


            if(server_answer -> get_200() && server_answer -> get_version()){
                
                if(server_answer -> get_size() > 0){
                    // std::cout << "WRITE WITH CASH" << std::endl;
                    std::shared_ptr<cache::cache_entry> cache_entry = my_cache -> add_entry(key, server_answer -> get_header() + server_answer -> get_size());

                    if(cache_entry == nullptr){
                        read_and_write_without_cache(buffer, data_from_server);
                    }
                    else{
                        std::unique_lock<std::shared_mutex> entry_lock(cache_entry -> entry_mutex);
                        write_with_cash(buffer, data_from_server, cache_entry);
                    }

                }else{
                    // std::cout << "WRITE WITHOUT CASH" << std::endl;
                    read_and_write_without_cache(buffer, data_from_server);
                }
            }else{
                read_and_write_without_cache(buffer, data_from_server);
            }

            // std::cout << "WRITE WITHOUT CASH" << std::endl;


            // std::cout << "WRITE FINISHED" << std::endl;
        }

        std::cout << "TASK FINISHED" << std::endl;
    }


    void task :: read_from_cache_with_write( std::shared_ptr<cache::cache_entry> now_cache){

        ssize_t current_read = 0;
        ssize_t size_cache = 0;
        ssize_t current_write_cache = 0;
        {
            size_cache = now_cache -> size;         
            current_write_cache = now_cache -> data_manager -> give_current_data();
        }

        ssize_t remainder;
        while(current_read < size_cache){

            if(now_cache -> data_manager -> is_terminated()){
                throw error::MyException("Terminated read with cash");
            }

            current_write_cache = now_cache -> data_manager -> give_current_data();

            if(current_write_cache - current_read >= startBufferSize){
                remainder = startBufferSize;
            }
            else{
                remainder = current_write_cache - current_read;
            }

            if(remainder == 0){
                continue;
            }

            // std::cout << "I READ " << current_read << std::endl;
            {
                std::shared_lock<std::shared_mutex> lock(now_cache -> data_manager -> rw_mutex);
                if(send_data(client, now_cache -> data.get() + current_read, remainder) != remainder){
                    throw error::MyException("Problem with cash data to client with read");
                } 
            }

            current_read += remainder;
            
        }
    }

     void task :: write_with_cash(std::shared_ptr<char[]> buffer, ssize_t data_from_server, std::shared_ptr<cache::cache_entry> cache_entry){

        ssize_t write_to_cash = 0;
        ssize_t cash_size = 0;
        bool is_client_connect = true;

        {
            std::unique_lock<std::shared_mutex> lock(cache_entry -> data_manager -> rw_mutex);

            std::memcpy(cache_entry ->data.get(), buffer.get(), data_from_server);

            cache_entry -> data_manager -> set_current_data(data_from_server);
            
            cash_size = cache_entry -> size;
        }

        write_to_cash += data_from_server;

        if(send_data(client, buffer.get(), data_from_server) != data_from_server){
            is_client_connect = false;
            close(client);
            // std::cout << "CLOSE CLIENT" << std::endl;
        }


        while (write_to_cash < cash_size)
        {   
            data_from_server = readData(server, buffer, startBufferSize);

            if(data_from_server <= 0){
                std::cout << "BIG CRUSH CASH" << std::endl;
                throw error::MyException("BIG CRUSH");
            }

            {
                std::unique_lock<std::shared_mutex> lock(cache_entry -> data_manager -> rw_mutex);

                std::memcpy(cache_entry -> data.get() + write_to_cash, buffer.get(), data_from_server);
                
                cache_entry -> data_manager -> set_current_data(data_from_server);

                write_to_cash += data_from_server;

                // std::cout << "I WRITE " << write_to_cash << std::endl;
            }

            if(is_client_connect){
                if(send_data(client, buffer.get(), data_from_server) != data_from_server){
                    is_client_connect = false;
                    close(client);
                    client = -1;
                }
            }
        }

        {
            cache_entry -> data_manager -> is_download = true;
        }

        return;
    }

    void task :: read_from_cache_without_write( std::shared_ptr<cache::cache_entry> now_cache){
        
        ssize_t current_read = 0;
        ssize_t size_cache = 0;
        
        size_cache = now_cache ->size;
        ssize_t remainder;

        while(current_read < size_cache){
            
            if(size_cache - current_read >= startBufferSize){
                remainder = startBufferSize;
            }
            else{
                remainder = size_cache - current_read;
            }
            {
                std::shared_lock<std::shared_mutex> lock(now_cache -> data_manager -> rw_mutex);
                if(send_data(client, now_cache -> data.get() + current_read, remainder) != remainder){
                    throw error::MyException("Problem with cash data to client with read");
                } 
            }
            
            current_read += remainder;
        }
        
    }

    void task :: read_and_write_without_cache(std::shared_ptr<char[]> buffer, ssize_t data_from_server){
        ssize_t now_data_size = data_from_server;
        do{
            if(send_data(client, buffer.get(), now_data_size) != now_data_size){
                return;
            }
        }while((now_data_size = readData(server, buffer, startBufferSize)) > 0);

    }

    void task :: from_server_to_client(std::shared_ptr<char[]> buffer){
        ssize_t data_from_server;
        while ((data_from_server = readData(server, buffer, startBufferSize)) >= 0)
        {
            if(send_data(client, buffer.get(), data_from_server) != data_from_server){
                return;
            }
        }
        
    }

    ssize_t task :: readData(int fd, std::shared_ptr<char[]> buffer, ssize_t size){
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        struct timeval timeout = { 0, 500000 };  // Тайм-аут 40 мс
        int max_retries = 50;  // Максимальное количество попыток

        for (int attempt = 0; attempt < max_retries; ++attempt) {
            int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
            
            if (ret == -1) {
                // std::cerr << "select() error on read attempt " << attempt + 1 << "\n";
                return -1;
            } else if (ret == 0) {
                // std::cout << "Timeout on read attempt " << attempt + 1 << "\n";
                continue;  // Пробуем снова
            } else {
                if (FD_ISSET(fd, &read_fds)) {
                    ssize_t result = recv(fd, buffer.get(), size, 0);
                    if (result == -1) {
                        // std::cerr << "recv() error on read attempt " << attempt + 1 << "\n";
                        return -1;
                    }
                    return result;
                }
            }
    }

        // std::cerr << "Failed to read data after " << max_retries << " attempts\n";
        return -1;
    }

    ssize_t task :: send_data(int fd, char* buffer, ssize_t size){
        ssize_t total_send = 0;
        int max_retries = 50;
        int attempt = 0;

        fd_set write_fds;
        FD_ZERO(&write_fds);
        FD_SET(fd, &write_fds);

        struct timeval timeout = {0, 500000 }; 

        while (total_send < size) {
                int ret = select(fd + 1, NULL, &write_fds, NULL, &timeout);
                
                if(attempt >= max_retries){
                    return total_send;
                }
                if (ret == -1) {
                    // std::cerr << "select() error on send attempt " << attempt + 1 << "\n";
                    return total_send;
                } else if (ret == 0) {
                    // std::cout << "Timeout on send attempt " << attempt + 1 << "\n";
                    ++attempt;
                    continue;
                } else {
                    if (FD_ISSET(fd, &write_fds)) {
                        ssize_t sent = send(fd, buffer + total_send, size - total_send, 0);
                        if (sent == -1) {
                            // std::cerr << "send() error on send attempt " << attempt + 1 << "\n";
                            return total_send;
                        }
                        total_send += sent;
                        // std::cout << "Sent " << total_send << " bytes\n";
                    }
                }
            }
            return total_send;

    }

    bool task :: connect_to_server(int port, std::string_view host, int* sock){

        *sock = socket(AF_INET, SOCK_STREAM, 0);
        if (*sock == -1) {
            std::cerr << "Failed to create socket\n";
            return false;
        }

        struct hostent* server = gethostbyname(std::string(host).c_str());
        if (server == nullptr) {
            std::cerr << "No such host found\n";
            return false;
        }

        struct sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        std::memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);
        serverAddr.sin_port = htons(port);

        if (connect(*sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Connection failed\n";
            close(*sock);
            *sock = -1;
            return false;
        }

        // std::cout << "Connected to server: " << host << ":" << port << "\n";
        return true;
    }

    task :: ~task(){
        shutdown = true;
        if(client > 0){
            close(client);
        }
        
        if(server > 0){
            close(server);
        }
        
    }
}