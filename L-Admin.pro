QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    server.cpp \
    settings.cpp \
    studentdatabase.cpp \
    taskdatabase.cpp \
    workplace.cpp

HEADERS += \
    mainwindow.h \
    server.h \
    settings.h \
    studentdatabase.h \
    styles.h \
    taskdatabase.h \
    workplace.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../L-Canvas_0_23/release/ -lL-Canvas
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../L-Canvas_0_23/debug/ -lL-Canvas
else:unix: LIBS += -L$$PWD/../L-Canvas_0_23/ -lL-Canvas

INCLUDEPATH += $$PWD/../L-Canvas_0_23
DEPENDPATH += $$PWD/../L-Canvas_0_23

DISTFILES +=

RESOURCES +=
