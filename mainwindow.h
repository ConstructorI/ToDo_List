#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QStyleFactory>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QDateTime>
#include <QLabel>
#include <QTextEdit>
#include <QTimer>
#include <windows.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void update_table();

protected:
    void closeEvent(QCloseEvent *event) override {
        if (trayIcon->isVisible()) {
            hide();
            event->ignore();
        }
    }

private slots:
    void on_pushButton_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            showNormal();
        }
    }

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QTimer *timer;
    QVector<QStringList> time_list;

    QSqlQueryModel *sqlmodel;
    QStandardItemModel *model;
};

class DateTimeDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QString displayText(const QVariant &value, const QLocale &locale) const override {
        QDateTime dateTime = QDateTime::fromString(value.toString(), Qt::ISODate);
        return dateTime.toString("dd.MM.yyyy HH:mm");
    }
};

#endif // MAINWINDOW_H
