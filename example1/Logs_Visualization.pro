QT       += core gui
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    datavectormodel.cpp \
    main.cpp \
    mainwindow.cpp \
    processdata.cpp \
    readdata.cpp \
    windowcontroller.cpp


HEADERS += \
    Data_Model.h \
    data_structure.h \
    datavectormodel.h \
    mainwindow.h \
    processdata.h \
    processed_data_struct.h \
    readdata.h \
    windowcontroller.h


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res_icon.qrc
