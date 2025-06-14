#pragma once
#include <string>
#include <vector>

enum class Priority { Low, Medium, High };
enum class Status { Active, Done };

struct Task {
    std::string title;
    std::string description;
    Priority priority;
    Status status;
    std::string deadline;
    std::vector<std::string> tags;

    Task(const std::string& t, const std::string& d, Priority p, Status s,
         const std::string& dl, const std::vector<std::string>& tg)
        : title(t), description(d), priority(p), status(s), deadline(dl), tags(tg) {}
};
