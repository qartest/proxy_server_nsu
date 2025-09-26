#include "rezult_request.hpp"

namespace answer{
    rezult_request::rezult_request(bool get, bool version, std::string_view input_host, std::string_view input_uri){
        is_GET = get;
        is_version_1 = version;
        host = input_host;
        uri = input_uri;
    }

    bool rezult_request::get_GET(){
        return is_GET;
    }
    bool rezult_request::get_version(){
        return is_version_1;
    }
    std::string_view rezult_request::get_host(){
        return host;
    }

    std::string_view  rezult_request::get_uri(){
        return uri;
    }
}