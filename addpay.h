#ifndef ADDPAY_H
#define ADDPAY_H

#include "db.h"

#include <QGroupBox>

namespace Ui {
class AddPay;
}

class AddPay : public QGroupBox
{
    Q_OBJECT

public:
    explicit AddPay(QWidget *parent = 0);
    ~AddPay();
    void SetDb(db *mdb);
private slots:
    void on_toolButton_add_clicked();

    void on_lineEdit_sum_textEdited(const QString &arg1);
    void btnOKEnable();
    void fillData(QString arg1);

    void on_lineEdit_ls_textEdited(const QString &arg1);

    void on_toolButton_clear_clicked();

private:
    Ui::AddPay *ui;
    db *myDB;
    QString id;
};

#endif // ADDPAY_H
