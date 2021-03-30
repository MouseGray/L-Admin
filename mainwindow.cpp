#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    studentTableModel = new QSqlQueryModel();
    ui->main_status_main_students_tb->setModel(studentTableModel);

    taskTableModel = new QSqlQueryModel();
    ui->task_main_table_tb->setModel(taskTableModel);

    workTableModel = new QSqlQueryModel();
    ui->stat_main_table_tb->setModel(workTableModel);

    ui->main_status_main_students_tb->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->stat_main_table_tb          ->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->stat_main_view_cnv->setTask(&task);

    ui->main_status_title_back_btn->setStyleSheet(buttonToolStyle);

    ui->stat_title_back_btn->setStyleSheet(buttonToolStyle);
    ui->stat_main_table_control_back_btn->setStyleSheet(buttonToolStyle);
    ui->stat_main_table_control_forward_btn->setStyleSheet(buttonToolStyle);
    ui->stat_main_view_task_control_back_btn->setStyleSheet(buttonToolStyle);
    ui->stat_main_view_task_control_forward_btn->setStyleSheet(buttonToolStyle);

    ui->task_title_back_btn->setStyleSheet(buttonToolStyle);
    ui->task_title_send_btn->setStyleSheet(buttonToolStyle);
    ui->task_main_table_control_back_btn->setStyleSheet(buttonToolStyle);
    ui->task_main_table_control_forward_btn->setStyleSheet(buttonToolStyle);

    ui->main_status_title_back_btn->hide();
}

MainWindow::~MainWindow()
{
    delete studentTableModel;
    delete taskTableModel;
    delete workTableModel;

    delete ui;
}

void MainWindow::CreateWorkplaces(int rows, int columns)
{
    // Задание рабочих мест
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < columns; c++) {
            workplaces.push_back(new Workplace(r*columns + c, this));
            ui->main_workplaces_glo->addWidget(workplaces.back(), r, c);
            connect(workplaces.back(), SIGNAL(rename_signal(Workplace*)), this, SLOT(SelectName(Workplace*)));
            connect(workplaces.back(), SIGNAL(showState_signal(Workplace*)), this, SLOT(ShowState(Workplace*)));
            connect(workplaces.back(), SIGNAL(selectTask_signal(Workplace*)), this, SLOT(TaskSelect(Workplace*)));
//          connect(workplaces.back(), SIGNAL(ShowWorkSignal(Workplace*)), this, SLOT(ShowWork(Workplace*)));
        }
    }
    ui->main_status_main_state_places_value_lb->setText(QString::number(workplaces.size()));
}

void MainWindow::SetServer(Server* server)
{
    this->server = server;
    connect(this->server, SIGNAL(StateChanged(int, WorkplaceState)), this, SLOT(StateChanged(int, WorkplaceState)));
    connect(this->server, SIGNAL(NewData(int, QJsonObject)), this, SLOT(NewData(int, QJsonObject)));
}

void MainWindow::setStudentDatabase(StudentDataBase *database)
{
    studentDatabase = database;
}

void MainWindow::setTaskDatabase(TaskDataBase *database)
{
    taskDatabase = database;
    fillTaskPage();
}

void MainWindow::fillTaskPage()
{
    assert(taskDatabase != nullptr);

    // Set page maximum
    ui->task_main_table_control_page_spn->setMaximum(taskDatabase->pageCount() - 1);
    // Fill table
    taskDatabase->fillModel(taskTableModel, 0);
    // Set control
    ui->task_main_table_control_back_btn->setEnabled(false);
    ui->task_main_table_control_page_spn->setValue(0);
    ui->task_main_table_control_forward_btn->setEnabled(
                ui->task_main_table_control_page_spn->value() != ui->task_main_table_control_page_spn->maximum());
}

