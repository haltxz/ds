#include "pringkvitok.h"
#include "ui_pringkvitok.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QPrintDialog>
#include <QPrinter>
#include <QShortcut>
#include <QTextEdit>
#include <QCheckBox>
#include <cmath>

PringKvitok::PringKvitok(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::PringKvitok)
{

    ui->setupUi(this);

    ui->dateEdit->setDate(QDate::currentDate());

    ui->tW->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tW->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tW->resizeColumnsToContents();
    ui->tW->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tW->horizontalHeader()->setStretchLastSection(true);

    ui->tW->setColumnCount(6);
    ui->tW->setHorizontalHeaderLabels(QString("В;ФИО;Группа;id;group_id;Л/Счет;").split(";"));
    ui->tW->setColumnWidth(0, 20);
    ui->tW->setColumnHidden(3, true);
    ui->tW->setColumnHidden(4, true);
    ui->progressBar->setVisible(false);

}

PringKvitok::~PringKvitok()
{
    delete ui;
}

void PringKvitok::SetDb(db *mdb)
{
    myDB = mdb;
     ui->comboBox_groups->addItem("Выбрать все", "s_all");
    ui->comboBox_groups->addItem("Снять все", "d_all");
    QSqlQueryModel *groups = myDB->Query("SELECT * FROM groups WHERE id > 2");
    for(int i = 0; i < groups->rowCount(); i++){
         ui->comboBox_groups->addItem(groups->record(i).value("name").toString(), groups->record(i).value("id").toString());
    }

    childsCheckBox_map.clear();
   QString sql = "SELECT t0.id, t0.group_id, t0.fio,  t1.name AS group_name, t0.ls FROM childs t0";
    sql += " LEFT OUTER  JOIN groups  t1 ON t0.group_id = t1.id";
    QSqlQueryModel *childs = myDB->Query(sql);
    for(int i = 0; i < childs->rowCount(); i++){
          QString childs_id = childs->record(i).value("id").toString();
          ui->tW->insertRow(ui->tW->rowCount());
          ui->tW->setItem(ui->tW->rowCount() - 1, 1,new QTableWidgetItem(childs->record(i).value("fio").toString()));
          ui->tW->setItem(ui->tW->rowCount() - 1, 2,new QTableWidgetItem(childs->record(i).value("group_name").toString()));
          ui->tW->setItem(ui->tW->rowCount() - 1, 3,new QTableWidgetItem(childs_id));
          ui->tW->setItem(ui->tW->rowCount() - 1, 4,new QTableWidgetItem(childs->record(i).value("group_id").toString()));
          ui->tW->setItem(ui->tW->rowCount() - 1, 5,new QTableWidgetItem(childs->record(i).value("ls").toString()));
          QCheckBox *cb = new QCheckBox();
          cb->setText(childs_id);
          connect(cb , SIGNAL( toggled(bool) ), this, SLOT( on_childs_toggled(bool)) );
          childsCheckBox_map[childs_id] = cb;
          ui->tW->setCellWidget(ui->tW->rowCount() - 1, 0, cb);

     }
    delete groups;
    delete childs;

}

void PringKvitok::on_childs_toggled(bool checked)
{
   // QCheckBox *cb =  (QCheckBox*)sender();
  //  childsCheckBox_map[cb->text()] = checked;
    //qDebug() << cb->text();
}

void PringKvitok::on_toolButton_select_clicked()
{
    QMapIterator<QString, QCheckBox*> i(childsCheckBox_map);
    QString gid = ui->comboBox_groups->currentData().toString();
    bool flag = gid == "s_all";
    if (gid == "s_all" || gid == "d_all"){
        while (i.hasNext()) {
            i.next();
             QCheckBox *cb = i.value();
             cb->setChecked(flag);
        }
    }else {
        for(int z = 0; z < ui->tW->rowCount(); z++){
            if (ui->tW->item(z, 4) != 0 && ui->tW->item(z, 3) != 0)
                childsCheckBox_map[ui->tW->item(z, 3)->text()]->setChecked( ui->tW->item(z, 4)->text() == gid );
        }
    }
}

