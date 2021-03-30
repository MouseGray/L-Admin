#include "settings.h"

Settings::Settings() :
    port(8000),
    taskDataBase("LTDB.db"),
    studentDataBase("LSDB.db"),
    workplaceRows(4),
    workplaceColumns(3)
{

}

void Settings::load()
{
    settings = new QSettings(settingsPath, QSettings::Format::IniFormat, this);

    get_value(port, settings);
    get_value(taskDataBase,     settings);
    get_value(studentDataBase,  settings);
    get_value(workplaceRows,    settings);
    get_value(workplaceColumns, settings);

    save();
}

int Settings::getPort() const
{
    return port;
}

QString Settings::getTaskDataBase() const
{
    return taskDataBase;
}

int Settings::getWorkplaceRows() const
{
    return workplaceRows;
}

int Settings::getWorkplaceColumns() const
{
    return workplaceColumns;
}

void Settings::save()
{
    set_value(port,             settings);
    set_value(taskDataBase,     settings);
    set_value(studentDataBase,  settings);
    set_value(workplaceRows,    settings);
    set_value(workplaceColumns, settings);
}

QString Settings::getStudentDataBase() const
{
    return studentDataBase;
}
