#ifndef TASK_H
#define TASK_H

#include <string>

enum class Priority {
    Low,
    Medium,
    High
};

inline std::string priorityToString(Priority p) {
    switch (p) {
        case Priority::Low:    return "Low";
        case Priority::Medium: return "Medium";
        case Priority::High:   return "High";
    }
    return "Unknown";
}

struct Task {
    std::string title;
    std::string subject;   // which course this task belongs to
    std::string dueDate;
    Priority priority;
    bool completed;

    Task(std::string title, std::string subject, std::string dueDate, Priority priority)
        : title(std::move(title)), subject(std::move(subject)),
          dueDate(std::move(dueDate)), priority(priority), completed(false) {}
};

#endif
