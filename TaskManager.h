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
    bool setComplete(int index, bool complete);

    // Four well-defined sorts — each has a primary key then 3 tiebreakers
    void sortByDeadline();    // deadline → priority → subject → entry
    void sortByPriority();    // priority → deadline → subject → entry
    void sortBySubject();     // subject  → deadline → priority → entry
    void sortByEntryOrder();  // restores original insertion order

    const std::vector<Task>& getTasks() const;
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

private:
    std::vector<Task> tasks;
    int nextEntry = 0;  // increments with every addTask call
};

#endif
