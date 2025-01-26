#pragma once

#include "rezult_request.hpp"
#include "rezult_response.hpp"
#include <string>
#include <memory>
#include <string_view>
#include <iostream>

namespace parser{
    namespace request{ // запрос принимаем от клиента
        std::unique_ptr<answer::rezult_request> parser(std::string_view input);
    }
    namespace response{ // ответ получаем с сервера, и пересылаем его клиенту
        std::unique_ptr<answer::rezult_response> parser(std::string_view input);
    }
}