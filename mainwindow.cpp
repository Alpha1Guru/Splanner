#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMouseEvent>
#include <QCloseEvent>
#include <algorithm>

// Logical column indices — never change these numbers, they are
// the true column positions regardless of how the user reorders them visually.
static const int COL_CHECK    = 0;
static const int COL_TASK     = 1;
static const int COL_SUBJECT  = 2;
static const int COL_DEADLINE = 3;
static const int COL_PRIORITY = 4;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    manager.loadFromFile("tasks.txt");
    setWindowTitle("Study Planner");

    // --- table ---
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
    completeButton     = new QPushButton("Mark Complete", this);
    unmarkButton       = new QPushButton("Unmark", this);
    removeButton       = new QPushButton("Remove Selected", this);
    sortPriorityButton = new QPushButton("Sort by Priority", this);
    sortDateButton     = new QPushButton("Sort by Deadline", this);
    sortSubjectButton  = new QPushButton("Sort by Subject", this);

    // --- layout ---
    QWidget *central = new QWidget(this);
    QVBoxLayout *main = new QVBoxLayout(central);

    QHBoxLayout *form = new QHBoxLayout();
    form->addWidget(titleInput,   3);   // stretch factors so title gets most space
    form->addWidget(subjectInput, 2);
    form->addWidget(dateInput,    1);
    form->addWidget(priorityInput,1);
    form->addWidget(addButton,    1);

    QHBoxLayout *actions1 = new QHBoxLayout();
    actions1->addWidget(completeButton);
    actions1->addWidget(unmarkButton);
    actions1->addWidget(removeButton);

    QHBoxLayout *actions2 = new QHBoxLayout();
    actions2->addWidget(sortPriorityButton);
    actions2->addWidget(sortDateButton);
    actions2->addWidget(sortSubjectButton);

    main->addWidget(taskTable);
    main->addLayout(form);
    main->addLayout(actions1);
    main->addLayout(actions2);

    setCentralWidget(central);
    resize(820, 480);

    // --- connect signals ---
    connect(addButton,          &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(completeButton,     &QPushButton::clicked, this, &MainWindow::onCompleteClicked);
    connect(unmarkButton,       &QPushButton::clicked, this, &MainWindow::onUnmarkClicked);
    connect(removeButton,       &QPushButton::clicked, this, &MainWindow::onRemoveClicked);
    connect(sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);
    connect(sortDateButton,     &QPushButton::clicked, this, &MainWindow::onSortDateClicked);
    connect(sortSubjectButton,  &QPushButton::clicked, this, &MainWindow::onSortSubjectClicked);
    connect(taskTable, &QTableWidget::itemChanged, this, &MainWindow::onItemChanged);

    refreshTable();
}

MainWindow::~MainWindow() {}

void MainWindow::setupTable() {
    taskTable = new QTableWidget(this);
    taskTable->setColumnCount(5);
    taskTable->setHorizontalHeaderLabels({"", "Task", "Subject", "Deadline", "Priority"});

    QHeaderView *header = taskTable->horizontalHeader();
    header->setSectionsMovable(true);      // columns can be dragged into any order
    header->setSectionResizeMode(QHeaderView::Interactive);  // columns are resizable
    header->setStretchLastSection(true);   // last column fills remaining space

    // Checkbox column stays narrow and fixed
    taskTable->setColumnWidth(COL_CHECK, 28);
    header->setSectionResizeMode(COL_CHECK, QHeaderView::Fixed);

    // Sensible default widths for the other columns
    taskTable->setColumnWidth(COL_TASK,     220);
    taskTable->setColumnWidth(COL_SUBJECT,  150);
    taskTable->setColumnWidth(COL_DEADLINE, 100);
    taskTable->setColumnWidth(COL_PRIORITY,  80);

    taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    taskTable->setSelectionMode(QAbstractItemView::ExtendedSelection); // Ctrl/Shift multi-select
    taskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);     // no inline editing
    taskTable->verticalHeader()->setVisible(false);                     // hide row numbers
    taskTable->setShowGrid(true);
    taskTable->setAlternatingRowColors(true);  // zebra striping for readability

    // Install event filter on viewport so we can detect clicks on empty space
    taskTable->viewport()->installEventFilter(this);
}

void MainWindow::refreshTable() {
    // Block signals so adding items doesn't trigger onItemChanged in a loop
    taskTable->blockSignals(true);
    taskTable->setRowCount(0);

    const auto& tasks = manager.getTasks();
    taskTable->setRowCount((int)tasks.size());

    for (int i = 0; i < (int)tasks.size(); i++) {
        const Task& t = tasks[i];

        // Column 0: checkbox
        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setCheckState(t.completed ? Qt::Checked : Qt::Unchecked);
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        checkItem->setTextAlignment(Qt::AlignCenter);
        taskTable->setItem(i, COL_CHECK, checkItem);

        // Columns 1-4: text data (not editable)
        auto makeItem = [](const QString& text) {
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            return item;
        };

        taskTable->setItem(i, COL_TASK,     makeItem(QString::fromStdString(t.title)));
        taskTable->setItem(i, COL_SUBJECT,  makeItem(QString::fromStdString(t.subject)));
        taskTable->setItem(i, COL_DEADLINE, makeItem(QString::fromStdString(t.dueDate)));
        taskTable->setItem(i, COL_PRIORITY, makeItem(QString::fromStdString(priorityToString(t.priority))));
    }

    taskTable->blockSignals(false);
}

// Called whenever a checkbox is clicked directly in the table
void MainWindow::onItemChanged(QTableWidgetItem *item) {
    if (item->column() != COL_CHECK) return;
    bool checked = (item->checkState() == Qt::Checked);
    manager.setComplete(item->row(), checked);
    // No refreshTable() here — the visual state is already correct
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

// Helper: returns a sorted-descending list of currently selected row indices
static QList<int> selectedRowsSorted(QTableWidget *table) {
    QList<int> rows;
    for (auto *item : table->selectedItems()) {
        if (!rows.contains(item->row()))
            rows.append(item->row());
    }
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    return rows;
}

void MainWindow::onRemoveClicked() {
    QList<int> rows = selectedRowsSorted(taskTable);
    if (rows.isEmpty()) return;
    // Remove bottom-up so upper row indices stay valid during deletion
    for (int row : rows)
        manager.removeTask(row);
    refreshTable();
}

void MainWindow::onCompleteClicked() {
    QList<int> rows = selectedRowsSorted(taskTable);
    if (rows.isEmpty()) return;
    for (int row : rows)
        manager.setComplete(row, true);
    refreshTable();
}

void MainWindow::onUnmarkClicked() {
    QList<int> rows = selectedRowsSorted(taskTable);
    if (rows.isEmpty()) return;
    for (int row : rows)
        manager.setComplete(row, false);
    refreshTable();
}

void MainWindow::onSortPriorityClicked() { manager.sortByPriority(); refreshTable(); }
void MainWindow::onSortDateClicked()     { manager.sortByDueDate();  refreshTable(); }
void MainWindow::onSortSubjectClicked()  { manager.sortBySubject();  refreshTable(); }

// Clicking on empty space below the last row clears the selection
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
