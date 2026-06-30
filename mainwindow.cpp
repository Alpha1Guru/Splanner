#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    manager.loadFromFile("tasks.txt");

    setWindowTitle("Study Planner");

    // --- build widgets ---
    taskListWidget = new QListWidget(this);

    titleInput = new QLineEdit(this);
    titleInput->setPlaceholderText("Task title");

    subjectInput = new QLineEdit(this);          // NEW
    subjectInput->setPlaceholderText("Course / Subject");

    dateInput = new QDateEdit(this);
    dateInput->setDate(QDate::currentDate());
    dateInput->setCalendarPopup(true);
    dateInput->setDisplayFormat("yyyy-MM-dd");

    priorityInput = new QComboBox(this);
    priorityInput->addItem("Low");
    priorityInput->addItem("Medium");
    priorityInput->addItem("High");
    priorityInput->setCurrentIndex(1);

    addButton          = new QPushButton("Add Task", this);
    removeButton       = new QPushButton("Remove Selected", this);
    completeButton     = new QPushButton("Mark Complete", this);
    sortPriorityButton = new QPushButton("Sort by Priority", this);
    sortDateButton     = new QPushButton("Sort by Due Date", this);
    sortSubjectButton  = new QPushButton("Sort by Subject", this); // NEW

    // --- layout ---
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Row 1: title + subject + date + priority + add button
    QHBoxLayout *formLayout = new QHBoxLayout();
    formLayout->addWidget(titleInput);
    formLayout->addWidget(subjectInput);   // NEW
    formLayout->addWidget(dateInput);
    formLayout->addWidget(priorityInput);
    formLayout->addWidget(addButton);

    // Row 2: action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(completeButton);
    actionLayout->addWidget(removeButton);
    actionLayout->addWidget(sortPriorityButton);
    actionLayout->addWidget(sortDateButton);
    actionLayout->addWidget(sortSubjectButton); // NEW

    mainLayout->addWidget(new QLabel("Your Tasks:"));
    mainLayout->addWidget(taskListWidget);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(actionLayout);

    setCentralWidget(central);
    resize(750, 400);  // slightly wider to fit the extra field

    // --- wire up buttons ---
    connect(addButton,          &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(removeButton,       &QPushButton::clicked, this, &MainWindow::onRemoveClicked);
    connect(completeButton,     &QPushButton::clicked, this, &MainWindow::onCompleteClicked);
    connect(sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);
    connect(sortDateButton,     &QPushButton::clicked, this, &MainWindow::onSortDateClicked);
    connect(sortSubjectButton,  &QPushButton::clicked, this, &MainWindow::onSortSubjectClicked); // NEW

    refreshList();
}

MainWindow::~MainWindow() {}

void MainWindow::refreshList() {
    taskListWidget->clear();
    const auto& tasks = manager.getTasks();
    for (const auto& t : tasks) {
        // Format: [X] Data Structures | Read Chapter 4 | due 2026-07-03 | High
        QString line = QString("[%1] %2  |  %3  |  due %4  |  %5")
            .arg(t.completed ? "X" : " ")
            .arg(QString::fromStdString(t.subject))   // subject shown first
            .arg(QString::fromStdString(t.title))
            .arg(QString::fromStdString(t.dueDate))
            .arg(QString::fromStdString(priorityToString(t.priority)));
        taskListWidget->addItem(line);
    }
}

void MainWindow::onAddClicked() {
    QString title   = titleInput->text().trimmed();
    QString subject = subjectInput->text().trimmed();
    if (title.isEmpty()) return;
    if (subject.isEmpty()) subject = "General"; // sensible default

    std::string dueDate = dateInput->date().toString("yyyy-MM-dd").toStdString();
    Priority p = static_cast<Priority>(priorityInput->currentIndex());

    manager.addTask(title.toStdString(), subject.toStdString(), dueDate, p);
    titleInput->clear();
    subjectInput->clear();
    refreshList();
}

void MainWindow::onRemoveClicked() {
    int row = taskListWidget->currentRow();
    if (row < 0) return;
    manager.removeTask(row);
    refreshList();
}

void MainWindow::onCompleteClicked() {
    int row = taskListWidget->currentRow();
    if (row < 0) return;
    manager.markComplete(row);
    refreshList();
}

void MainWindow::onSortPriorityClicked() {
    manager.sortByPriority();
    refreshList();
}

void MainWindow::onSortDateClicked() {
    manager.sortByDueDate();
    refreshList();
}

void MainWindow::onSortSubjectClicked() {   // NEW
    manager.sortBySubject();
    refreshList();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    manager.saveToFile("tasks.txt");
    event->accept();
}
