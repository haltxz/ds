#ifndef DB_H
#define DB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

class db : public QObject
{
    Q_OBJECT
public:
    explicit db(QObject *parent = 0);
    ~db();
    QSqlRelationalTableModel *setupModel(const QString &tableName);
    QSqlQueryModel *Query(QString Query);
    QSqlDatabase *getDB();
signals:

public slots:
private:
    bool dbopen;
    QSqlDatabase myDB;
    QSqlTableModel  *colnames;
    QSqlTableModel  *config;

};

#endif // DB_H