void MainWindow::NewData(int id, QJsonObject json)
{
    assert(studentDatabase != nullptr);

    if(this->workplaces.size() <= id) {
        QMessageBox::warning(this, "Error!", "This workplace is not created!", QMessageBox::Button::Close);
        return;
    }
    auto doc = QJsonDocument(json);

    if(studentDatabase->saveData(workplaces[id]->getStudentid(), workplaces[id]->getTaskid(), doc.toJson())) {
        workplaces[id]->setState(WorkplaceState::Checking);
        UpdateState();
        // TODO: проверка
        return;
    }
    QMessageBox::warning(this, "Error!", "Date is not saved!", QMessageBox::Button::Close);
    return;
}

void MainWindow::StateChanged(int id, WorkplaceState state)
{
    if(this->workplaces.size() <= id) {
        QMessageBox::warning(this, "Error!", "This workplace is not created!", QMessageBox::Button::Close);
        return;
    }
    workplaces[id]->setState(state);
    UpdateState();
}

void MainWindow::SelectName(Workplace *sender)
{
    assert(studentDatabase != nullptr);

    ui->main_status_title_lb->setText("Members");
    ui->main_status_title_back_btn->show();

    currentWorkplace = sender->getPlaceid();

    studentDatabase->fillStudensModel(studentTableModel);

    ui->main_status_main_stk->setCurrentIndex(1);
}

void MainWindow::TaskSelect(Workplace *sender)
{
    assert(taskDatabase != nullptr);

    cur_studentid = sender->getStudentid();
    currentWorkplace = sender->getPlaceid();

    // Set default values
    ui->task_main_view_number_value_lb->setText("-");
    ui->task_main_view_title_value_lb ->setText("-");
    ui->task_main_view_text_value_te  ->setText("-");
    ui->task_main_view_script_value_te->setText("-");

    // Fill title
    ui->task_title_lb->setText("Tasks: " + sender->getName());
    // Switch page
    ui->central_stk->setCurrentIndex(1);
}

void MainWindow::ShowState(Workplace *sender)
{
    assert(studentDatabase != nullptr);

    cur_studentid = sender->getStudentid();

    // Set page maximum for this student
    studentDatabase->refreshPageLimit(sender->getStudentid());
    ui->stat_main_table_control_page_spn->setMaximum(studentDatabase->pageCount() - 1);
    // Set default values
    ui->stat_main_view_task_number_value_lb ->setText("-");
    ui->stat_main_view_task_title_value_lb  ->setText("-");
    ui->stat_main_view_task_text_value_lb   ->setText("-");
    ui->stat_main_view_task_date_value_lb   ->setText("-");
    ui->stat_main_view_task_mark_value_lb   ->setText("-");
    ui->stat_main_view_task_control_value_lb->setText("-");
    ui->stat_main_view_task_control_back_btn   ->setEnabled(false);
    ui->stat_main_view_task_control_forward_btn->setEnabled(false);
    // Get studentid
    auto studentid = sender->getStudentid();
    // Load state values
    if (!studentDatabase->open()) {
        QMessageBox::warning(this, "Error", "Oops, could not connect to the database.\nTry it again.", QMessageBox::Button::Close);
        return;
    }
    ui->stat_main_view_state_completed_value_lb->setText(QString::number(studentDatabase->getCompletedTask(studentid)));
    ui->stat_main_view_state_rating_value_lb->setText(QString::number(studentDatabase->getAverageRating(studentid), 'g', 2));
    ui->stat_main_view_state_unchecked_value_lb->setText(QString::number(studentDatabase->getUnckeckedTask(studentid)));
    studentDatabase->close();
    // Fill table
    studentDatabase->fillWorksModel(workTableModel, studentid, 0);
    // Set control
    ui->stat_main_table_control_back_btn->setEnabled(false);
    ui->stat_main_table_control_page_spn->setValue(0);
    ui->stat_main_table_control_forward_btn->setEnabled(
                ui->stat_main_table_control_page_spn->value() != ui->stat_main_table_control_page_spn->maximum());
    // Fill title
    ui->stat_title_lb->setText("State: " + sender->getName());
    // Switch page
    ui->central_stk->setCurrentIndex(2);
}

void MainWindow::ShowWork(Workplace *sender)
{
}

