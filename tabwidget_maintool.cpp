#include "tabwidget_maintool.h"
#include "ui_tabwidget_maintool.h"
#include <QDebug>


TabWidget_MainTool::TabWidget_MainTool(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget_MainTool)
{
    ui->setupUi(this);

    QPushButton *btn = new QPushButton();
    btn->setObjectName("childs");
    btn->setText("Дети");
    btnList.append(btn);
    ui->horizontalLayoutList->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("teachers");
    btn->setText("Педагоги");
    btnList.append(btn);
    ui->horizontalLayoutList->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("groups");
    btn->setText("Группы");
    btnList.append(btn);
    ui->horizontalLayoutList->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("classes");
    btn->setText("Занятия");
    btnList.append(btn);
    ui->horizontalLayoutList->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("accounts");
    btn->setText("Начисления");
    btnList.append(btn);
    ui->horizontalLayoutList->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("pay");
    btn->setText("Оплата");
    btnList.append(btn);
    ui->horizontalLayoutList->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("add_acc");
    btn->setText("Начислить");
    btnList.append(btn);
    ui->horizontalLayoutDoc->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("add_pay");
    btn->setText("Оплата");
    btnList.append(btn);
    ui->horizontalLayoutDoc->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("kvitok");
    btn->setText("Квитанция");
    btnList.append(btn);
    ui->horizontalLayoutDoc->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("tabel");
    btn->setText("Табель");
    btnList.append(btn);
    ui->horizontalLayoutDoc->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("TeachersPayment");
    btn->setText("Выплаты");
    btnList.append(btn);
    ui->horizontalLayoutDoc->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("about");
    btn->setText("О программе");
    btnList.append(btn);
    ui->horizontalLayoutSys->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("sys_config");
    btn->setText("Конфигурация");
    btnList.append(btn);
    ui->horizontalLayoutSys->addWidget(btn);

    btn = new QPushButton();
    btn->setObjectName("sys_colnames");
    btn->setText("Псевдонимы");
    btnList.append(btn);
    ui->horizontalLayoutSys->addWidget(btn);

    for(int i = 0; i < btnList.count(); i++){
        connect(btnList.value(i) , SIGNAL( clicked() ), this, SLOT( on_pushButton_clicked() ) );
        btnList.value(i)->setCheckable(true);
    }
    QSpacerItem *qs = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->horizontalLayoutList->addItem(qs);
}

TabWidget_MainTool::~TabWidget_MainTool()
{
    for(int i = 0; i < btnList.count(); i++){
       delete btnList.value(i);
    }
    btnList.clear();
    delete ui;
}

void TabWidget_MainTool::on_pushButton_clicked()
{
    for(int i = 0; i < btnList.count(); i++){
        QPushButton *btn = btnList.value(i);
        btn->setChecked(false);
    }
    QPushButton *btn =  (QPushButton*)sender();
    btn->setChecked(true);
    emit ButtonClicked(btn->objectName(), btn->text());
}
