#ifndef DBLIST_H
#define DBLIST_H

#include <QGroupBox>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

namespace Ui {
class dblist;
}

class dblist : public QGroupBox
{
    Q_OBJECT

public:
    explicit dblist(QWidget *parent = 0);
    ~dblist();
    void setModel(QSqlRelationalTableModel *model, QString name, QString caption, QString type = "list");

signals:

private slots:
    void on_toolButton_addRow_clicked();
    void on_toolButton_refresh_clicked();
    void on_toolButton_delRow_clicked();  

    void on_toolButton_save_clicked();

    void on_toolButton_edit_clicked();

private:
    Ui::dblist *ui;
    QSqlRelationalTableModel  *model;
    QString type;
    QString name;
    QString id;
};

#endif // DBLIST_H
