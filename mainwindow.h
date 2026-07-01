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
    bool eventFilter(QObject *obj, QEvent *event) override;  // for click-to-deselect

private slots:
    void onAddClicked();
    void onRemoveClicked();        // removes ALL selected rows
    void onCompleteClicked();      // marks ALL selected rows complete
    void onUnmarkClicked();        // unmarks ALL selected rows
    void onSortPriorityClicked();
    void onSortDateClicked();
    void onSortSubjectClicked();
    void onItemChanged(QTableWidgetItem *item);  // handles checkbox toggling

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
    QPushButton  *sortDateButton;
    QPushButton  *sortSubjectButton;
};

#endif
