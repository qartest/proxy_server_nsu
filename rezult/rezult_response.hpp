#pragma once
#include <unistd.h> 
#include <string>

namespace answer{
    class rezult_response
    {
    private:
        bool is_200;
        bool is_version;
        bool is_300;

        std::string location;

        ssize_t size_answer;
        ssize_t size_header;
    public:
        rezult_response(bool answer_200, bool version, ssize_t answer, ssize_t header, bool answer_300, std::string_view location_in);
        ~rezult_response() = default;

        bool get_200();
        bool get_version();

        std::string get_location();
        bool get_300();

        ssize_t get_size();
        ssize_t get_header();
    };
}