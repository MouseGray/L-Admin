#include "mainwindow.h"
#include "settings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Settings cfg;
    cfg.load();

    Server server;
    server.listen(QHostAddress::Any, cfg.getPort());

    StudentDataBase studentDataBase;
    studentDataBase.setPath(cfg.getStudentDataBase());
    studentDataBase.setPageLimit(30);

    TaskDataBase taskDataBase;
    taskDataBase.setPath(cfg.getTaskDataBase());
    taskDataBase.setPageLimit(30);

    MainWindow w;
    w.setStudentDatabase(&studentDataBase);
    w.setTaskDatabase(&taskDataBase);
    w.SetServer(&server);
    w.CreateWorkplaces(cfg.getWorkplaceRows(), cfg.getWorkplaceColumns());
    w.show();
    return a.exec();
}
