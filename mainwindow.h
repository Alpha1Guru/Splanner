#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include "TaskManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onCompleteClicked();
    void onSortPriorityClicked();
    void onSortDateClicked();
    void onSortSubjectClicked();   // NEW

private:
    void refreshList();

    TaskManager manager;

    QListWidget *taskListWidget;
    QLineEdit   *titleInput;
    QLineEdit   *subjectInput;     // NEW
    QDateEdit   *dateInput;
    QComboBox   *priorityInput;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *completeButton;
    QPushButton *sortPriorityButton;
    QPushButton *sortDateButton;
    QPushButton *sortSubjectButton; // NEW

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif
