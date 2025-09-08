#ifndef USER_HPP
#define USER_HPP
#include <string>

// User表的ORM类
class User
{
public:
    User(){};
    User(int id, const std::string &name, const std::string &password, const std::string &state)
        : id(id), name(name), password(password), state(state) {}

    int getId() const { return id; }
    void setId(int newId) { id = newId; }

    std::string getName() const { return name; }
    void setName(const std::string &newName) { name = newName; }

    std::string getPassword() const { return password; }
    void setPassword(const std::string &newPassword) { password = newPassword; }

    std::string getState() const { return state; }
    void setState(const std::string &newState) { state = newState; }

private:
    int id {-1};
    std::string name {""};
    std::string password {""};
    std::string state {"offline"};
};

#endif