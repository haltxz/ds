#include "db.h"
#include <QDebug>
#include <QFileInfo>
#include <QString>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
db::db(QObject *parent) : QObject(parent)
{
    dbopen = false;
    myDB = QSqlDatabase::addDatabase("QSQLITE");

     QString pathToDB = QDir::currentPath()+QString("/db.sqlite");
     myDB.setDatabaseName(pathToDB);


     QFileInfo checkFile(pathToDB);

     if (checkFile.isFile()) {
         if (myDB.open()) {
              qDebug() << "[+] Connected to Database File";
              dbopen = true;
         }
         else {
              qDebug() << "[!] Database File was not opened";
         }
     }
     else {
         qDebug() << "[!] Database File does not exist";
     }
     if (dbopen){
//         config = new QSqlTableModel(this);
//         config->setTable("sys_config");
//         config->select();
//         colnames = new QSqlTableModel(this);
//         colnames->setTable("sys_colnames");
//         colnames->select();

     }

}

QSqlQueryModel *db::Query(QString Query)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    qDebug() << Query;
    model->setQuery(QSqlQuery(Query, myDB));
    if (model->lastError().type() != QSqlError::NoError)
       qDebug() << model->lastError().text();
    else if (model->query().isSelect())
       qDebug() << "Query OK.";
    else
       qDebug() << "Query OK, number of affected rows: " << model->query().numRowsAffected();
    return model;
}

QSqlDatabase *db::getDB()
{
    return &myDB;
}

/* Метод для инициализации модеи представления данных
 * */
QSqlRelationalTableModel *db::setupModel(const QString &tableName)
{
    /* Производим инициализацию модели представления данных
     * с установкой имени таблицы в базе данных, по которому
     * будет производится обращение в таблице
     * */
    QSqlRelationalTableModel  *model;
    model = new QSqlRelationalTableModel(this);
    model->setTable(tableName);
    if (tableName == "groups_filling")
        model->setRelation(2, QSqlRelation("groups", "id", "name"));
    if (tableName == "childs")
        model->setRelation(5, QSqlRelation("groups", "id", "name"));
    if (tableName == "teachers")
        model->setRelation(3, QSqlRelation("groups", "id", "name"));
    if (tableName == "pay")
        model->setRelation(1, QSqlRelation("childs", "id", "fio"));
    if (tableName == "accounts"){
        model->setRelation(1, QSqlRelation("childs", "id", "fio"));
        model->setRelation(2, QSqlRelation("teachers", "id", "fio"));
        model->setRelation(3, QSqlRelation("classes", "id", "name"));
        model->setRelation(4, QSqlRelation("groups", "id", "name"));
    }
    /* Устанавливаем названия колонок в таблице с сортировкой данных
     * */

    QSqlQueryModel *colnames;
    colnames = Query("SELECT * FROM sys_colnames WHERE sys_colnames.tab = '"+ tableName + "'");

   // qDebug() << colnames->rowCount();
    if (colnames->rowCount() > 0)
        for(int i = 0; i < model->columnCount(); i++){

            for (int j = 0; j < colnames->rowCount(); j++){
                QSqlRecord rec = colnames->record(j);
                if (rec.value("col").toString() == model->headerData(i, Qt::Horizontal).toString())
                    model->setHeaderData(i,Qt::Horizontal, rec.value("name").toString());
            }
        }
    // Устанавливаем сортировку по возрастанию данных по нулевой колонке
    model->setSort(1,Qt::AscendingOrder);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    delete colnames;
    return model;
}

db::~db()
{
    qDebug() << "Closing the connection to Database file on exist";
    myDB.close();

}