void MainWindow::UpdateState()
{
    int connected = 0;
    int working = 0;
    for(int i = 0; i < this->workplaces.size(); i++){
        if(this->workplaces[i]->getState() != WorkplaceState::Disconnected) connected++;
        if(this->workplaces[i]->getState() == WorkplaceState::Working) working++;
    }

    ui->main_status_main_state_connected_value_lb->setText(QString::number(connected));
    ui->main_status_main_state_work_value_lb->setText(QString::number(working));
}

void MainWindow::on_stat_title_back_btn_clicked()
{
    ui->central_stk->setCurrentIndex(0);
}

void MainWindow::on_stat_main_table_control_back_btn_clicked()
{
    auto value = ui->stat_main_table_control_page_spn->value();
    auto maximum = ui->stat_main_table_control_page_spn->maximum();
    if (value == 0) return;
    ui->stat_main_table_control_page_spn->setValue(value - 1);
    ui->stat_main_table_control_back_btn->setEnabled(value - 1 != 0);
    ui->stat_main_table_control_forward_btn->setEnabled(value - 1 != maximum);
}

void MainWindow::on_stat_main_table_control_forward_btn_clicked()
{
    auto value = ui->stat_main_table_control_page_spn->value();
    auto maximum = ui->stat_main_table_control_page_spn->maximum();
    if (value == maximum) return;
    ui->stat_main_table_control_page_spn->setValue(value + 1);
    ui->stat_main_table_control_back_btn->setEnabled(value + 1 != 0);
    ui->stat_main_table_control_forward_btn->setEnabled(value + 1 != maximum);
}

void MainWindow::on_stat_main_table_control_page_spn_valueChanged(int arg1)
{
    studentDatabase->fillWorksModel(workTableModel, cur_studentid, arg1);
    ui->stat_main_table_control_back_btn->setEnabled(arg1 != 0);
    ui->stat_main_table_control_forward_btn->setEnabled(arg1 != ui->stat_main_table_control_page_spn->maximum());
}

void MainWindow::on_stat_main_view_task_control_back_btn_clicked()
{
    task.previous();
    ui->stat_main_view_task_control_value_lb->setText(task.current()->name());
}

void MainWindow::on_stat_main_view_task_control_forward_btn_clicked()
{
    task.next();
    ui->stat_main_view_task_control_value_lb->setText(task.current()->name());
}

void MainWindow::on_stat_main_table_tb_doubleClicked(const QModelIndex &index)
{
    // get data from columns
    auto workid = ui->stat_main_table_tb->model()->index(index.row(), 0).data().toInt();
    auto date   = ui->stat_main_table_tb->model()->index(index.row(), 1).data().toString();
    auto mark   = ui->stat_main_table_tb->model()->index(index.row(), 2).data().toInt();
    auto taskid = ui->stat_main_table_tb->model()->index(index.row(), 3).data().toInt();
    // get script by workid
    auto data = studentDatabase->getScript(workid);
    // try get task
    try {
        task.fromJSON(QJsonDocument::fromJson(data).object());
    }  catch (std::invalid_argument&) {
        QMessageBox::warning(this, "Error", "Oops, script could not be deployed.\nPlease contact your administrator.");
        return;
    }
    // set known parameters
    ui->stat_main_view_task_number_value_lb->setText(QString::number(taskid));
    ui->stat_main_view_task_date_value_lb->setText(date);
    ui->stat_main_view_task_mark_value_lb->setText(QString::number(mark));
    // load task parameters
    if (!taskDatabase->open()) {
        QMessageBox::warning(this, "Error", "Oops, could not connect to the database.\nTry it again.", QMessageBox::Button::Close);
        return;
    }
    ui->stat_main_view_task_title_value_lb->setText(taskDatabase->getTitle(taskid));
    ui->stat_main_view_task_text_value_lb->setText(taskDatabase->getText(taskid));
    // set control
    ui->stat_main_view_task_control_back_btn->setEnabled(true);
    ui->stat_main_view_task_control_forward_btn->setEnabled(true);
    ui->stat_main_view_task_control_value_lb->setText(task.current()->name());
    taskDatabase->close();
}

