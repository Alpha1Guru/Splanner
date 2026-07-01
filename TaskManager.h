#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>
#include <string>
#include "Task.h"

class TaskManager {
public:
    void addTask(const std::string& title, const std::string& subject,
                 const std::string& dueDate, Priority priority);
    bool removeTask(int index);
    bool setComplete(int index, bool complete);  // replaces markComplete — can toggle
    void sortByPriority();
    void sortByDueDate();
    void sortBySubject();

    const std::vector<Task>& getTasks() const;
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

private:
    std::vector<Task> tasks;
};

#endif
