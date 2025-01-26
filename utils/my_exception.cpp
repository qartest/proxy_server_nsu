#include "my_exception.hpp"

namespace error{
    const char* MyException:: what() const noexcept{
        return message.c_str();
    }
}