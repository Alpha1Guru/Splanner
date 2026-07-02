#include "TaskManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Tasks with no deadline sort as if their deadline is infinitely far away.
// "9999-99-99" is a string that sorts after any real YYYY-MM-DD date.
static const std::string FAR_FUTURE = "9999-99-99";

static const std::string& eff(const std::string& d) {
    return d.empty() ? FAR_FUTURE : d;
}

void TaskManager::addTask(const std::string& title, const std::string& subject,
                          const std::string& dueDate, Priority priority) {
    Task t(title, subject, dueDate, priority);
    t.entryOrder = nextEntry++;
    tasks.push_back(t);
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

// Sort by deadline → priority → subject → entry
void TaskManager::sortByDeadline() {
    std::stable_sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        if (eff(a.dueDate) != eff(b.dueDate))
            return eff(a.dueDate) < eff(b.dueDate);                              // 1. closest deadline first
        if (a.priority != b.priority)
            return static_cast<int>(a.priority) > static_cast<int>(b.priority); // 2. highest priority first
        if (a.subject != b.subject)
            return a.subject < b.subject;                                        // 3. subject alphabetically
        return a.entryOrder < b.entryOrder;                                      // 4. earliest entry first
    });
}

// Sort by priority → deadline → subject → entry
void TaskManager::sortByPriority() {
    std::stable_sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        if (a.priority != b.priority)
            return static_cast<int>(a.priority) > static_cast<int>(b.priority); // 1. highest priority first
        if (eff(a.dueDate) != eff(b.dueDate))
            return eff(a.dueDate) < eff(b.dueDate);                              // 2. closest deadline first
        if (a.subject != b.subject)
            return a.subject < b.subject;                                        // 3. subject alphabetically
        return a.entryOrder < b.entryOrder;                                      // 4. earliest entry first
    });
}

// Sort by subject → deadline → priority → entry
void TaskManager::sortBySubject() {
    std::stable_sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        if (a.subject != b.subject)
            return a.subject < b.subject;                                        // 1. subject alphabetically
        if (eff(a.dueDate) != eff(b.dueDate))
            return eff(a.dueDate) < eff(b.dueDate);                              // 2. closest deadline first
        if (a.priority != b.priority)
            return static_cast<int>(a.priority) > static_cast<int>(b.priority); // 3. highest priority first
        return a.entryOrder < b.entryOrder;                                      // 4. earliest entry first
    });
}

// Restores the original order tasks were entered in
void TaskManager::sortByEntryOrder() {
    std::stable_sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.entryOrder < b.entryOrder;
    });
}

const std::vector<Task>& TaskManager::getTasks() const { return tasks; }

// Save format: title|subject|dueDate|priority|completed|entryOrder
bool TaskManager::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) return false;
    for (const auto& t : tasks)
        out << t.title    << '|' << t.subject  << '|' << t.dueDate << '|'
            << priorityToString(t.priority) << '|'
            << (t.completed ? 1 : 0) << '|' << t.entryOrder << '\n';
    return true;
}

bool TaskManager::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return false;
    tasks.clear();
    nextEntry = 0;
    std::string line;
    int seq = 0;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string titleP, subjectP, dateP, priorityP, doneP, entryP;
        std::getline(ss, titleP,    '|');
        std::getline(ss, subjectP,  '|');
        std::getline(ss, dateP,     '|');
        std::getline(ss, priorityP, '|');
        std::getline(ss, doneP,     '|');
        std::getline(ss, entryP,    '|');

        Priority p = Priority::Medium;
        if (priorityP == "Low")  p = Priority::Low;
        else if (priorityP == "High") p = Priority::High;

        Task t(titleP, subjectP, dateP, p);
        t.completed = (doneP == "1");

        // If an old file has no entryOrder field, assign sequentially
        if (!entryP.empty()) {
            try {
                t.entryOrder = std::stoi(entryP);
                if (t.entryOrder >= nextEntry) nextEntry = t.entryOrder + 1;
            } catch (...) {
                t.entryOrder = seq;
                nextEntry = seq + 1;
            }
        } else {
            t.entryOrder = seq;
            nextEntry = seq + 1;
        }
        seq++;
        tasks.push_back(t);
    }
    return true;
}
