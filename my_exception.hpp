#pragma once
#include <iostream>
#include <exception>
#include <string>

namespace error{
    class MyException : public std::exception {
    private:
        std::string message; 
    public:
        explicit MyException(const std::string& msg) : message(msg) {}

        const char* what() const noexcept override;
    };
}
