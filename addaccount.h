#ifndef ADDACCOUNT_H
#define ADDACCOUNT_H

#include "db.h"
#include "freezetablewidget.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QStandardItemModel>
#include <QPrinter>

namespace Ui {
class AddAccount;
}

class AddAccount : public QGroupBox
{
    Q_OBJECT

public:
    explicit AddAccount(QWidget *parent = 0);
    ~AddAccount();
    void SetDb(db *mdb);
    void fillData();
    void calc();
    QString prepareVedomost();
    QString prepareKvit(QString child_id);
    QString prepareRashiforvka();
    void to_pdf(QString fname, QString data, QPrinter::Orientation Orientation);
    void to_print(QString data, QPrinter::Orientation Orientation);

private slots:
    void on_toolButton_fill_clicked();

    void on_toolButton_save_clicked();

    void on_toolButton_print_clicked();

    void on_toolButton_clicked();

    void on_tW_cellChanged(QStandardItem *item);

    void on_teachers_toggled(bool checked);

    void on_comboBox_groups_currentIndexChanged(int index);

    void on_dateEdit_timeChanged(const QTime &time);

    void on_dateEdit_editingFinished();
    void on_PrintMenu(QAction* act);


private:
    Ui::AddAccount *ui;
    db *myDB;
    QMap<int, int> classes_map;
    QMap<int, QString> classesByNum_map;
    QMap<int, QString> classesNameByNum_map;
    QMap<int, float> classesPriceByNum_map;
    QMap<int, float> classesSumByNum_map;
    QMap<QString, QString> childs_map;
    QMap<QString, QString> teachers_map;
    QMap<QString, bool> teachersCheckBox_map;
    QMap<int, QString> accounts_id;
    QString printName;
    QMap<int, float> child_dolg;
    bool filled;
    FreezeTableWidget *tW;
    QStandardItemModel *model;
    QMenu* m_pmnu;
    int from_btn_print;

};

#endif // ADDACCOUNT_H