void MainWindow::on_task_title_back_btn_clicked()
{
    ui->central_stk->setCurrentIndex(0);
}

void MainWindow::on_task_main_table_control_back_btn_clicked()
{
    auto value = ui->task_main_table_control_page_spn->value();
    auto maximum = ui->task_main_table_control_page_spn->maximum();
    if (value == 0) return;
    ui->task_main_table_control_page_spn->setValue(value - 1);
    ui->task_main_table_control_back_btn->setEnabled(value - 1 != 0);
    ui->task_main_table_control_forward_btn->setEnabled(value - 1 != maximum);
}

void MainWindow::on_task_main_table_control_forward_btn_clicked()
{
    auto value = ui->task_main_table_control_page_spn->value();
    auto maximum = ui->task_main_table_control_page_spn->maximum();
    if (value == 0) return;
    ui->task_main_table_control_page_spn->setValue(value + 1);
    ui->task_main_table_control_back_btn->setEnabled(value + 1 != 0);
    ui->task_main_table_control_forward_btn->setEnabled(value + 1 != maximum);
}

void MainWindow::on_task_main_table_control_page_spn_valueChanged(int arg1)
{
    taskDatabase->fillModel(workTableModel, arg1);
    ui->task_main_table_control_back_btn->setEnabled(arg1 != 0);
    ui->task_main_table_control_forward_btn->setEnabled(arg1 != ui->task_main_table_control_page_spn->maximum());
}

void MainWindow::on_task_main_table_tb_doubleClicked(const QModelIndex &index)
{
    // get data from columns
    auto taskid = ui->task_main_table_tb->model()->index(index.row(), 0).data().toInt();
    auto title  = ui->task_main_table_tb->model()->index(index.row(), 1).data().toString();
    auto text   = ui->task_main_table_tb->model()->index(index.row(), 2).data().toString();
    // get script by workid
    auto data = taskDatabase->getScript(taskid);
    // set known parameters
    ui->task_main_view_number_value_lb->setText(QString::number(taskid));
    ui->task_main_view_title_value_lb ->setText(title);
    ui->task_main_view_text_value_te  ->setText(text);
    ui->task_main_view_script_value_te->setText(data);

    cur_taskid = taskid;
}

void MainWindow::on_task_title_send_btn_clicked()
{
    QJsonObject task;
    if(!taskDatabase->open()) return;
    task.insert("title", taskDatabase->getTitle(cur_taskid));
    task.insert("text", taskDatabase->getText(cur_taskid));
    taskDatabase->close();
    task.insert("data", QJsonDocument::fromJson(taskDatabase->getScript(cur_taskid)).object());

    qDebug() << task;
    server->sendData(currentWorkplace, "task", task);
    workplaces[currentWorkplace]->setTaskid(cur_taskid);
    workplaces[currentWorkplace]->setState(WorkplaceState::Waiting);
    ui->central_stk->setCurrentIndex(0);
}

void MainWindow::on_main_status_title_back_btn_clicked()
{
    ui->main_status_title_lb->setText("Status");
    ui->main_status_title_back_btn->hide();
    ui->main_status_main_stk->setCurrentIndex(0);
}

void MainWindow::on_main_status_main_students_tb_doubleClicked(const QModelIndex &index)
{
    QString name = ui->main_status_main_students_tb->model()->index(index.row(), 1).data().toString() + " "
            + ui->main_status_main_students_tb->model()->index(index.row(), 2).data().toString();
    int id = ui->main_status_main_students_tb->model()->index(index.row(), 0).data().toInt();
    for(int i = 0; i < workplaces.size(); i++){
        if(workplaces[i]->getName() == name) {
            QMessageBox::warning(this, "Error!", "This member already has workplace!", QMessageBox::Button::Close);
            return;
        }
    }
    workplaces[currentWorkplace]->setName(name);
    workplaces[currentWorkplace]->setStudentid(id);

    ui->main_status_title_lb->setText("Status");
    ui->main_status_title_back_btn->hide();
    ui->main_status_main_stk->setCurrentIndex(0);
}
