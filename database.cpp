#include "database.h"
#include <QtSql>

Database::Database(QObject *parent)
    : QObject{parent}
{}

Database::~Database()
{
    if (db.isOpen()) {
        db.close();
    }
}

QSqlDatabase Database::db_r(){
    return db;
}

void Database::close_conn(){
    db.close();
}

bool Database::openDatabase(const QString &dbName)
{
    QDateTime currenttime;
    QString time_string = currenttime.currentDateTime().toString("yyyy/MM/dd hh:mm:ss,zzz");
    db = QSqlDatabase::addDatabase("QSQLITE", time_string);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
        return false;
    }
    QSqlQuery query(db);
    // 21
    QString createTableSI = "CREATE TABLE IF NOT EXISTS tasks ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "name TEXT, desc TEXT, date TEXT, notif BOOL)";
    if (!query.exec(createTableSI)) {
        qDebug() << "Ошибка создания таблицы:" << query.lastError().text();
        return false;
    }
    return true;
}

void Database::insertTask(QString &name,
                          QString &desc,
                          QString &date,
                          QString &notif){
    QSqlQuery query(db);

    query.prepare("INSERT INTO tasks (name, desc, date, notif) "
                  ""
                  "VALUES (:name, :desc, :date, :notif)");

    query.bindValue(":name", name);
    query.bindValue(":desc", desc);
    query.bindValue(":date", date);
    query.bindValue(":notif", notif);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Успешно добавлено.";
    }
}

void Database::updateTask(int Id,
                          QString &name,
                          QString &desc,
                          QString &date,
                          QString &notif){
    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET "
                  "name = :name, "
                  "desc = :desc, "
                  "date = :date, "
                  "notif = :notif "
                  "WHERE id = :id");
    query.bindValue(":id", Id);
    query.bindValue(":name", name);
    query.bindValue(":desc", desc);
    query.bindValue(":date", date);
    query.bindValue(":notif", notif);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления данных:" << query.lastError().text();
    } else {
        qDebug() << "Успешно обновлено.";
    }
}

void Database::deleteTask(int Id)
{
    QSqlQuery query(db);

    query.prepare("DELETE FROM tasks WHERE id = :id");
    query.bindValue(":id", Id);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления данных:" << query.lastError().text();
    } else {
        qDebug() << "Успешно удалено.";
    }
}

QVector<QStringList> Database::get_data_times(){
    QVector<QStringList> data;
    QSqlQuery query(db);

    QString zapros = "SELECT name, desc, date, notif FROM tasks";

    if (!query.exec(zapros)) {
        qDebug() << "Ошибка Select:" << query.lastError().text();
    }

    while (query.next()) {
        QString name_data = query.value(0).toString();
        QString desc_data = query.value(1).toString();
        QString date_data = query.value(2).toString();
        QString notif_data = query.value(3).toString();

        QStringList data_list;
        data_list << name_data << desc_data << date_data << notif_data;

        data.append(data_list);
    }
    return data;
}
