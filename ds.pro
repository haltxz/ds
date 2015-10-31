#-------------------------------------------------
#
# Project created by QtCreator 2015-10-20T13:59:54
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(printsupport): QT += printsupport

TARGET = ds
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    db.cpp \
    dblist.cpp \
    tabwidget_maintool.cpp \
    addpay.cpp \
    addaccount.cpp \
    about.cpp \
    pringkvitok.cpp \
    printtabel.cpp \
    teacherspayment.cpp \
    freezetablewidget.cpp

HEADERS  += mainwindow.h \
    db.h \
    dblist.h \
    tabwidget_maintool.h \
    addpay.h \
    addaccount.h \
    about.h \
    pringkvitok.h \
    printtabel.h \
    teacherspayment.h \
    freezetablewidget.h

FORMS    += mainwindow.ui \
    dblist.ui \
    tabwidget_maintool.ui \
    addpay.ui \
    addaccount.ui \
    about.ui \
    pringkvitok.ui \
    printtabel.ui \
    teacherspayment.ui
