#include "addpay.h"
#include "ui_addpay.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>
AddPay::AddPay(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::AddPay)
{
    ui->setupUi(this);
}

AddPay::~AddPay()
{
    delete ui;
}

void AddPay::SetDb(db *mdb)
{
    myDB = mdb;
}

void AddPay::on_toolButton_add_clicked()
{
    QString date = "'"+ QDate::currentDate().toString("MM.yyyy") +"'";
    QString sum = ui->lineEdit_sum->text();
    myDB->Query("INSERT INTO pay (child_id, date, sum) VALUES (" + id + "," + date + "," + sum + ")");

 /*   QString tmp = ui->lineEdit_ls->text();
    ui->lineEdit_ls->setText("");
    ui->lineEdit_ls->setText(tmp);
    fillData("");
    fillData(tmp);*/
    fillData("");
}

void AddPay::on_lineEdit_sum_textEdited(const QString &arg1)
{
    btnOKEnable();
}

void AddPay::btnOKEnable()
{
   if (id != "" && ui->lineEdit_sum->text() != "")  ui->toolButton_add->setEnabled(true);
    else ui->toolButton_add->setEnabled(false);
}

void AddPay::fillData(QString arg1)
{

    QString sql = "SELECT * FROM childs WHERE idx_fio LIKE '" + arg1 +"%'";
    if (id != "")sql = "SELECT * FROM childs WHERE id =" + id;

    QSqlQueryModel *child = myDB->Query(sql);
    ui->label_finded->setText(  QString::number( child->rowCount()) );
    if (child->rowCount() == 1){
        QSqlRecord rec = child->record(0);
        id = rec.value("id").toString();
        ui->label_fio->setText(rec.value("fio").toString());
        ui->label_info->setText(rec.value("info").toString());

        ui->lineEdit_ls->setText(rec.value("fio").toString());

        QSqlQueryModel *group = myDB->Query("SELECT name FROM groups WHERE id = " + rec.value("group_id").toString());
        rec = group->record(0);
        ui->label_group->setText(rec.value("name").toString());


        sql = "SELECT t0.id, t0.date, t0.hours, t0.sum, t1.name, t2.fio, t3.name, t0.info FROM accounts  t0" ;
        sql += " LEFT OUTER  JOIN classes  t1 ON t0.classes_id = t1.id";
        sql += " LEFT OUTER  JOIN teachers  t2 ON t0.teacher_id = t2.id";
        sql += " LEFT OUTER  JOIN groups t3 ON t0.group_id = t3.id";
        sql += " WHERE t0.child_id = " + id + " ORDER  BY t0.id DESC ";
        QSqlQueryModel *accounts = myDB->Query(sql);
        ui->tableView_acc->setModel(accounts);

        sql = "SELECT id, date, sum, info FROM pay WHERE child_id = " + id + " ORDER  BY id DESC";
        QSqlQueryModel *pay = myDB->Query(sql);
        ui->tableView_pay->setModel(pay);

        QSqlQueryModel *sumpay = myDB->Query("SELECT sum(sum) AS asum FROM pay WHERE child_id = " + id);
        QSqlQueryModel *sumpacc = myDB->Query("SELECT sum(sum) AS asum FROM accounts WHERE child_id = " + id);
        rec = sumpay->record(0);
        QString sump = rec.value("asum").toString();
        if (sump == "")sump="0";
        ui->label_paysum->setText(sump);

        rec = sumpacc->record(0);
        QString sumc = rec.value("asum").toString();
        if (sumc == "")sumc="0";
        ui->label_accsum->setText(sumc);

        float balans = sump.toFloat() - sumc.toFloat();
        ui->label_balans->setText(QString::number(balans));

        accounts->setHeaderData(0,Qt::Horizontal, "№");
        accounts->setHeaderData(1,Qt::Horizontal, "Дата");
        accounts->setHeaderData(2,Qt::Horizontal, "Часы");
        accounts->setHeaderData(3,Qt::Horizontal, "Сумма");
        accounts->setHeaderData(4,Qt::Horizontal, "Занятие");
        accounts->setHeaderData(5,Qt::Horizontal, "Педагог");
        accounts->setHeaderData(6,Qt::Horizontal, "Группа");
        accounts->setHeaderData(7,Qt::Horizontal, "Информация");

        pay->setHeaderData(0,Qt::Horizontal, "№");
        pay->setHeaderData(1,Qt::Horizontal, "Дата");
        pay->setHeaderData(2,Qt::Horizontal, "Сумма");
        pay->setHeaderData(3,Qt::Horizontal, "Информация");
    } else {
        QSqlQueryModel *clear = new QSqlQueryModel();
        ui->tableView_acc->setModel(clear);
        ui->tableView_pay->setModel(clear);
        ui->label_fio->setText("");
        ui->label_info->setText("");
        ui->label_group->setText("");
        ui->label_accsum->setText("");
        ui->label_paysum->setText("");
        ui->label_balans->setText("");
        id = "";
        btnOKEnable();

    }
        ui->tableView_acc->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView_acc->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView_acc->resizeColumnsToContents();
        ui->tableView_acc->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView_acc->horizontalHeader()->setStretchLastSection(true);

        ui->tableView_pay->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView_pay->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView_pay->resizeColumnsToContents();
        ui->tableView_pay->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView_pay->horizontalHeader()->setStretchLastSection(true);

}

void AddPay::on_lineEdit_ls_textEdited(const QString &arg1)
{
        fillData(arg1);
        btnOKEnable();
}

void AddPay::on_toolButton_clear_clicked()
{
    ui->lineEdit_ls->setText("");
    id = "";
    btnOKEnable();
}
