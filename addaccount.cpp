#include "addaccount.h"
#include "ui_addaccount.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QPrintDialog>
#include <QPrinter>
#include <QShortcut>
#include <QTextEdit>
#include <QCheckBox>
#include <cmath>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QHeaderView>
#include "freezetablewidget.h"



AddAccount::AddAccount(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::AddAccount)
{
    ui->setupUi(this);
   // filled = false;

    ui->dateEdit->setDate(QDate::currentDate());
    model = new QStandardItemModel();
    connect(model , SIGNAL( itemChanged(QStandardItem *) ), this, SLOT( on_tW_cellChanged(QStandardItem *)) );


    ui->tW_ped->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tW_ped->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tW_ped->resizeColumnsToContents();
    ui->tW_ped->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tW_ped->horizontalHeader()->setStretchLastSection(true);

    ui->tW_aped->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tW_aped->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tW_aped->resizeColumnsToContents();
    ui->tW_aped->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tW_aped->horizontalHeader()->setStretchLastSection(true);

    ui->tW_aped->setColumnCount(3);
    ui->tW_aped->setHorizontalHeaderLabels(QString("В;Педагог;id").split(";"));
    ui->tW_aped->setColumnWidth(0, 20);
    ui->tW_aped->setColumnHidden(2, true);



}

AddAccount::~AddAccount()
{
    delete ui;
}

void AddAccount::SetDb(db *mdb)
{
    myDB = mdb;

    for(int i = 0; i < 6; i++){
        model->insertColumn(model->columnCount());
        //tW->insertColumn(tW->columnCount());
    }
    //model->setColumnCount(6);


    ui->tW_ped->setColumnCount(4);
    QString headers = "Ребенок;Педагог;";
    QString classes_h = "";
    QSqlQueryModel *classes = myDB->Query("SELECT * FROM classes");
    for(int i = 0; i < classes->rowCount(); i++){
        model->insertColumn(model->columnCount());
        ui->tW_ped->insertColumn(ui->tW_ped->columnCount());
        classes_h += classes->record(i).value("name").toString()+";";
        classes_map.insert(classes->record(i).value("id").toInt(), i + 2);
        classesByNum_map.insert(i + 2, classes->record(i).value("id").toString());
        classesNameByNum_map.insert(i + 2, classes->record(i).value("name").toString());
        classesPriceByNum_map.insert(i + 2, classes->record(i).value("price").toFloat());
    }

    headers += classes_h + "Зан.;Нач.;Долг;Итого к оплате;";    
    model->setHorizontalHeaderLabels(headers.split(";"));


    tW = new FreezeTableWidget(model);
   // tW->setColumnWidth(1, 300);
    tW->setObjectName("tW");
    tW->setSelectionBehavior(QAbstractItemView::SelectRows);
    tW->setSelectionMode(QAbstractItemView::SingleSelection);
    tW->resizeColumnsToContents();
    tW->setColumnWidth(0, tW->columnWidth(0) + 10);
    tW->setColumnWidth(1, tW->columnWidth(1) + 10);
    tW->setColumnWidth(2, tW->columnWidth(2) + 50);
    tW->horizontalHeader()->setStretchLastSection(true);
    ui->verticalLayout_tw->addWidget(tW);

    headers = "Педагог;" + classes_h + "Итого часов;Сумма;Выплата педагогу";
    ui->tW_ped->setHorizontalHeaderLabels(headers.split(";"));

    QSqlQueryModel *groups = myDB->Query("SELECT * FROM groups WHERE id > 2");
    for(int i = 0; i < groups->rowCount(); i++){
         ui->comboBox_groups->addItem(groups->record(i).value("name").toString(), groups->record(i).value("id").toString());
    }

    delete classes;
    delete groups;

    fillData();
}

