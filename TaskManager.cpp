#include "TaskManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>

void TaskManager::addTask(const std::string& title, const std::string& subject,
                          const std::string& dueDate, Priority priority) {
    tasks.emplace_back(title, subject, dueDate, priority);
}

bool TaskManager::removeTask(int index) {
    if (index < 0 || index >= (int)tasks.size()) return false;
    tasks.erase(tasks.begin() + index);
    return true;
}

bool TaskManager::setComplete(int index, bool complete) {
    if (index < 0 || index >= (int)tasks.size()) return false;
    tasks[index].completed = complete;
    return true;
}

void TaskManager::sortByPriority() {
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return static_cast<int>(a.priority) > static_cast<int>(b.priority);
    });
}

void TaskManager::sortByDueDate() {
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.dueDate < b.dueDate;
    });
}

void TaskManager::sortBySubject() {
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.subject < b.subject;
    });
}

const std::vector<Task>& TaskManager::getTasks() const { return tasks; }

bool TaskManager::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) return false;
    for (const auto& t : tasks)
        out << t.title << '|' << t.subject << '|' << t.dueDate << '|'
            << priorityToString(t.priority) << '|' << (t.completed ? 1 : 0) << '\n';
    return true;
}

bool TaskManager::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return false;
    tasks.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string titleP, subjectP, dateP, priorityP, doneP;
        std::getline(ss, titleP,    '|');
        std::getline(ss, subjectP,  '|');
        std::getline(ss, dateP,     '|');
        std::getline(ss, priorityP, '|');
        std::getline(ss, doneP,     '|');
        Priority p = Priority::Medium;
        if (priorityP == "Low")  p = Priority::Low;
        else if (priorityP == "High") p = Priority::High;
        Task t(titleP, subjectP, dateP, p);
        t.completed = (doneP == "1");
        tasks.push_back(t);
    }
    return true;
}
