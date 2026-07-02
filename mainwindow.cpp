#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMouseEvent>
#include <QCloseEvent>
#include <algorithm>

static const int COL_CHECK    = 0;
static const int COL_TASK     = 1;
static const int COL_SUBJECT  = 2;
static const int COL_DEADLINE = 3;
static const int COL_PRIORITY = 4;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    manager.loadFromFile("tasks.txt");
    setWindowTitle("Study Planner");

    setupTable();

    // --- input form ---
    titleInput = new QLineEdit(this);
    titleInput->setPlaceholderText("Task title");

    subjectInput = new QLineEdit(this);
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

    addButton = new QPushButton("Add Task", this);

    // --- action buttons ---
    completeButton     = new QPushButton("Mark Complete",   this);
    unmarkButton       = new QPushButton("Unmark",          this);
    removeButton       = new QPushButton("Remove Selected", this);
    sortPriorityButton = new QPushButton("Sort by Priority",this);
    sortDeadlineButton = new QPushButton("Sort by Deadline",this);
    sortSubjectButton  = new QPushButton("Sort by Subject", this);
    sortEntryButton    = new QPushButton("Sort by Entry",   this);

    // --- layout ---
    QWidget     *central = new QWidget(this);
    QVBoxLayout *main    = new QVBoxLayout(central);

    QHBoxLayout *form = new QHBoxLayout();
    form->addWidget(titleInput,    3);
    form->addWidget(subjectInput,  2);
    form->addWidget(dateInput,     1);
    form->addWidget(priorityInput, 1);
    form->addWidget(addButton,     1);

    QHBoxLayout *actions1 = new QHBoxLayout();
    actions1->addWidget(completeButton);
    actions1->addWidget(unmarkButton);
    actions1->addWidget(removeButton);

    QHBoxLayout *actions2 = new QHBoxLayout();
    actions2->addWidget(sortPriorityButton);
    actions2->addWidget(sortDeadlineButton);
    actions2->addWidget(sortSubjectButton);
    actions2->addWidget(sortEntryButton);

    main->addWidget(taskTable);
    main->addLayout(form);
    main->addLayout(actions1);
    main->addLayout(actions2);

    setCentralWidget(central);
    resize(820, 480);

    // --- signals ---
    connect(addButton,          &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(completeButton,     &QPushButton::clicked, this, &MainWindow::onCompleteClicked);
    connect(unmarkButton,       &QPushButton::clicked, this, &MainWindow::onUnmarkClicked);
    connect(removeButton,       &QPushButton::clicked, this, &MainWindow::onRemoveClicked);
    connect(sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);
    connect(sortDeadlineButton, &QPushButton::clicked, this, &MainWindow::onSortDeadlineClicked);
    connect(sortSubjectButton,  &QPushButton::clicked, this, &MainWindow::onSortSubjectClicked);
    connect(sortEntryButton,    &QPushButton::clicked, this, &MainWindow::onSortEntryClicked);
    connect(taskTable, &QTableWidget::itemChanged, this, &MainWindow::onItemChanged);

    refreshTable();
}

MainWindow::~MainWindow() {}

void MainWindow::setupTable() {
    taskTable = new QTableWidget(this);
    taskTable->setColumnCount(5);
    taskTable->setHorizontalHeaderLabels({"", "Task", "Subject", "Deadline", "Priority"});

    QHeaderView *header = taskTable->horizontalHeader();
    header->setSectionsMovable(true);
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setStretchLastSection(true);

    taskTable->setColumnWidth(COL_CHECK,    28);
    taskTable->setColumnWidth(COL_TASK,    220);
    taskTable->setColumnWidth(COL_SUBJECT, 150);
    taskTable->setColumnWidth(COL_DEADLINE,100);
    taskTable->setColumnWidth(COL_PRIORITY, 80);
    header->setSectionResizeMode(COL_CHECK, QHeaderView::Fixed);

    taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    taskTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    taskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    taskTable->verticalHeader()->setVisible(false);
    taskTable->setShowGrid(true);
    taskTable->setAlternatingRowColors(true);

    taskTable->viewport()->installEventFilter(this);
}

void MainWindow::refreshTable() {
    taskTable->blockSignals(true);
    taskTable->setRowCount(0);

    const auto& tasks = manager.getTasks();
    taskTable->setRowCount((int)tasks.size());

    for (int i = 0; i < (int)tasks.size(); i++) {
        const Task& t = tasks[i];

        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setCheckState(t.completed ? Qt::Checked : Qt::Unchecked);
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        taskTable->setItem(i, COL_CHECK, checkItem);

        auto makeItem = [](const QString& text) {
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            return item;
        };

        // Show "No deadline" for tasks with an empty due date
        QString deadlineText = t.dueDate.empty()
            ? "No deadline"
            : QString::fromStdString(t.dueDate);

        taskTable->setItem(i, COL_TASK,     makeItem(QString::fromStdString(t.title)));
        taskTable->setItem(i, COL_SUBJECT,  makeItem(QString::fromStdString(t.subject)));
        taskTable->setItem(i, COL_DEADLINE, makeItem(deadlineText));
        taskTable->setItem(i, COL_PRIORITY, makeItem(QString::fromStdString(priorityToString(t.priority))));
    }

    taskTable->blockSignals(false);
}

void MainWindow::onItemChanged(QTableWidgetItem *item) {
    if (item->column() != COL_CHECK) return;
    manager.setComplete(item->row(), item->checkState() == Qt::Checked);
}

void MainWindow::onAddClicked() {
    QString title   = titleInput->text().trimmed();
    QString subject = subjectInput->text().trimmed();
    if (title.isEmpty()) return;
    if (subject.isEmpty()) subject = "General";

    std::string dueDate = dateInput->date().toString("yyyy-MM-dd").toStdString();
    Priority p = static_cast<Priority>(priorityInput->currentIndex());

    manager.addTask(title.toStdString(), subject.toStdString(), dueDate, p);
    titleInput->clear();
    subjectInput->clear();
    refreshTable();
}

static QList<int> selectedRowsSorted(QTableWidget *table) {
    QList<int> rows;
    for (auto *item : table->selectedItems())
        if (!rows.contains(item->row())) rows.append(item->row());
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    return rows;
}

void MainWindow::onRemoveClicked() {
    QList<int> rows = selectedRowsSorted(taskTable);
    if (rows.isEmpty()) return;
    for (int row : rows) manager.removeTask(row);
    refreshTable();
}

void MainWindow::onCompleteClicked() {
    QList<int> rows = selectedRowsSorted(taskTable);
    if (rows.isEmpty()) return;
    for (int row : rows) manager.setComplete(row, true);
    refreshTable();
}

void MainWindow::onUnmarkClicked() {
    QList<int> rows = selectedRowsSorted(taskTable);
    if (rows.isEmpty()) return;
    for (int row : rows) manager.setComplete(row, false);
    refreshTable();
}

void MainWindow::onSortPriorityClicked() { manager.sortByPriority();   refreshTable(); }
void MainWindow::onSortDeadlineClicked() { manager.sortByDeadline();   refreshTable(); }
void MainWindow::onSortSubjectClicked()  { manager.sortBySubject();    refreshTable(); }
void MainWindow::onSortEntryClicked()    { manager.sortByEntryOrder(); refreshTable(); }

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == taskTable->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if (!taskTable->indexAt(me->pos()).isValid())
            taskTable->clearSelection();
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    manager.saveToFile("tasks.txt");
    event->accept();
}
