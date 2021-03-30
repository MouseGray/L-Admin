#ifndef WORKPLACE_H
#define WORKPLACE_H

#include <QObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QStyleOption>
#include <QPainter>
#include <QToolButton>

#include "styles.h"

enum class WorkplaceState
{
    Disconnected,
    ConnectedNN,
    Connected,
    Waiting,
    Working,
    Checking
};

class Workplace : public QWidget
{
    Q_OBJECT
public:
    Workplace(int placeid, QWidget *parent = nullptr);

    void setState(WorkplaceState state) noexcept;

    void setName(const QString& name) noexcept;
    inline void setStudentid(int studentid) noexcept { _studentid = studentid; }
    inline void setTaskid(int taskid) noexcept { _taskid = taskid; }

    inline WorkplaceState getState() const noexcept { return _state; };
    inline int getPlaceid() const noexcept { return _placeid; }

    inline QString getName() const noexcept { return _name; }
    inline int getStudentid() const noexcept { return _studentid; }
    inline int getTaskid() const noexcept { return _taskid; };

    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

    static QString getWorkplaceStateText(WorkplaceState state);

    static void setWidgetColor(QWidget* widget, QPalette::ColorRole role, const QColor& color);
private slots:
    void on_rename_btn_clicked() { rename_signal(this); }
    void on_state_btn_clicked() { showState_signal(this); }
    void on_task_btn_clicked() { selectTask_signal(this); }
//  void ShowWorkButtonSlot() { ShowWorkSignal(this); }
signals:
    void rename_signal(Workplace* sender);
    void showState_signal(Workplace* sender);
    void selectTask_signal(Workplace* sender);
//  void ShowWorkSignal(Workplace* sender);
private:
    void updateState();
    void setButtonSet(bool rename_btn, bool state_btn, bool task_btn);

    QString _name;

    int _taskid = 0;
    int _placeid = -1;
    int _studentid = 0;

    WorkplaceState _state = WorkplaceState::Disconnected;

    QVBoxLayout workplace_lo;
    QLabel workplace_name_lb;
    QLabel workplace_state_lb;

    QPushButton workplace_rename_btn;
    QPushButton workplace_state_btn;
    QPushButton workplace_task_btn;
//  QPushButton ShowWorkButton;

    static constexpr QColor windowColor = QColor(255, 210, 150);
    static constexpr QColor selectWindowColor = QColor(255, 180, 130);

    static constexpr QColor connectedColor = QColor(100, 255, 100);
    static constexpr QColor disconnectedColor = QColor(255, 100, 100);
};

#endif // WORKPLACE_H
