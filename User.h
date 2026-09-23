#pragma once

#include <string>

class User
{
public:
    long long id;
    std::string name;
    std::string email;
    std::string passwordHash;
    std::string role;

    User()
    {
        id = 0;
        role = "BUYER";
    }
};