void AddAccount::fillData()
{
    filled = false;   
    model->setRowCount(0);
    ui->tW_ped->setRowCount(0);
    ui->tW_aped->setRowCount(0);

    childs_map.clear();
    teachers_map.clear();
    accounts_id.clear();
    child_dolg.clear();

    QString gid = ui->comboBox_groups->currentData().toString();
    QString date = ui->dateEdit->text();

    //заполняем окошко с дополнительными педагогами и на выбраных ставим галочку
    QSqlQueryModel *th = myDB->Query("SELECT * FROM teachers WHERE group_id > 2");
    QSqlQueryModel *th_use_in_group = myDB->Query("SELECT teacher_id FROM accounts WHERE group_id = " + gid + " AND date='"+date+"'  GROUP BY teacher_id");
    for(int i = 0; i < th->rowCount(); i++){
          //добавляем в окошко только педагого не из текущей группы
          if (th->record(i).value("group_id").toString() != gid){
              QString th_id = th->record(i).value("id").toString();
              ui->tW_aped->insertRow(ui->tW_aped->rowCount());
              ui->tW_aped->setItem(ui->tW_aped->rowCount() - 1, 1,new QTableWidgetItem(th->record(i).value("fio").toString()));
              ui->tW_aped->setItem(ui->tW_aped->rowCount() - 1, 2,new QTableWidgetItem(th_id));
              QCheckBox *cb = new QCheckBox();
              cb->setText(th_id);
              connect(cb , SIGNAL( toggled(bool) ), this, SLOT( on_teachers_toggled(bool)) );
              ui->tW_aped->setCellWidget(ui->tW_aped->rowCount() - 1, 0, cb);
              for(int j = 0; j < th_use_in_group->rowCount(); j++){

                   if (th_id == th_use_in_group->record(j).value("teacher_id").toString()){
                       cb->setChecked(true);
                       teachersCheckBox_map[th_id] = true;

                   }
              }
          }
    }
    delete th;
    delete th_use_in_group;

     // вормеруем список дополнительных педагогов по тем у которых есть галка или уже были начисления
    QString teacher_id = "0";
    QMapIterator<QString, bool>i(teachersCheckBox_map);
     while (i.hasNext()) {
         i.next();
         if (i.value()) teacher_id += ","+i.key();
     }
     teacher_id = "("+ teacher_id + ")";


    QSqlQueryModel *childs = myDB->Query("SELECT * FROM childs WHERE group_id > 2 AND group_id = " + gid);
    QSqlQueryModel *teachers = myDB->Query("SELECT * FROM teachers WHERE  group_id = " + gid + " OR id IN " + teacher_id );
    int count_teachers = teachers->rowCount();

    QString sql = "SELECT t2.fio, t1.name AS classes,sum(t0.hours) AS hours , sum(t0.hours)*t1.price AS sum, t2.percent AS pc FROM accounts t0";
     sql += " LEFT OUTER  JOIN classes  t1 ON t0.classes_id = t1.id";
     sql += " LEFT OUTER  JOIN teachers  t2 ON t0.teacher_id = t2.id";
     sql += " WHERE date = '"+date+"' AND t0.group_id =" + gid;
     sql += " GROUP BY date, t0.teacher_id, t0.classes_id";
     QSqlQueryModel *total_th = myDB->Query(sql);
     QString teacher = "";
     int sumhours = 0;
     int sum = 0;
     float percent = 0;
     for(int j = 0; j < total_th->rowCount(); j++){
        if (total_th->record(j).value("fio").toString() != teacher){
            ui->tW_ped->insertRow(ui->tW_ped->rowCount());
            sumhours = 0;
            sum = 0;
            percent = total_th->record(j).value("pc").toInt();
            ui->tW_ped->setItem(ui->tW_ped->rowCount() - 1, 0,new QTableWidgetItem(total_th->record(j).value("fio").toString()));
            teacher = total_th->record(j).value("fio").toString();
        }
        for(int i = 1; i < ui->tW_ped->columnCount() - 1; i++){
            if (ui->tW_ped->horizontalHeaderItem(i)->text() == total_th->record(j).value("classes").toString()){
                ui->tW_ped->setItem(ui->tW_ped->rowCount() - 1, i,new QTableWidgetItem(total_th->record(j).value("hours").toString()));
                sumhours += total_th->record(j).value("hours").toInt();
                sum += total_th->record(j).value("sum").toInt();
                float sumtoped = percent / 100 * sum;
               // qDebug() << percent * 100;
                ui->tW_ped->setItem(ui->tW_ped->rowCount() - 1, ui->tW_ped->columnCount() - 3,new QTableWidgetItem(QString::number(sumhours)));
                ui->tW_ped->setItem(ui->tW_ped->rowCount() - 1, ui->tW_ped->columnCount() - 2,new QTableWidgetItem(QString::number(sum)));
                ui->tW_ped->setItem(ui->tW_ped->rowCount() - 1, ui->tW_ped->columnCount() - 1,new QTableWidgetItem(QString::number(sumtoped)));
            }
        }
    }


    sql = "SELECT child_id, sum(sum) AS sum FROM accounts";
    sql += " WHERE date < '"+date+"'";
    sql += " GROUP BY child_id";
    QSqlQueryModel *child_acc_all = myDB->Query(sql);
    for(int j = 0; j < child_acc_all->rowCount(); j++){
       child_dolg[child_acc_all->record(j).value("child_id").toInt()] = child_acc_all->record(j).value("sum").toFloat();
    }
    sql = "SELECT child_id, sum(sum) AS sum FROM pay";
    sql += " WHERE date <= '"+date+"'";
    sql += " GROUP BY child_id";
    QSqlQueryModel *child_pay_all = myDB->Query(sql);
    for(int j = 0; j < child_pay_all->rowCount(); j++){
       int key = child_pay_all->record(j).value("child_id").toInt();
       if (child_dolg.contains(key)) child_dolg[key] -= child_pay_all->record(j).value("sum").toFloat();
    }

    //qDebug() <<  child_dolg[99];


    for(int i = 0; i < childs->rowCount(); i++){
        QSqlQueryModel *acc = myDB->Query("SELECT * FROM accounts WHERE group_id = " + gid + " AND date ='" + date + "'" + " AND child_id =" + childs->record(i).value("id").toString());
        int child_sum = 0;

        for(int j = 0; j < count_teachers; j++){
            model->insertRow(model->rowCount());
            int rowid = model->rowCount() - 1;
            model->setItem(rowid,1,new QStandardItem(teachers->record(j).value("fio").toString()));
            teachers_map.insert(teachers->record(j).value("fio").toString(), teachers->record(j).value("id").toString());
            //заполняем таблицу с занятиями
            for(int z = 0; z < acc->rowCount(); z++){
                QString tid = acc->record(z).value("teacher_id").toString();
               // int teacher_sum = 0;

                if (teachers->record(j).value("id").toString() == tid){
                    model->setItem(rowid, classes_map[acc->record(z).value("classes_id").toInt()], new QStandardItem(acc->record(z).value("hours").toString()));
                //    teacher_sum += acc->record(z).value("hours").toInt();
                    accounts_id.insert(rowid, acc->record(z).value("id").toString());
                }
                //child_sum += teacher_sum;
               // tW->setItem(rowid,  tW->columnCount() - 3, new QTableWidgetItem(QString::number(teacher_sum)));
            }
        }

        tW->setSpan(model->rowCount() - count_teachers, 0, count_teachers, 1);
        tW->setSpan(model->rowCount() - count_teachers, model->columnCount() - 2, count_teachers, 1);
        tW->setSpan(model->rowCount() - count_teachers, model->columnCount() - 3, count_teachers, 1);

        //итого к оплате
        model->setItem(model->rowCount() - count_teachers,  model->columnCount() - 2, new QStandardItem(QString::number(child_sum)));
        //долг
        if (child_dolg[childs->record(i).value("id").toInt()] > 0)
            model->setItem(model->rowCount() - count_teachers,  model->columnCount() - 3, new QStandardItem(QString::number(child_dolg[childs->record(i).value("id").toInt()])));
        //фио ребенка
        model->setItem(model->rowCount() - count_teachers,  0, new QStandardItem(childs->record(i).value("fio").toString()));
        childs_map.insert(childs->record(i).value("fio").toString(), childs->record(i).value("id").toString());

        delete acc;
    }

    delete childs;
    delete teachers;
    delete total_th;
    delete child_acc_all;
    delete child_pay_all;
    calc();
    tW->resizeColumnsToContents();
    filled = true;

}

