#include "printtabel.h"
#include "ui_printtabel.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QPrintDialog>
#include <QPrinter>
#include <QShortcut>
#include <QTextEdit>
#include <QCheckBox>

printtabel::printtabel(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::printtabel)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());

    ui->tW->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tW->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tW->resizeColumnsToContents();
    ui->tW->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tW->horizontalHeader()->setStretchLastSection(true);

    ui->tW->setColumnCount(4);
    ui->tW->setHorizontalHeaderLabels(QString("В;Группа;Иноформация;id;").split(";"));
    ui->tW->setColumnWidth(0, 20);
    ui->tW->setColumnHidden(3, true);

}

printtabel::~printtabel()
{
    delete ui;
}

void printtabel::SetDb(db *mdb)
{
     myDB = mdb;
    QString sql = "SELECT id, name, info FROM groups WHERE id > 2 ";

     QSqlQueryModel *groups = myDB->Query(sql);
     for(int i = 0; i < groups->rowCount(); i++){
           QString group_id = groups->record(i).value("id").toString();
           ui->tW->insertRow(ui->tW->rowCount());
           ui->tW->setItem(ui->tW->rowCount() - 1, 1,new QTableWidgetItem(groups->record(i).value("name").toString()));
           ui->tW->setItem(ui->tW->rowCount() - 1, 2,new QTableWidgetItem(groups->record(i).value("info").toString()));
           ui->tW->setItem(ui->tW->rowCount() - 1, 3,new QTableWidgetItem(group_id));

           QCheckBox *cb = new QCheckBox();
           cb->setText(group_id);
           //connect(cb , SIGNAL( toggled(bool) ), this, SLOT( on_childs_toggled(bool)) );
           groupsCheckBox_map[group_id] = cb;
           ui->tW->setCellWidget(ui->tW->rowCount() - 1, 0, cb);

      }
     delete groups;
}

QString printtabel::prepareTabel(QString group_id)
{

    QString pg;
    QString sql = "SELECT fio FROM childs WHERE group_id="+group_id;
    QSqlQueryModel *child = myDB->Query(sql);
    QString group_name;
    for(int z = 0; z < ui->tW->rowCount(); z++){
        if (ui->tW->item(z, 3) != 0 && ui->tW->item(z, 3)->text() == group_id )
          group_name =  ui->tW->item(z, 1)->text();
    }

    pg = "<html><head><meta http-equiv='content-type' content='text/html; charset=utf-8'>   <style>      body {        width: 297mm;          height: 210mm;        padding: 15mm 15mm 15mm 25mm;         background-color: white;          font-family:  'Times New Roman';         font-size: 150px;      }      td{	font-size: 150px;      }      th{	font-size: 150px;      }        .tdc{	text-align:center;      }    </style>    </head><body>";
    pg += "<table  border=3 cellpadding=1 cellspacing=0 width=100%><tr><td>";
    pg += " Табель посещаемости группа "+ group_name +" МДОУ «Детский сад №81». Дата " + ui->dateEdit->text() + "</td></tr>";
    pg += "<tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td></tr></table>";
    pg += "</h3><table  border=3 cellpadding=1 cellspacing=0 width=100%  style='page-break-after:always;'><tr><th>№</th><th>ФИО</th>";
    for(int i = 1; i < 32; i++ ){
        pg += "<th>" + QString::number(i) + "</th>";
    }
    pg += "<th>Всего</th></tr>";
    for(int j = 0; j < child->rowCount(); j++){
        QString fio = child->record(j).value("fio").toString();
        pg += "<tr><td>" +  QString::number(j + 1) + "</td><td>"+ fio + "</td>";
        for(int i = 1; i < 33; i++ ){
            pg += "<td>&nbsp;</td>";
        }
        pg += "</tr>";
    }
    pg += "</table>";
    delete child;
    return pg;
}

void printtabel::on_pushButton_pdf_clicked()
{
#if !defined(QT_NO_PRINTER)

    QString pg;
    QMapIterator<QString, QCheckBox*> i(groupsCheckBox_map);

    while (i.hasNext()) {
        i.next();
         QCheckBox *cb = i.value();
         if (cb->checkState() == Qt::Checked){
             pg += prepareTabel(cb->text());
          }
    }


     QTextDocument textDocument;

     textDocument.setHtml(pg);
     QPrinter printer(QPrinter::HighResolution);

     printer.setOutputFormat(QPrinter::PdfFormat);
     printer.setOutputFileName( "tab_"+ ui->dateEdit->text()+".pdf");

     printer.setOrientation(QPrinter::Landscape);
     printer.setPaperName("A4");

     printer.setPageMargins(1, 0.5, 1, 0.5, QPrinter::Unit() );
     textDocument.print(&printer);
 #endif
}

void printtabel::on_toolButton_print_clicked()
{
#if !defined(QT_NO_PRINTER)

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);

   dlg->setWindowTitle(tr("Настройки принтера"));
   if (dlg->exec() == QDialog::Accepted){
       QString pg;
       QMapIterator<QString, QCheckBox*> i(groupsCheckBox_map);

       while (i.hasNext()) {
           i.next();
            QCheckBox *cb = i.value();
            if (cb->checkState() == Qt::Checked){
                pg += prepareTabel(cb->text());
             }
       }
        QTextDocument textDocument;

        textDocument.setHtml(pg);
         QPrinter printer(QPrinter::HighResolution);
         printer.setOrientation(QPrinter::Landscape);
         printer.setPaperName("A4");
         printer.setPageMargins(1, 0.5, 1, 0.5, QPrinter::Unit() );
         textDocument.print(&printer);
   }
   delete dlg;
 #endif
}
