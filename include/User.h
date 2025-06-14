#pragma once
#include "Task.h"
#include <vector>
#include <string>

class User {
public:
    User(const std::string& name);
    void add_task(const Task& task);
    void delete_task(size_t index);
    std::vector<Task> search_tasks(const std::string& keyword) const;
    const std::vector<Task>& get_tasks() const;

private:
    std::string username;
    std::vector<Task> tasks;
};