void AddAccount::calc()
{
    filled = false;
    classesSumByNum_map.clear();
   // bool newteacher = false;
   // bool newchild = false;
    float sumh = 0;
    float sum = 0;
   // float dolg = 0;
    float childsum = 0;
    int rownewchild = 0;

    for(int i = 0; i < model->rowCount(); i++){
        if (model->item(i, 0) != 0) {
            //newchild = true;
            childsum = 0;
            rownewchild = i;
           // dolg = 0;
            if (model->item(i, model->columnCount() - 3) != 0)
                childsum = model->item(i, model->columnCount() - 3)->text().toFloat();
        }
       // if (tW->item(i, 1) != 0) newteacher = true;

        sum = 0;
        sumh = 0;

        for(int j = 1; j < model->columnCount() - 5; j++){
            if (model->item(i, j) != 0){
                sumh += model->item(i, j)->text().toInt();
                sum += classesPriceByNum_map[j] * model->item(i, j)->text().toFloat();
                classesSumByNum_map[j] += model->item(i, j)->text().toFloat();
            }
        }
         model->setItem(i, model->columnCount() - 5,new QStandardItem(QString::number(sumh)));
         model->setItem(i, model->columnCount() - 4,new QStandardItem(QString::number(sum)));

         childsum += sum;
         model->setItem(rownewchild, model->columnCount() - 2,new QStandardItem(QString::number(childsum)));

        // newchild = false;
    }
    filled = true;
}

