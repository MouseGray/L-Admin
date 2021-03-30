#ifndef STUDENTDATABASE_H
#define STUDENTDATABASE_H

#include <QSqlDatabase>
#include <QSqlQueryModel>

// +-----------------------------------------+
// |                 Students                |
// +-----------+------+---------+------------+
// | StutendID | Name | Surname | Patronymic |
// +-----------+------+---------+------------+

// +----------------------------------------------------+
// |                        Works                       |
// +--------+-----------+------+------+--------+--------+
// | WorkID | StutendID | Date | Mark | Script | taskid |
// +--------+-----------+------+------+--------+--------+

class StudentDataBase
{
public:
    StudentDataBase();

    bool setPath(const QString& filepath);

    bool setPageLimit(int limit);

    int pageCount() const;

    bool refreshPageLimit(int studentid);

    bool fillStudensModel(QSqlQueryModel* model);

    bool fillWorksModel(QSqlQueryModel* model, int studentid, int page);

    QByteArray getScript(int workid);

    int saveData(int studentid, int taskid, const QString& script);

    bool setMark(int workid, int mark);

    bool open();

    int getCompletedTask(int studentid);

    double getAverageRating(int studentid);

    int getUnckeckedTask(int studentid);

    void close();
private:
    int _pageLimit;

    int _pageCount;

    QString _filepath;

    QSqlDatabase dataBase = QSqlDatabase::addDatabase("QSQLITE", "student_connection");

    static constexpr char DBS_TABLE[]       = "student";
    static constexpr char DBW_TABLE[]       = "work";
    static constexpr char DB_STUDENTID[]    = "studentid";
    static constexpr char DB_WORKID[]       = "workid";
    static constexpr char DBS_NAME[]        = "name";
    static constexpr char DBS_SURNAME[]     = "surname";
    static constexpr char DBS_PATRONYMIC[]  = "patronymic";
    static constexpr char DBW_DATE[]        = "date";
    static constexpr char DBW_MARK[]        = "mark";
    static constexpr char DBW_SCRIPT[]      = "script";
    static constexpr char DBW_TASKID[]      = "taskid";
};

#endif // STUDENTDATABASE_H
