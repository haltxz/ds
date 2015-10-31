#ifndef TEACHERSPAYMENT_H
#define TEACHERSPAYMENT_H

#include "db.h"

#include <QGroupBox>

namespace Ui {
class TeachersPayment;
}

class TeachersPayment : public QGroupBox
{
    Q_OBJECT

public:
    explicit TeachersPayment(QWidget *parent = 0);
    ~TeachersPayment();
    void SetDb(db *mdb);
    void fillData();
private slots:
    void on_toolButton_clicked();

    void on_toolButton_save_clicked();

private:
    Ui::TeachersPayment *ui;
    db *myDB;
};

#endif // TEACHERSPAYMENT_H
