QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14 console

DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -lversion

SOURCES += \
    codegenerator.cpp \
    engine.cpp \
    main.cpp \
    connectionwindow.cpp \
    networkrequester.cpp \
    pipemanager.cpp \
    filepatching.cpp

HEADERS += \
    codegenerator.h \
    connectionwindow.h \
    engine.h \
    networkrequester.h \
    pipemanager.h \
filepatching.h

FORMS += \
    connectionwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
