#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "about.h"
#include "addaccount.h"
#include "addpay.h"
#include "db.h"
#include "dblist.h"
#include "pringkvitok.h"
#include "printtabel.h"
#include "teacherspayment.h"

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void ToolBarBtnClick(QString name, QString title);

private:
    Ui::MainWindow *ui;
    db myDB;
    QSqlRelationalTableModel *current_model;
    dblist *current_dblist;
    AddAccount *ac;
    AddPay *ap;
    About *about;
    PringKvitok *pk;
    printtabel *tabel;
    TeachersPayment *tp;
    int count_dblist = 0;
    int count_model = 0;
    QString widgetname;
    void upModel(QSqlRelationalTableModel *item);
    void updblist(dblist *item);
    void downAll();

};

#endif // MAINWINDOW_H
