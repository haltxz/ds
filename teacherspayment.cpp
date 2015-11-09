#include "teacherspayment.h"
#include "ui_teacherspayment.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QPrintDialog>
#include <QPrinter>
#include <QShortcut>
#include <QTextEdit>
#include <QCheckBox>

TeachersPayment::TeachersPayment(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::TeachersPayment)
{
     ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());

    ui->tW->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tW->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tW->resizeColumnsToContents();
    //ui->tW->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tW->horizontalHeader()->setStretchLastSection(true);

    ui->tW->setColumnCount(12);
    ui->tW->setHorizontalHeaderLabels(QString("Педагог;Занятие;Часы;Сумма;Процент;Начислено;Долг;teacher_id;classes_id;child_id;Выплата;").split(";"));
   // ui->tW->setColumnWidth(0, 20);
    //ui->tW->setColumnHidden(3, true);
}

TeachersPayment::~TeachersPayment()
{
    delete ui;
}

void TeachersPayment::SetDb(db *mdb)
{
    myDB = mdb;
    fillData();
}

void TeachersPayment::fillData()
{

     ui->tW->setRowCount(0);
    QString date = ui->dateEdit->text();
    ui->tableView_payment->setModel(myDB->setupModel("payment"));
    QString sql = "SELECT * FROM worked_money";
    QMultiMap<int, int> treewm;
    QMap<int, float> childs_pay;
    QMap<QString, float> teachers_pay;
    QMap<QString, int> teachers_row;
    sql = "SELECT t2.fio, t0.teacher_id, t1.name AS classes, t0.classes_id, sum(t0.hours) AS hours , sum(t0.hours)*t1.price AS sum, t2.percent,  t0.child_id";
    sql += " FROM accounts t0";
    sql += " LEFT OUTER  JOIN classes  t1 ON t0.classes_id = t1.id";
    sql += " LEFT OUTER  JOIN teachers  t2 ON t0.teacher_id = t2.id ";
    sql += " WHERE date ='"+ date +"'";
    sql += " GROUP BY  t0.teacher_id, t0.child_id, t0.classes_id";
    float allnachisleno = 0;
    QSqlQueryModel *wm = myDB->Query(sql);
    for(int i = 0; i < wm->rowCount(); i++){
          ui->tW->insertRow(ui->tW->rowCount());
          int row_id = ui->tW->rowCount() - 1;
          int child_id = wm->record(i).value("child_id").toInt();
          float sum = wm->record(i).value("sum").toFloat();
          float nachisleno = wm->record(i).value("percent").toFloat() / 100 * sum;
          allnachisleno += nachisleno;
          treewm.insert(child_id, row_id);
          QString mapkey = wm->record(i).value("teacher_id").toString() +
                  "-" + wm->record(i).value("classes_id").toString() +
                  "-" + wm->record(i).value("child_id").toString();

          teachers_pay[mapkey] += nachisleno;
          teachers_row[mapkey] = row_id;
          ui->tW->setItem(row_id, 0,new QTableWidgetItem(wm->record(i).value("fio").toString()));
          ui->tW->setItem(row_id, 1,new QTableWidgetItem(wm->record(i).value("classes").toString()));
          ui->tW->setItem(row_id, 2,new QTableWidgetItem(wm->record(i).value("hours").toString()));
          ui->tW->setItem(row_id, 3,new QTableWidgetItem(QString::number( sum )));
          ui->tW->setItem(row_id, 4,new QTableWidgetItem(wm->record(i).value("percent").toString()));
          ui->tW->setItem(row_id, 5,new QTableWidgetItem(QString::number( nachisleno )));
          ui->tW->setItem(row_id, 6,new QTableWidgetItem(QString::number( 0 )));
          ui->tW->setItem(row_id, 7,new QTableWidgetItem(wm->record(i).value("teacher_id").toString()));
          ui->tW->setItem(row_id, 8,new QTableWidgetItem(wm->record(i).value("classes_id").toString()));
          ui->tW->setItem(row_id, 9,new QTableWidgetItem(wm->record(i).value("child_id").toString()));
          ui->tW->setItem(row_id, 10,new QTableWidgetItem("0"));
     }

    //всего начисленно педагогу
    QSqlQueryModel *payment = myDB->Query("SELECT child_id, teacher_id, classes_id, sum(sum) AS sum , sum(pay) AS pay FROM payment WHERE date < '"+ date +"'GROUP BY teacher_id, classes_id, child_id");
    for(int z = 0; z < payment->rowCount(); z++){
        QString mapkey = payment->record(z).value("teacher_id").toString() +
                        "-" + payment->record(z).value("classes_id").toString() +
                        "-" + payment->record(z).value("child_id").toString();
        float dolg = payment->record(z).value("sum").toFloat() - payment->record(z).value("pay").toFloat();
        ui->tW->setItem(teachers_row[mapkey], 6,new QTableWidgetItem(QString::number( dolg )));
        teachers_pay[mapkey] += dolg;
    }

    //всего получено оплат
    float allpay = 0;
    QSqlQueryModel *ca = myDB->Query("SELECT child_id, sum(sum) AS sum FROM pay WHERE date <'" + date +"' OR  date ='" + date +"' GROUP BY child_id");
    for(int z = 0; z < ca->rowCount(); z++){
        int child_id = ca->record(z).value("child_id").toInt();
        childs_pay[child_id] = ca->record(z).value("sum").toFloat();
        allpay += ca->record(z).value("sum").toFloat();
    }

    //всего начисленно на ребенка
    QSqlQueryModel *cp = myDB->Query("SELECT child_id, sum(sum) AS sum FROM accounts WHERE date <'" + date +"' GROUP BY child_id");
    for(int z = 0; z < cp->rowCount(); z++){
        int child_id = cp->record(z).value("child_id").toInt();
        childs_pay[child_id] -= cp->record(z).value("sum").toFloat();
        allpay -= cp->record(z).value("sum").toFloat();
    }

    for(int z = 0; z < cp->rowCount(); z++){

        int child_id = cp->record(z).value("child_id").toInt();

        QList<int> values = treewm.values(child_id);
        for (int i = 0; i < values.size(); ++i){            
            float dolg = ui->tW->item(values.at(i), 6)->text().toFloat();
            float balans = childs_pay[child_id];
            float add = ui->tW->item(values.at(i), 5)->text().toFloat();
            float part = add;
            if ((balans - add) < 0){
               part = balans;
            }
            qDebug() << balans;
            if (balans > 0){
                part = part + dolg;
                childs_pay[child_id] += dolg;
              //  balans = childs_pay[child_id];
                ui->tW->setItem(values.at(i), 10,new QTableWidgetItem(QString::number(part)));
            }
            ui->tW->setItem(values.at(i), 11,new QTableWidgetItem(QString::number(balans)));
        }
    }

    sql = "SELECT  t2.fio, t1.name, t0.teacher_id, t0.classes_id, sum(t0.sum) AS sum , sum(t0.pay) AS pay FROM payment t0";
    sql += " LEFT OUTER  JOIN classes  t1 ON t0.classes_id = t1.id";
    sql += " LEFT OUTER  JOIN teachers  t2 ON t0.teacher_id = t2.id ";
    sql += " WHERE t0.date = '"+ date +"' OR sum < pay GROUP BY t0.teacher_id, t0.classes_id";

    ui->tableView->setModel( myDB->Query(sql) );
    ui->tableView->resizeColumnsToContents();
    ui->label_pay->setText(QString::number(allpay));
    ui->label_fot->setText(QString::number(allnachisleno));
    delete wm;
    delete cp;
}

void TeachersPayment::on_toolButton_clicked()
{
    fillData();
}

void TeachersPayment::on_toolButton_save_clicked()
{

    myDB->Query("DELETE FROM payment WHERE date = '" + ui->dateEdit->text() + "'");
    for(int i = 0; i < ui->tW->rowCount(); i++){
            float sum =  ui->tW->item(i, 5)->text().toFloat();
            float dolg =  ui->tW->item(i, 6)->text().toFloat();
            QString sql = "INSERT INTO payment(teacher_id, classes_id, child_id, date, sum, pay) VALUES(";
            sql += ui->tW->item(i, 7)->text() + ", ";
            sql += ui->tW->item(i, 8)->text() + ", ";
            sql += ui->tW->item(i, 9)->text() + ", ";
            sql += "'" + ui->dateEdit->text() + "', ";
            sql +=  QString::number(sum + dolg) + ", ";
            sql += ui->tW->item(i, 10)->text();
            sql += ")";
           myDB->Query(sql);
    }

}