QString AddAccount::prepareVedomost()
{
    QString child= "";
    QString teacher = "";
    QString countspan = QString::number(teachers_map.count());
    QString date = ui->dateEdit->text();
    QString pg;
    QString head_tab;
    pg = "<html><head><meta http-equiv='content-type' content='text/html; charset=utf-8'>   <style>      body {        width: 297mm;          height: 210mm;        padding: 15mm 15mm 15mm 25mm;         background-color: white;          font-family:  'Times New Roman';         font-size: 150px;      }      td{	font-size: 150px;      }      th{	font-size: 150px;      }        .tdc{	text-align:center;      }    </style>    </head><body>";
    pg += "<h3>Ведомость за " + date+ " "+ ui->comboBox_groups->currentText() +"</h3>";
    head_tab = "<table  border=3 cellpadding=1 cellspacing=0 width=100% style='page-break-after:always;'>";
    head_tab += "<tr><th>№</th><th>Ребенок</th><th>Педагог</th>##classes<th>Зан.</th><th>Нач.</th><th>Долг</th><th>Итого</th></tr>";

    QString classes = "";
    QMapIterator<int, QString> i(classesNameByNum_map);
     while (i.hasNext()) {
         i.next();
         if (classesSumByNum_map.contains(i.key())){

            classes += "<th>" +i.value() +"</th>";
         }
    }
  // qDebug() << classes;
    head_tab.replace("##classes", classes);
    pg += head_tab;
    bool newchild = false;
    int count = 0;
    QString val = "";
    for(int i = 0; i < model->rowCount(); i++){
        if (model->item(i, 0) != 0) {
                child = model->item(i, 0)->text();
                newchild = true;
                count++;
                if (count == 19)pg += "</table><br>" + head_tab;
        }
        if (model->item(i, 1) != 0) teacher = model->item(i, 1)->text();
       pg += "<tr>";
       if (newchild)pg += "<td rowspan="+countspan+" class='tdc'>" +QString::number(count)+ "</td>" ;
       if (newchild)pg += "<td rowspan="+countspan+">" +child+ "</td>" ;
       pg += "<td>" +teacher+ "</td>" ;

//        for(int j = 2; j < model->columnCount() - 5; j++){
//            val = "&nbsp;";
//            if (model->item(i, j) != 0)
//                val = model->item(i, j)->text();
//            pg += "<td  class='tdc'>" +val+ "</td>" ;
//        }
        QMapIterator<int, QString> j(classesNameByNum_map);
         while (j.hasNext()) {
             j.next();
             if (classesSumByNum_map.contains(j.key())){
                val = "&nbsp;";
                if (model->item(i, j.key()) != 0)
                    val = model->item(i, j.key())->text();
                pg += "<td  class='tdc'>" + val + "</td>" ;
             }
        }

        val = "&nbsp;";
        if (model->item(i, model->columnCount() - 5) != 0)
            val = model->item(i, model->columnCount() - 5)->text();
        pg += "<td class='tdc'>" +val+ "</td>" ;

        val = "&nbsp;";
        if (model->item(i, model->columnCount() - 4) != 0)
            val = model->item(i, model->columnCount() - 4)->text();
        pg += "<td class='tdc'>" +val+ "</td>" ;

        val = "&nbsp;";
        if (model->item(i, model->columnCount() - 3) != 0)
            val = model->item(i, model->columnCount() - 3)->text();
        if (newchild)pg += "<td rowspan="+countspan+" class='tdc'>" +val+ "</td>" ;

        val = "&nbsp;";
        if (model->item(i, model->columnCount() - 2) != 0)
            val = model->item(i, model->columnCount() - 2)->text();
        if (newchild)pg += "<td rowspan="+countspan+" class='tdc'>" +val+ "</td>" ;


         pg += "</tr>";


         newchild = false;

    }

    pg += "</table>МДОУ «Детский сад №81»<br>";

    pg += "<table  border=3 cellpadding=1 cellspacing=0 width=100%>";
    pg += "<tr><th>№</th><th>Педагог</th>##classes<th>Итого часов</th><th>Сумма</th><th>К оплате</th></tr>";
    pg.replace("##classes", classes);

    for(int i = 0; i < ui->tW_ped->rowCount(); i++){
       if (ui->tW_ped->item(i, 0) != 0) teacher = ui->tW_ped->item(i, 0)->text();
       pg += "<tr>";
       pg += "<td class='tdc'>" +QString::number(i + 1)+ "</td>" ;
       pg += "<td>" +teacher+ "</td>" ;

//        for(int j = 1; j < ui->tW_ped->columnCount(); j++){
//            val = "&nbsp;";
//            if (ui->tW_ped->item(i, j) != 0)
//                val = ui->tW_ped->item(i, j)->text();
//            pg += "<td  class='tdc'>" +val+ "</td>" ;
//        }

       QMapIterator<int, QString> z(classesNameByNum_map);
        while (z.hasNext()) {
            z.next();
            if (classesSumByNum_map.contains(z.key())){
               val = "&nbsp;";
               if (ui->tW_ped->item(i, z.key() - 1) != 0)
                   val = ui->tW_ped->item(i, z.key() - 1)->text();
               pg += "<td  class='tdc'>" + val + "</td>" ;
            }
       }

        for(int j = ui->tW_ped->columnCount() - 3; j < ui->tW_ped->columnCount(); j++){
            val = "&nbsp;";
            if (ui->tW_ped->item(i, j) != 0)
                val = ui->tW_ped->item(i, j)->text();
            pg += "<td  class='tdc'>" +val+ "</td>" ;
        }

         pg += "</tr>";
    }
    pg +=   "</table>МДОУ «Детский сад №81»</body></html>";

    return pg;
}



