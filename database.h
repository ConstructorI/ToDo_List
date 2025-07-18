#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool openDatabase(const QString &dbName);

    void insertTask(QString &name,
                    QString &desc,
                    QString &date,
                    QString &notif);

    void updateTask(int Id,
                    QString &name,
                    QString &desc,
                    QString &date,
                    QString &notif);

    void deleteTask(int Id);

    QVector<QStringList> get_data_times();

    QSqlDatabase db_r();

    void close_conn();


private:
    QSqlDatabase db;

};

#endif // DATABASE_H
