#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>
#include <string>
#include "Task.h"

class TaskManager {
public:
    // subject is now a required field alongside title and dueDate
    void addTask(const std::string& title, const std::string& subject,
                 const std::string& dueDate, Priority priority);
    bool removeTask(int index);
    bool markComplete(int index);
    void sortByPriority();
    void sortByDueDate();
    void sortBySubject();           // NEW: group tasks by course

    const std::vector<Task>& getTasks() const;

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

private:
    std::vector<Task> tasks;
};

#endif
