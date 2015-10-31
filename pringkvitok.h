#ifndef PRINGKVITOK_H
#define PRINGKVITOK_H

#include "db.h"

#include <QCheckBox>
#include <QGroupBox>

namespace Ui {
class PringKvitok;
}

class PringKvitok : public QGroupBox
{
    Q_OBJECT

public:
    explicit PringKvitok(QWidget *parent = 0);
    ~PringKvitok();
    void SetDb(db *mdb);
    QString prepareKvit(QString child_id);

private slots:
    void on_toolButton_select_clicked();
    void on_childs_toggled(bool checked);
    void on_toolButton_print_clicked();

    void on_toolButton_pdf_clicked();

private:
    Ui::PringKvitok *ui;
    db *myDB;
    QMap<QString, QCheckBox*> childsCheckBox_map;
};

#endif // PRINGKVITOK_H
