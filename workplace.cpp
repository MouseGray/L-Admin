#include "workplace.h"
#include <QDebug>
#include <QTimer>

Workplace::Workplace(int placeid, QWidget *parent) : QWidget(parent)
{
    _placeid = placeid;
    _state = WorkplaceState::Disconnected;

    setLayout(&workplace_lo);

    auto _palette = palette();
    _palette.setColor(QPalette::ColorRole::Window, QColor(255, 210, 150));
    setPalette(_palette);

    setAutoFillBackground(true);

    workplace_lo.setMargin(0);
    workplace_lo.addSpacerItem(new QSpacerItem(0, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));

    workplace_name_lb.setAlignment(Qt::AlignHCenter);
    workplace_lo.addWidget(&workplace_name_lb);

    workplace_rename_btn.setText("Rename");
    workplace_rename_btn.setStyleSheet(buttonStyle);
    workplace_lo.addWidget(&workplace_rename_btn);

    workplace_state_btn.setText("State");
    workplace_state_btn.setStyleSheet(buttonStyle);
    workplace_lo.addWidget(&workplace_state_btn);

    workplace_task_btn.setText("Send Task");
    workplace_task_btn.setStyleSheet(buttonStyle);
    workplace_lo.addWidget(&workplace_task_btn);

    workplace_lo.addSpacerItem(new QSpacerItem(0, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));

    workplace_state_lb.setText("Disconnected");
    workplace_state_lb.setAlignment(Qt::AlignHCenter);
    workplace_state_lb.setAutoFillBackground(true);
    workplace_lo.addWidget(&workplace_state_lb);

    connect(&workplace_rename_btn, &QPushButton::clicked, this, &Workplace::on_rename_btn_clicked);
    connect(&workplace_state_btn,  &QPushButton::clicked, this, &Workplace::on_state_btn_clicked);
    connect(&workplace_task_btn,   &QPushButton::clicked, this, &Workplace::on_task_btn_clicked);

    updateState();
}

void Workplace::setState(WorkplaceState state) noexcept
{
    _state = state;
    updateState();
    setButtonSet(false, false, false);
}

void Workplace::setName(const QString &name) noexcept
{
    if(_state == WorkplaceState::ConnectedNN)
        setState(WorkplaceState::Connected);
    _name = name;
    workplace_name_lb.setText(name);
}

void Workplace::enterEvent(QEvent*)
{
    setWidgetColor(this, QPalette::ColorRole::Window, selectWindowColor);
    updateState();
}

void Workplace::leaveEvent(QEvent*)
{
    setWidgetColor(this, QPalette::ColorRole::Window, windowColor);
    setButtonSet(false, false, false);
}

QString Workplace::getWorkplaceStateText(WorkplaceState state)
{
    switch(state){
    case WorkplaceState::Disconnected: return "Disconnected";
    case WorkplaceState::ConnectedNN:  return "Connected noname";
    case WorkplaceState::Connected:    return "Connected";
    case WorkplaceState::Waiting:      return "Waiting...";
    case WorkplaceState::Working:      return "Working...";
    case WorkplaceState::Checking:     return "Checking...";
    }
    return "unknown";
}

void Workplace::setWidgetColor(QWidget *widget, QPalette::ColorRole role, const QColor &color)
{
    auto _palette = widget->palette();
    _palette.setColor(role, color);
    widget->setPalette(_palette);
}

void Workplace::updateState()
{
    switch(_state){
    case WorkplaceState::Disconnected:
        workplace_name_lb.setText("-");
        setButtonSet(false, false, false);
        setWidgetColor(&workplace_state_lb, QPalette::ColorRole::Window, disconnectedColor);
        _name = "";
        break;
    case WorkplaceState::ConnectedNN:
        workplace_name_lb.setText("Noname");
        setButtonSet(true, false, false);
        setWidgetColor(&workplace_state_lb, QPalette::ColorRole::Window, connectedColor);
        break;
    case WorkplaceState::Connected:
        setButtonSet(true, true, true);
        break;
    case WorkplaceState::Waiting: case WorkplaceState::Working: case WorkplaceState::Checking:
        setButtonSet(false, true, false);
        break;
    }
    workplace_state_lb.setText(getWorkplaceStateText(_state));
}

void Workplace::setButtonSet(bool rename_btn, bool state_btn, bool task_btn)
{
    rename_btn  ? workplace_rename_btn.show()   : workplace_rename_btn.hide();
    state_btn   ? workplace_state_btn.show()    : workplace_state_btn.hide();
    task_btn    ? workplace_task_btn.show()     : workplace_task_btn.hide();
}
