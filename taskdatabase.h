#ifndef TASKDATABASE_H
#define TASKDATABASE_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QJsonObject>

class TaskDataBase
{
public:
    TaskDataBase();

    bool setPath(const QString& filepath);

    bool setPageLimit(int limit);

    int pageCount() const;

    bool fillModel(QSqlQueryModel* model, int page);

    QByteArray getScript(int taskid);

    bool open();

    QString getTitle(int taskid);

    QString getText(int taskid);

    void close();
private:
    int _pageLimit;

    int _pageCount;

    QString _filepath;

    QSqlDatabase dataBase = QSqlDatabase::addDatabase("QSQLITE", "task_connection");

    static constexpr char DB_TABLE[]    = "task";
    static constexpr char DB_TASKID[]   = "taskid";
    static constexpr char DB_TITLE[]    = "title";
    static constexpr char DB_TEXT[]     = "text";
    static constexpr char DB_SCRIPT[]   = "script";
};

#endif // TASKDATABASE_H
