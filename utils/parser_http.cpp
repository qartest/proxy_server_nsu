#include "parser_http.hpp"

namespace parser{
    namespace request{
        bool isGetMethod(std::string_view input){
            return (input.find("GET ") != std::string::npos);
        }

        bool isNormalVersion(std::string_view input){
            if(input.find("HTTP/1.0") != std::string::npos){
                return true;
            }
            if(input.find("HTTP/1.1") != std::string::npos){
                return true;
            }
            return false;
        }

        std::string_view getHost(std::string_view input){
            std::string_view hostPrefix = "Host: ";

            size_t hostPos = input.find(hostPrefix);
            if (hostPos == std::string_view::npos) {
                return "";
            }

            hostPos += hostPrefix.length();

            size_t endPos = input.find("\r\n", hostPos);
            if (endPos == std::string_view::npos) {
                return "";  
            }
            std::string_view host = input.substr(hostPos, endPos - hostPos);

            size_t protocolPos = host.find("://");
            if (protocolPos != std::string_view::npos) {
                host.remove_prefix(protocolPos + 3);
            }

            return host;

        }

        std::string_view get_uri(std::string_view request) {
            size_t start = request.find(' ');
            if (start == std::string_view::npos) return ""; 

            start += 1; // Пропускаем пробел.
            size_t end = request.find(' ', start);
            if (end == std::string_view::npos) return ""; 

            return std::string_view(request.data() + start, end - start);
        }

        std::unique_ptr<answer::rezult_request> parser(std::string_view input){
            return std::make_unique<answer::rezult_request>(isGetMethod(input), isNormalVersion(input), getHost(input), get_uri(input));
        }
    }

    namespace response{
        bool isNormalVersion(std::string_view input){
            if(input.find("HTTP/1.0") != std::string::npos){
                return true;
            }
            if(input.find("HTTP/1.1") != std::string::npos){
                return true;
            }
            return false;
        }
        bool is_300_code(std::string_view input) {
            size_t start = input.find(' ');
            if (start == std::string_view::npos || start + 3 > input.size()) {
                return false; 
            }

            return input[start + 1] == '3' && input[start + 2] >= '0' && input[start + 2] <= '9';
        }


        bool is_200_code(std::string_view input){
            size_t start = input.find(' ') + 1;
            size_t end = input.find(' ', start);
            return ("200" == std::string_view(input.begin() + start, end - start));
        }

        ssize_t size_body(std::string_view input){
            std::string_view fun = "Content-Length: ";

            size_t content_pos = input.find(fun);
            if(content_pos == std::string_view::npos){
                return 0;
            }

            content_pos += fun.size();

            size_t end_pos = input.find("\r\n", content_pos);
            if(end_pos == std::string_view::npos){
                return 0;
            }

            std::string_view content_length = input.substr(content_pos, end_pos - content_pos);

            try{
                return static_cast<ssize_t>(std::stoi(std::string(content_length)));
            } catch( const std::exception& e){
                return 0;
            }
        }

        std::string_view getLocation(std::string_view input){
            std::string_view hostPrefix = "Location: ";

            size_t hostPos = input.find(hostPrefix);
            if (hostPos == std::string_view::npos) {
                return "";
            }

            hostPos += hostPrefix.length();

            size_t endPos = input.find("\r\n", hostPos);
            if (endPos == std::string_view::npos) {
                return "";  
            }
            std::string_view host = input.substr(hostPos, endPos - hostPos);

            size_t protocolPos = host.find("://");
            if (protocolPos != std::string_view::npos) {
                host.remove_prefix(protocolPos + 3);
            }

            return host;

        }

        ssize_t get_header_size(std::string_view response){

            size_t header_end = response.find("\r\n\r\n");
            if (header_end == std::string::npos) {
                return -1;  // Не удалось найти разделитель
            }
            return static_cast<ssize_t>(header_end + 4); 
        }
        

        std::unique_ptr<answer::rezult_response> parser(std::string_view input){
            return std::make_unique<answer::rezult_response>(is_200_code(input), isNormalVersion(input), size_body(input), get_header_size(input), is_300_code(input), getLocation(input));
        }
    }
}