void PringKvitok::on_toolButton_print_clicked()
{
#if !defined(QT_NO_PRINTER)

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);

   dlg->setWindowTitle(tr("Настройки принтера"));
   if (dlg->exec() == QDialog::Accepted){
       QString pg;
       QMapIterator<QString, QCheckBox*> i(childsCheckBox_map);
       ui->progressBar->setVisible(true);
       int count = 0;
       int c = 0;
       while (i.hasNext()) {
           i.next();
            QCheckBox *cb = i.value();
            if (cb->checkState() == Qt::Checked){
                count++;
             }
       }

       ui->progressBar->setMaximum(count);
       while (i.hasPrevious()) {
           i.previous();
            QCheckBox *cb = i.value();
            if (cb->checkState() == Qt::Checked){
                pg += prepareKvit(cb->text());
                ui->progressBar->setValue(c++);
             }
       }
        QTextDocument textDocument;

        textDocument.setHtml(pg);
         QPrinter printer(QPrinter::HighResolution);
         printer.setOrientation(QPrinter::Portrait);
         printer.setPaperName("A4");
         printer.setPageMargins(1, 0.5, 1, 0.5, QPrinter::Unit() );
         if (ui->checkBox_duplex->checkState() == Qt::Checked)printer.setDuplex(QPrinter::DuplexAuto);
         textDocument.print(&printer);
          ui->progressBar->setVisible(false);
   }
   delete dlg;
 #endif
}

