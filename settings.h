#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>



class Settings : QObject
{
    Q_OBJECT
public:
    Settings();

    void load();

    int getPort() const;

    QString getTaskDataBase() const;

    int getWorkplaceRows() const;

    int getWorkplaceColumns() const;

    QString getStudentDataBase() const;

private:
    template<typename T>
    T variant_cast(const QVariant& variant, const T& default_value);

    template<>
    int variant_cast<int>(const QVariant& variant, const int& default_value) {
        auto cast_result = true;
        auto value = variant.toInt(&cast_result);
        return cast_result ? value : default_value;
    }

    template<>
    QString variant_cast<QString>(const QVariant& variant, const QString& default_value) {
        auto value = variant.toString();
        return !value.isEmpty() ? value : default_value;
    }

    template<typename T>
    void settings_value(T& var, char const* var_name, QSettings* settings) {
        auto val = settings->value(var_name, var);
        var = variant_cast<T>(val, var);
    }

    #define get_value(var, settings) settings_value(var, #var, settings)
    #define set_value(var, settings) settings->setValue(#var, var)

    void save();

    int port;

    QString taskDataBase;

    QString studentDataBase;

    int workplaceRows;

    int workplaceColumns;

    QSettings* settings;

    static constexpr char const* settingsPath = "settings.ini";
};

#endif // SETTINGS_H