void AddAccount::on_toolButton_fill_clicked()
{
    fillData();
}

void AddAccount::on_toolButton_save_clicked()
{
    QString child_id = "";
    QString teacher_id = "";
    QString gid = ui->comboBox_groups->currentData().toString();
    QString date = ui->dateEdit->text();
    QString sql;
    myDB->Query("DELETE FROM accounts WHERE date = '" + date + "' AND group_id = " + gid);
    for(int i = 0; i < model->rowCount() ; i++){
        if (model->item(i, 0) != 0) child_id = childs_map[model->item(i, 0)->text()];
        if (model->item(i, 1) != 0) teacher_id = teachers_map[model->item(i, 1)->text()];

        for(int j = 2; j < model->columnCount() - 5; j++){
            if (model->item(i, j) != 0 && model->item(i, j)->text() != "0"){
                float sum = classesPriceByNum_map[j] * model->item(i, j)->text().toInt();
                sql = "INSERT INTO accounts(child_id, teacher_id, classes_id, group_id, date, sum, info, hours) VALUES(";
                sql += child_id + ", ";
                sql += teacher_id + ", ";
                sql += classesByNum_map[j] + ", ";
                sql += gid + ", ";
                sql += "'" +date + "', ";
                sql += QString::number(sum) + ", ";
                sql += "'',";
                sql += model->item(i, j)->text();
                sql += ");\n";
               myDB->Query(sql);
              //  if (res->query().numRowsAffected() == 1)
              //      myDB->Query("DELETE FROM accounts WHERE id = " + accounts_id[i]);
            }
        }
    }
    fillData();

}


void AddAccount::on_toolButton_print_clicked()
{

    #if !defined(QT_NO_PRINTER)
         QTextEdit textEdit;
         textEdit.setHtml(prepareVedomost());
         QPrinter printer(QPrinter::HighResolution);

         printer.setOutputFormat(QPrinter::PdfFormat);
         QString pdfname = "v" + ui->dateEdit->text() + "_" + ui->comboBox_groups->currentText();
         printer.setOutputFileName(""+ pdfname+".pdf");

         printer.setOrientation(QPrinter::Landscape);
         printer.setPaperName("A4");

         printer.setPageMargins(1, 0.5, 1, 0.5, QPrinter::Unit() );
         textEdit.print(&printer);
     #endif

}

void AddAccount::on_toolButton_clicked()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);

   dlg->setWindowTitle(tr("Настройки принтера"));
   if (dlg->exec() == QDialog::Accepted){
     QTextEdit textEdit;
     textEdit.setHtml(prepareVedomost());
     QPrinter printer(QPrinter::HighResolution);
     printer.setOrientation(QPrinter::Landscape);
     printer.setPaperName("A4");
     textEdit.print(&printer);
   }
   delete dlg;
}

void AddAccount::on_tW_cellChanged(QStandardItem *item)
{
    //qDebug() << "item changed";
    if (filled)calc();

}




void AddAccount::on_teachers_toggled(bool checked)
{
     QCheckBox *cb =  (QCheckBox*)sender();
     teachersCheckBox_map[cb->text()] = checked;

     //qDebug() << cb->text().toInt();
}

void AddAccount::on_comboBox_groups_currentIndexChanged(int index)
{
    fillData();
    teachersCheckBox_map.clear();
}

void AddAccount::on_dateEdit_timeChanged(const QTime &time)
{
   fillData();
   teachersCheckBox_map.clear();
}

void AddAccount::on_dateEdit_editingFinished()
{
    fillData();
    teachersCheckBox_map.clear();
}

