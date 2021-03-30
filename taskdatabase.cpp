#include "taskdatabase.h"

#include <QFile>
#include <QSqlQuery>
#include <QtMath>

TaskDataBase::TaskDataBase()
{

}

bool TaskDataBase::setPath(const QString &filepath)
{
    _filepath = filepath;
    if (QFile::exists(_filepath)){
        dataBase.setDatabaseName(filepath);
        return dataBase.isValid();
    }
    return false;
}

bool TaskDataBase::setPageLimit(int limit)
{
    _pageLimit = limit;

    if (!dataBase.open()) return false;

    QSqlQuery query(dataBase);

    QString query_text = QString("SELECT COUNT(*) FROM %1")
            .arg(DB_TABLE);

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

int TaskDataBase::pageCount() const
{
    return _pageCount;
}

bool TaskDataBase::fillModel(QSqlQueryModel *model, int page)
{
    assert(page >= 0);

    if (!dataBase.open()) return false;

    QSqlQuery query(dataBase);

    QString query_text =
            QString("SELECT %1, %2, %3 FROM %4 LIMIT %5, %6")
            .arg(DB_TASKID)
            .arg(DB_TITLE)
            .arg(DB_TEXT)
            .arg(DB_TABLE)
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

QByteArray TaskDataBase::getScript(int taskid)
{
    assert(taskid >= 0);

    if (!dataBase.open()) return "";

    QSqlQuery query(dataBase);

    QString query_text = QString("SELECT %1 FROM %2 WHERE %3 = %4")
            .arg(DB_SCRIPT)
            .arg(DB_TABLE)
            .arg(DB_TASKID)
            .arg(taskid);

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

bool TaskDataBase::open()
{
    return dataBase.open();
}

QString TaskDataBase::getTitle(int taskid)
{
    QSqlQuery query(dataBase);

    assert (query.exec("SELECT title "
                       "FROM task "
                       "WHERE taskid = " + QString::number(taskid) + ";"));

    if (query.next())
        return query.value(0).toString();
    return "-";
}

QString TaskDataBase::getText(int taskid)
{
    QSqlQuery query(dataBase);

    assert (query.exec("SELECT text "
                       "FROM task "
                       "WHERE taskid = " + QString::number(taskid) + ";"));

    if (query.next())
        return query.value(0).toString();
    return "-";
}

void TaskDataBase::close()
{
    dataBase.close();
}
