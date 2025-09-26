#include "rezult_response.hpp"

namespace answer{
    rezult_response::rezult_response(bool answer_200, bool version, ssize_t answer, ssize_t header, bool answer_300, std::string_view location_in){
        is_200 = answer_200;
        is_version = version;
        size_answer = answer;
        size_header = header;

        is_300 = answer_300;
        location = std::string(location_in); 
    }

    bool rezult_response::get_200(){
        return is_200;
    }

    bool rezult_response::get_version(){
        return is_version;
    }

    ssize_t rezult_response::get_size(){
        return size_answer;
    }

    ssize_t rezult_response::get_header(){
        return size_header;
    }

    std::string rezult_response::get_location(){
        return location;
    }
    bool rezult_response::get_300(){
        return is_300;
    }

}