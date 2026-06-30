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

bool TaskManager::markComplete(int index) {
    if (index < 0 || index >= (int)tasks.size()) return false;
    tasks[index].completed = true;
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

// Groups tasks together by subject so all "Data Structures" tasks
// appear together, then all "Literature" tasks, and so on.
void TaskManager::sortBySubject() {
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.subject < b.subject;
    });
}

const std::vector<Task>& TaskManager::getTasks() const {
    return tasks;
}

// Save format: title|subject|dueDate|priority|completed
bool TaskManager::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    for (const auto& t : tasks) {
        out << t.title << '|' << t.subject << '|' << t.dueDate << '|'
            << priorityToString(t.priority) << '|'
            << (t.completed ? 1 : 0) << '\n';
    }
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
        std::string titlePart, subjectPart, datePart, priorityPart, donePart;
        std::getline(ss, titlePart,    '|');
        std::getline(ss, subjectPart,  '|');
        std::getline(ss, datePart,     '|');
        std::getline(ss, priorityPart, '|');
        std::getline(ss, donePart,     '|');

        // Handle old save files that had no subject field
        // If priorityPart looks like a date, the file is old format —
        // shift fields and use "General" as a default subject.
        if (priorityPart.empty() && (datePart == "Low" || datePart == "Medium" || datePart == "High")) {
            priorityPart = datePart;
            datePart     = subjectPart;
            donePart     = priorityPart;
            subjectPart  = "General";
        }

        Priority p = Priority::Medium;
        if (priorityPart == "Low")  p = Priority::Low;
        else if (priorityPart == "High") p = Priority::High;

        Task t(titlePart, subjectPart, datePart, p);
        t.completed = (donePart == "1");
        tasks.push_back(t);
    }
    return true;
}
