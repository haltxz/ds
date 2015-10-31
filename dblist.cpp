#include "dblist.h"
#include "ui_dblist.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

dblist::dblist(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::dblist)
{   

    ui->setupUi(this);  

}

dblist::~dblist()
{
    delete ui;
}

void dblist::setModel(QSqlRelationalTableModel *model, QString name, QString caption, QString type)
{
    this->setTitle(caption);
    this->model = model;
    this->type = type;
    this->name = name;

    ui->tableView->setModel(model);     // Устанавливаем модель на TableView
    if (type != "doc")ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
   //  Разрешаем выделение строк
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    if (type == "doc")ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    if (name == "childs")  ui->tableView->setItemDelegate(new QSqlRelationalDelegate( ui->tableView));
    if (name == "teachers")  ui->tableView->setItemDelegate(new QSqlRelationalDelegate( ui->tableView));
    if (name == "accounts")  ui->tableView->setItemDelegate(new QSqlRelationalDelegate( ui->tableView));
    if (name == "pay")  ui->tableView->setItemDelegate(new QSqlRelationalDelegate( ui->tableView));

}



void dblist::on_toolButton_addRow_clicked()
{

        ui->tableView->model()->insertRow(0);
        ui->tableView->model()->submit();
}

void dblist::on_toolButton_refresh_clicked()
{
    model->select();
}

void dblist::on_toolButton_delRow_clicked()
{
   ui->tableView->model()->removeRow(ui->tableView->currentIndex().row());
   model->submitAll();
   model->select();
}


void dblist::on_toolButton_save_clicked()
{    
    if (name == "childs"){
        QString fio;
        QString idx_fio;
        for(int i = 0; i < model->rowCount(); i++){
            fio = model->record(i).value("fio").toString().toLower();
            idx_fio = model->record(i).value("idx").toString();
            if (fio != idx_fio){
                QModelIndex index = model->index(i,7);
                model->setData(index, fio);
            }
        }
    }
    model->submitAll();
}

void dblist::on_toolButton_edit_clicked()
{

}
