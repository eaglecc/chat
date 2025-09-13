#pragma once
#include <string>
#include <vector>
#include "groupuser.hpp" 

class Group {
public:
    Group() = default;
    Group(int groupId, const std::string &groupName, const std::string &groupDesc)
        : id(groupId), name(groupName), desc(groupDesc) {};

    void setId(int groupId) { id = groupId; }
    int getId() const { return id; }

    void setName(const std::string &groupName) { name = groupName; }
    std::string getName() const { return name; }

    void setDesc(const std::string &groupDesc) { desc = groupDesc; }
    std::string getDesc() const { return desc; }

    std::vector<GroupUser> &getUsers() { return users; }

private:
    int id = 0;                 // 群组ID
    std::string name = "";       // 群组名称
    std::string desc = "";       // 群组描述
    std::vector<GroupUser> users = {}; // 群组成员列表
};