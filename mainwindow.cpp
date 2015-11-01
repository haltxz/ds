#include "addaccount.h"
#include "addpay.h"
#include "dblist.h"
#include "mainwindow.h"
#include "tabwidget_maintool.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QString>
#include <QDir>
#include <QSqlTableModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    ui->setupUi(this);

    TabWidget_MainTool *tb = new TabWidget_MainTool();
    ui->mainToolBar->addWidget(tb);
    connect(tb , SIGNAL( ButtonClicked(QString, QString) ), this, SLOT( ToolBarBtnClick(QString, QString)) );


    downAll();
   /* widgetname = "about";
    about = new About();
    ui->vlwidget->addWidget(about);*/
    widgetname = "TeachersPayment";
    tp = new TeachersPayment();
    tp->SetDb(&myDB);
    ui->vlwidget->addWidget(tp);

}

MainWindow::~MainWindow()
{
    downAll();

    delete ui;
}

void MainWindow::ToolBarBtnClick(QString name, QString title)
{
    downAll();
    bool use = false;
    QString type = "list";
    widgetname = name;
    if (name == "add_pay"){
       ap = new AddPay();
       ap->SetDb(&myDB);
       ui->vlwidget->addWidget(ap);
       use = true;
    }
    if (name == "add_acc"){
       ac = new AddAccount();
       ac->SetDb(&myDB);
       ui->vlwidget->addWidget(ac);
       use = true;
    }

    if (name == "kvitok"){
       pk = new PringKvitok();
       pk->SetDb(&myDB);
       ui->vlwidget->addWidget(pk);
       use = true;
    }
    if (name == "about"){
        about = new About();
        ui->vlwidget->addWidget(about);
       use = true;
    }
    if (name == "tabel"){
        tabel = new printtabel();
        tabel->SetDb(&myDB);
        ui->vlwidget->addWidget(tabel);
       use = true;
    }
    if (name == "TeachersPayment"){
        tp = new TeachersPayment();
        tp->SetDb(&myDB);
        ui->vlwidget->addWidget(tp);
       use = true;
    }
    if (!use){
        upModel(myDB.setupModel(name));
        updblist(new dblist());
        ui->vlwidget->addWidget(current_dblist);
        current_dblist->setModel(current_model, name, title, type);
    }
}


void MainWindow::upModel(QSqlRelationalTableModel *item)
{
    if (count_model > 0) {
        delete current_model;
        count_model = 0;
    }
    current_model = item;
    count_model++;
}


void MainWindow::updblist(dblist *item)
{
    if (count_dblist > 0) {
        delete current_dblist;
        count_dblist = 0;
    }
    current_dblist = item;
    count_dblist++;
}

void MainWindow::downAll()
{
    if (widgetname == "add_pay")delete ap;
    if (widgetname == "add_acc")delete ac;
    if (widgetname == "about")delete about;
    if (widgetname == "kvitok")delete pk;
    if (widgetname == "tabel")delete tabel;
    if (widgetname == "TeachersPayment")delete tp;
    if (count_model > 0) delete current_model;
    count_model = 0;
    if (count_dblist > 0)delete current_dblist;
    count_dblist = 0;
    widgetname = "";
}