QString PringKvitok::prepareKvit(QString child_id)
{
    QSqlQueryModel *kvitok = myDB->Query("SELECT value FROM sys_config WHERE name='kvitok'");
    QString teml = kvitok->record(0).value("value").toString();
    QString pg;
    QString tmp;
    delete kvitok;


    float pay = 0;
    QString ls;
    QString fio;
    QString sql = "SELECT sum(sum) AS sum  FROM pay WHERE child_id="+child_id;
    QSqlQueryModel *child_pay_all = myDB->Query(sql);
    for(int j = 0; j < child_pay_all->rowCount(); j++){
        pay = child_pay_all->record(j).value("sum").toFloat();
    }

    sql = "SELECT t0.classes_id, sum(t0.sum) AS sum FROM accounts t0";
    sql += " WHERE t0.date ='"+ui->dateEdit->text()+"' AND t0.child_id=" + child_id;
    sql += " GROUP BY  t0.classes_id";
    QSqlQueryModel *child_acc_cur = myDB->Query(sql);

    sql = "SELECT t0.classes_id, t1.name AS classes, sum(t0.sum) AS sum,  t2.ls, t2.fio  FROM accounts t0";
    sql += " LEFT JOIN classes t1 ON t0.classes_id = t1.id";
    sql += " LEFT JOIN childs  t2 ON t0.child_id = t2.id";
    sql += " WHERE t0.date <='"+ui->dateEdit->text()+"' AND t0.child_id=" + child_id;
    sql += " GROUP BY t0.classes_id";
    QSqlQueryModel *child_acc_all = myDB->Query(sql);

    QMap<int, float> all;
    QMap<int, float> cur;
    QMap<int, float> minus;
    float sumall = 0;
    float sumcur = 0;

    for(int i = 0; i < child_acc_all->rowCount(); i++){
        all[child_acc_all->record(i).value("classes_id").toInt()] = child_acc_all->record(i).value("sum").toFloat();
        ls = child_acc_all->record(i).value("ls").toString();
        fio = child_acc_all->record(i).value("fio").toString();
    }
    for(int i = 0; i < child_acc_cur->rowCount(); i++){
        int id = child_acc_cur->record(i).value("classes_id").toInt();
        cur[id] = child_acc_cur->record(i).value("sum").toFloat();
        all[id] -= cur[id];
        //sumall += all[id];
    }


    QMapIterator<int, float> i(all);
    while (i.hasNext()) {
        i.next();
        sumall += i.value();
    }
    float op = pay;
    while (i.hasPrevious()) {
         i.previous();
         float percet = i.value() / sumall;
         int m = floor(pay * percet);
         if (i.value() < m) m = i.value();
         op -= m;
         minus[i.key()] = m;
     }
    QMapIterator<int, float> j(minus);
    while (j.hasNext()) {
        j.next();
        all[j.key()] -= j.value();
        if (all[j.key()] >= op && op > 0){
            all[j.key()] -= op;
            op = 0;
        }
    }

     //qDebug()  << op;

    tmp = "ФИО " + fio +" Переплата предыдущего периода: " + QString::number( op );
    tmp += "<table border=3 cellpadding=1 cellspacing=0 class='classes'>";
    tmp += "<tr><th></th><th>Долг</th><th>"+ui->dateEdit->text()+"</th><th>Итого</th></tr>";
    sumall = 0;
    for(int j = 0; j < child_acc_all->rowCount(); j++){
        int id = child_acc_all->record(j).value("classes_id").toInt();
        sumcur += cur[id];
        sumall += all[id];
        if ((all[id] + cur[id]) > 0){
        tmp += "<tr>";
        tmp += "<td>"+ child_acc_all->record(j).value("classes").toString()+"</td>";
        tmp += "<td>"+ QString::number( all[id] )+"</td>";
        tmp += "<td>"+ QString::number( cur[id] )+"</td>";
        tmp += "<td><i>"+ QString::number( all[id] + cur[id] ) +"</i></td>";
        tmp += "</tr>";
        }
    }
     tmp += "<tr><td></td>";
     tmp += "<td><i>"+ QString::number( sumall )+"</i></td>";
     tmp += "<td><i>"+ QString::number( sumcur )+"</i></td>";
     sumall += sumcur;
     tmp += "<td><b>"+ QString::number( sumall )+"</b></td>";
     tmp += "</tr>";
     tmp += "</table>МДОУ «Детский сад №81»";

     tmp = "<table border=0  style='page-break-after:always;'><tr><td><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br></td><td>"+  tmp +"</td></tr><tr><td></td><td>"+  tmp +"  </td></tr></table>";

     pg = tmp;
     float topay = sumall - op;
     if (topay < 0) topay = 0;
            tmp = teml;
            tmp.replace("##sum", QString::number(topay));
            tmp.replace("##date", ui->dateEdit->text());
            tmp.replace("##ls", ls);
            tmp.replace("##fio",  fio);
            pg = tmp + pg;


    delete child_pay_all;
    delete child_acc_cur;
    return pg;
}

void PringKvitok::on_toolButton_pdf_clicked()
{
#if !defined(QT_NO_PRINTER)

    QString pg;
    QMapIterator<QString, QCheckBox*> i(childsCheckBox_map);

    ui->progressBar->setVisible(true);
    int count = 0;
    int c = 0;
    while (i.hasNext()) {
        i.next();
         QCheckBox *cb = i.value();
         if (cb->checkState() == Qt::Checked){
             count++;
          }
    }

    ui->progressBar->setMaximum(count);
    while (i.hasPrevious()) {
        i.previous();
         QCheckBox *cb = i.value();
         if (cb->checkState() == Qt::Checked){
             pg += prepareKvit(cb->text());
             ui->progressBar->setValue(c++);
          }
    }
     QTextDocument textDocument;

     textDocument.setHtml(pg);
     QPrinter printer(QPrinter::HighResolution);

     printer.setOutputFormat(QPrinter::PdfFormat);
     printer.setOutputFileName( "print.pdf");

     printer.setOrientation(QPrinter::Portrait);
     printer.setPaperName("A4");

     printer.setPageMargins(1, 0.5, 1, 0.5, QPrinter::Unit() );
     textDocument.print(&printer);
     ui->progressBar->setValue(c++);
     ui->progressBar->setVisible(false);
 #endif
}
