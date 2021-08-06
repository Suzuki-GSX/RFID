#ifndef DATABASE_H
#define DATABASE_H


#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include<QtDebug>
#include<QString>
#include <QDate>
#include <QTableView>
#include <QHeaderView>


static QSqlDatabase createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rfid.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to e  stablish a database connection.\n"
                        "This example needs SQLite support. Please read "
                        "the Qt SQL driver documentation for information how "
                        "to build it.\n\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
//        return false;
    }


/*
    QSqlQuery query(db);

    if (!query.exec("CREATE TABLE student ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name VARCHAR,"
                    "money INTEGER,"
                    "state VARCHAR)")) {
//        QMessageBox::critical(0, QObject::tr("Database Error"),
//                              query.lastError().text());

    }
    else{
        qDebug() << "插入数据employee";
        query.exec("insert into student(id,name,money,state) values(1032,'wt',5000,'normal');");
        query.exec("insert into student(id,name,money,state) values(1033,'wxx',5000,'normal');");
        query.exec("insert into student(id,name,money,state) values(1113,'hyk',5000,'normal');");
        qDebug() << "插入数据employee  ok ";
    }*/

    return db;

}


#endif // DATABASE_H
