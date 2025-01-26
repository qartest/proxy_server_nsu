#pragma once

#include<string>
#include <memory>

namespace answer{
    class rezult_request
    {
    private:
        bool is_GET;
        bool is_version_1;
        std::string_view host;
        std::string_view uri;
    public:
        rezult_request(bool get, bool version, std::string_view input_host, std::string_view input_uri);
        ~rezult_request() = default;

        bool get_GET();
        bool get_version();
        std::string_view get_host();
        std::string_view get_uri();
    };
}