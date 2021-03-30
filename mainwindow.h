#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QtMath>

#include <QMessageBox>

#include <QDir>

#include "server.h"
#include "studentdatabase.h"
#include "taskdatabase.h"
#include "l_task.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void CreateWorkplaces(int rows, int columns);
    void SetServer(Server* server);

    void setStudentDatabase(StudentDataBase* database);
    void setTaskDatabase(TaskDataBase* database);

    void fillTaskPage();
private slots:
    void NewData(int id, QJsonObject json);
    void StateChanged(int id, WorkplaceState state);

    void SelectName(Workplace* sender);
    void TaskSelect(Workplace* sender);
    void ShowState(Workplace* sender);
    void ShowWork(Workplace* sender);

    void on_stat_title_back_btn_clicked();
    void on_stat_main_table_control_back_btn_clicked();
    void on_stat_main_table_control_forward_btn_clicked();
    void on_stat_main_table_control_page_spn_valueChanged(int arg1);
    void on_stat_main_view_task_control_back_btn_clicked();
    void on_stat_main_view_task_control_forward_btn_clicked();
    void on_stat_main_table_tb_doubleClicked(const QModelIndex &index);

    void on_task_title_back_btn_clicked();
    void on_task_main_table_control_back_btn_clicked();
    void on_task_main_table_control_forward_btn_clicked();
    void on_task_main_table_control_page_spn_valueChanged(int arg1);
    void on_task_main_table_tb_doubleClicked(const QModelIndex &index);
    void on_task_title_send_btn_clicked();

    void on_main_status_title_back_btn_clicked();
    void on_main_status_main_students_tb_doubleClicked(const QModelIndex &index);
private:
    void UpdateState();

    Ui::MainWindow *ui;

    Server* server = nullptr;

    L_Task task;

    StudentDataBase* studentDatabase = nullptr;
    TaskDataBase* taskDatabase = nullptr;

    QVector<Workplace*> workplaces;

    QSqlQueryModel* studentTableModel = nullptr;
    int currentWorkplace = 0;

    QSqlQueryModel* workTableModel = nullptr;
    int pageTaskTable = 0;
    int cur_studentid = 0;

    QSqlQueryModel* taskTableModel = nullptr;
    int pageArchiveTable = 0;
    int cur_taskid = 0;
};
#endif // MAINWINDOW_H
