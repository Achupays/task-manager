#include "User.h"
#include <algorithm>

User::User(const std::string& name) : username(name) {}

void User::add_task(const Task& task) {
    tasks.push_back(task);
}

void User::delete_task(size_t index) {
    if (index < tasks.size()) {
        tasks.erase(tasks.begin() + index);
    }
}

std::vector<Task> User::search_tasks(const std::string& keyword) const {
    std::vector<Task> results;
    for (const auto& task : tasks) {
        if (task.title.find(keyword) != std::string::npos ||
            task.description.find(keyword) != std::string::npos) {
            results.push_back(task);
        }
    }
    return results;
}

const std::vector<Task>& User::get_tasks() const {
    return tasks;
}
