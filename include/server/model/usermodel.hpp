#ifndef USERMODEL_HPP
#define USERMODEL_HPP
#include "user.hpp"

class UserModel
{
public:
    // User表的CRUD操作
    bool insert(User &user);            // 插入用户信息
    User query(int id);                 // 根据id查询用户信息
    bool updateState(const User &user); // 更新用户状态
    void resetState();                  // 重置用户状态
};

#endif