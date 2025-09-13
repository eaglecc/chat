#pragma once

#include "group.hpp"
#include <string>
#include <vector>

using namespace std;

class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    void addGroup(int userId, int groupId, const string &role);
    // 查询用户所在的群组信息
    vector<Group> queryGroups(int userId);
    // 根据群组ID查询群组用户id列表
    std::vector<int> queryGroupsUsers(int userId, int groupId);

};