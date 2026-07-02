#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include "TaskManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onCompleteClicked();
    void onUnmarkClicked();
    void onSortPriorityClicked();
    void onSortDeadlineClicked();
    void onSortSubjectClicked();
    void onSortEntryClicked();          // NEW: restore original order
    void onItemChanged(QTableWidgetItem *item);

private:
    void setupTable();
    void refreshTable();

    TaskManager manager;

    QTableWidget *taskTable;
    QLineEdit    *titleInput;
    QLineEdit    *subjectInput;
    QDateEdit    *dateInput;
    QComboBox    *priorityInput;

    QPushButton  *addButton;
    QPushButton  *completeButton;
    QPushButton  *unmarkButton;
    QPushButton  *removeButton;
    QPushButton  *sortPriorityButton;
    QPushButton  *sortDeadlineButton;
    QPushButton  *sortSubjectButton;
    QPushButton  *sortEntryButton;      // NEW
};

#endif
