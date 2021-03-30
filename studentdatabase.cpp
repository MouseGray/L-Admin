#include "studentdatabase.h"

#include <QFile>
#include <QSqlQuery>
#include <QtMath>
#include <QDateTime>
#include <QDebug>
#include <QSqlError>

StudentDataBase::StudentDataBase()
{

}

bool StudentDataBase::setPath(const QString &filepath)
{
    _filepath = filepath;
    if (QFile::exists(_filepath)){
        dataBase.setDatabaseName(filepath);
        return dataBase.isValid();
    }
    return false;
}

bool StudentDataBase::setPageLimit(int limit)
{
    _pageLimit = limit;

    if (!dataBase.open()) return false;

    QSqlQuery query(dataBase);

    QString query_text = QString("SELECT COUNT(*) FROM %1")
            .arg(DBW_TABLE);

    if (query.exec(query_text)) {
        if (query.next()) {
            _pageCount = qCeil(query.value(0).toFloat()/_pageLimit);
            dataBase.close();
            return true;
        }
    }
    dataBase.close();
    return false;
}

int StudentDataBase::pageCount() const
{
    return _pageCount;
}

bool StudentDataBase::refreshPageLimit(int studentid)
{
    if (!dataBase.open()) return false;

    QSqlQuery query(dataBase);

    QString query_text = QString("SELECT COUNT(*) FROM %1 WHERE %2 = %3")
            .arg(DBW_TABLE)
            .arg(DB_STUDENTID)
            .arg(studentid);

    if (query.exec(query_text)) {
        if (query.next()) {
            _pageCount = qMax(qCeil(query.value(0).toFloat()/_pageLimit), 1);
            dataBase.close();
            return true;
        }
    }
    dataBase.close();
    return false;
}

bool StudentDataBase::fillStudensModel(QSqlQueryModel *model)
{
    if (!dataBase.open()) return false;

    QSqlQuery query(dataBase);


    QString query_text = "SELECT student.studentid AS No, name AS Name, surname AS Surname, patronymic AS Patronymic, AVG(mark) AS Rating "
                         "FROM student "
                         "LEFT JOIN work "
                         "ON student.studentid = work.studentid "
                         "GROUP BY student.studentid, name, surname, patronymic;";

    if (query.exec(query_text)) {
        model->setQuery(query);
        dataBase.close();
        return true;
    }

    qDebug() << query.lastError();

    dataBase.close();
    return false;
}

bool StudentDataBase::fillWorksModel(QSqlQueryModel *model, int studentid, int page)
{
    assert(page >= 0);

    if (!dataBase.open()) return false;

    QSqlQuery query(dataBase);

    QString query_text = QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %6 = %7 LIMIT %8, %9")
            .arg(DB_WORKID)
            .arg(DBW_DATE)
            .arg(DBW_MARK)
            .arg(DBW_TASKID)
            .arg(DBW_TABLE)
            .arg(DB_STUDENTID)
            .arg(studentid)
            .arg(page*_pageLimit)
            .arg(_pageLimit);

    if (query.exec(query_text)) {
        model->setQuery(query);
        dataBase.close();
        return true;
    }

    dataBase.close();
    return false;
}

QByteArray StudentDataBase::getScript(int workid)
{
    assert(workid >= 0);

    if (!dataBase.open()) return "";

    QSqlQuery query(dataBase);

    QString query_text = QString("SELECT %1 FROM %2 WHERE %3 = %4")
            .arg(DBW_SCRIPT)
            .arg(DBW_TABLE)
            .arg(DB_WORKID)
            .arg(workid);

    if (query.exec(query_text)) {
        if (query.next()) {
            auto result = query.value(0).toByteArray();
            dataBase.close();
            return result;
        }
    }

    dataBase.close();
    return "";
}

int StudentDataBase::saveData(int studentid, int taskid, const QString &script)
{
    assert(studentid >= 0);

    if (!dataBase.open()) return -1;

    QSqlQuery query(dataBase);

    QString query_text = QString("INSERT INTO %1 (%2, %3, %4, %5) VALUES (%6, %7, %8, %9)")
            .arg(DBW_TABLE)
            .arg(DB_STUDENTID)
            .arg(DBW_DATE)
            .arg(DBW_SCRIPT)
            .arg(DBW_TASKID)
            .arg(studentid)
            .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"))
            .arg(script)
            .arg(taskid);

    if (query.exec(query_text)) {
        if (query.numRowsAffected()) {
            query_text = "SELECT last_insert_rowid()";
            if (query.exec(query_text)) {
                if (query.next()) {
                    auto ok = false;
                    auto result = query.value(0).toInt(&ok);
                    dataBase.close();
                    return ok ? result : -1;
                }
            }
        }
    }

    dataBase.close();
    return -1;
}

bool StudentDataBase::setMark(int workid, int mark)
{
    assert(workid >= 0);

    if (!dataBase.open()) return false;

    QSqlQuery query(dataBase);

    QString query_text = QString("UPDATE %1 SET %2 = %3 WHERE %4 = %5")
            .arg(DBW_TABLE)
            .arg(DBW_MARK)
            .arg(mark)
            .arg(DB_WORKID)
            .arg(workid);

    if (query.exec(query_text)) {
        if (query.numRowsAffected()) {
            dataBase.close();
            return true;
        }
    }

    dataBase.close();
    return false;
}

bool StudentDataBase::open()
{
    return dataBase.open();
}

int StudentDataBase::getCompletedTask(int studentid)
{
    assert(studentid > 0);

    QSqlQuery query(dataBase);
    assert(query.exec("SELECT COUNT(*) "
                   "FROM work "
                   "WHERE studentid = " + QString::number(studentid) + ";"));

    if (query.next())
        return query.value(0).toInt();

    return 0;
}

double StudentDataBase::getAverageRating(int studentid)
{
    assert(studentid > 0);

    QSqlQuery query(dataBase);
    assert(query.exec("SELECT AVG(mark) "
                   "FROM work "
                   "WHERE studentid = " + QString::number(studentid) + ";"));

    if (query.next())
        return query.value(0).toDouble();

    return 0;
}

int StudentDataBase::getUnckeckedTask(int studentid)
{
    assert(studentid > 0);

    QSqlQuery query(dataBase);
    assert(query.exec("SELECT COUNT(*) "
                   "FROM work "
                   "WHERE mark IS NULL AND studentid = " + QString::number(studentid) + ";"));

    if (query.next())
        return query.value(0).toInt();

    return 0;
}

void StudentDataBase::close()
{
    dataBase.close();
}
