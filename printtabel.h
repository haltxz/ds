#ifndef PRINTTABEL_H
#define PRINTTABEL_H

#include "db.h"

#include <QCheckBox>
#include <QGroupBox>

namespace Ui {
class printtabel;
}

class printtabel : public QGroupBox
{
    Q_OBJECT

public:
    explicit printtabel(QWidget *parent = 0);
    ~printtabel();
    void SetDb(db *mdb);
    QString prepareTabel(QString group_id);

private slots:
    void on_pushButton_pdf_clicked();

    void on_toolButton_print_clicked();

private:
    Ui::printtabel *ui;
    db *myDB;
    QMap<QString, QCheckBox*> groupsCheckBox_map;

};

#endif // PRINTTABEL_H
