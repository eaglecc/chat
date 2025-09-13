#pragma once
#include "user.hpp"
#include <string>

class GroupUser : public User {
public:
    void setRole(std::string newRole) { role = std::move(newRole); }
    std::string getRole() const { return role; }

private:
    std::string role {""}; // 群组中的角色，如"admin"、"member"
};