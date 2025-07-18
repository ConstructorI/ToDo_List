#include "add_task.h"
#include "ui_add_task.h"
#include "database.h"

#include <QDebug>

add_task::add_task(QWidget *parent, bool edit_mode)
    : QDialog(parent)
    , ui(new Ui::add_task)
    , is_edit(edit_mode)
{
    ui->setupUi(this);
    this->setStyle(QStyleFactory::create("Fusion"));
    this->setWindowIcon(QIcon("icon.ico"));

    if (is_edit){
        ui->pushButton_add->setHidden(true);
    } else {
        ui->pushButton_edit->setHidden(true);
        ui->pushButton_del->setHidden(true);
    }

    QString calendarStyle = R"(
        QCalendarWidget {
            background-color: rgb(25, 25, 25);
            color: rgb(255, 255, 255);
            border: 1px solid rgb(60, 60, 60);
        }

        QCalendarWidget QToolButton {
            background-color: rgb(25, 25, 25);
            color: rgb(255, 255, 255);
            font-size: 12px;
            icon-size: 0px, 0px;
            border: none;
        }

        QCalendarWidget QToolButton:hover {
            background-color: rgb(25, 25, 25);
        }

        QCalendarWidget QMenu {
            background-color: rgb(25, 25, 25);
            color: rgb(255, 255, 255);
            border: 1px solid rgb(60, 60, 60);
        }

        QCalendarWidget QMenu::item:selected {
            background-color: rgb(70, 70, 70);
        }

        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: rgb(25, 25, 25);
        }

        QCalendarWidget QAbstractItemView {
            alternate-background-color: rgb(30, 30, 30);
            selection-background-color: rgb(70, 70, 70);
            selection-color: rgb(255, 255, 255);
            gridline-color: rgb(60, 60, 60);
        }

        QCalendarWidget QAbstractItemView:enabled {
            color: rgb(255, 255, 255);
            font-size: 12px;
        }

        QCalendarWidget QAbstractItemView:disabled {
            color: rgb(100, 100, 100);
        }

        QCalendarWidget QAbstractItemView:selected {
            background: rgb(80, 80, 80);
        }
    )";

    CustomCalendar *calendar = new CustomCalendar();
    calendar->setStyle(QStyleFactory::create("Fusion"));
    calendar->setStyleSheet(calendarStyle);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->verticalLayout_4->addWidget(calendar);
}

add_task::~add_task()
{
    delete ui;
}

void add_task::on_pushButton_add_clicked()
{
    this->name = ui->lineEdit_name->text();
    this->desc = ui->textEdit_desc->toPlainText();

    QLayoutItem *item = ui->verticalLayout_4->itemAt(1);
    if (item) {
        QWidget *widget = item->widget();
        if (widget) {
            QCalendarWidget *calendar = qobject_cast<QCalendarWidget *>(widget);
            if (calendar) {
                this->date = calendar->selectedDate();
            }
        }
    }

    this->hour = ui->spinBox_hour->value();
    this->min = ui->spinBox_min->value();
    if (ui->checkBox_notif->checkState() == Qt::Checked){
        this->notif = "1";
    } else {
        this->notif = "0";
    }
    QDateTime dateTime(date, QTime(hour, min));
    this->dateTimeString = dateTime.toString(Qt::ISODate);

    Database db;
    db.openDatabase("database.db");
    db.insertTask(name, desc, dateTimeString, notif);
    db.close_conn();
    this->close();
}

void add_task::fill_interface()
{
    QDateTime dateTime = QDateTime::fromString(dateTimeString, Qt::ISODate);

    QLayoutItem *item = ui->verticalLayout_4->itemAt(1);
    if (item) {
        QWidget *widget = item->widget();
        if (widget) {
            QCalendarWidget *calendar = qobject_cast<QCalendarWidget *>(widget);
            if (calendar) {
                calendar->setSelectedDate(dateTime.date());
            }
        }
    }

    ui->lineEdit_name->setText(name);
    ui->textEdit_desc->setPlainText(desc);
    if (notif == "1"){
        ui->checkBox_notif->setCheckState(Qt::Checked);
    } else {
        ui->checkBox_notif->setCheckState(Qt::Unchecked);
    }

    ui->spinBox_hour->setValue(dateTime.time().hour());
    ui->spinBox_min->setValue(dateTime.time().minute());
    set_spinboxes(dateTime.time().hour(), dateTime.time().minute());
}

void add_task::set_spinboxes(int c_hour,
                             int c_min)
{
    if (c_hour < 23){
        ui->pushButton_hour_down->setText(QString::number(c_hour + 1));
    } else {
        ui->pushButton_hour_down->setText(QString::number(0));
    }

    if (c_hour != 0){
        ui->pushButton_hour_up->setText(QString::number(c_hour - 1));
    } else {
        ui->pushButton_hour_up->setText(QString::number(23));
    }

    if (c_min < 59){
        ui->pushButton_min_down->setText(QString::number(c_min + 1));
    } else {
        ui->pushButton_min_down->setText(QString::number(0));
    }

    if (c_min != 0){
        ui->pushButton_min_up->setText(QString::number(c_min - 1));
    } else {
        ui->pushButton_min_up->setText(QString::number(59));
    }
}

void add_task::on_spinBox_hour_valueChanged(int arg1)
{
    if (arg1 < 23){
        ui->pushButton_hour_down->setText(QString::number(arg1 + 1));
    } else {
        ui->pushButton_hour_down->setText(QString::number(0));
    }

    if (arg1 != 0){
        ui->pushButton_hour_up->setText(QString::number(arg1 - 1));
    } else {
        ui->pushButton_hour_up->setText(QString::number(23));
    }
}


void add_task::on_spinBox_min_valueChanged(int arg1)
{
    if (arg1 < 59){
        ui->pushButton_min_down->setText(QString::number(arg1 + 1));
    } else {
        ui->pushButton_min_down->setText(QString::number(0));
    }

    if (arg1 != 0){
        ui->pushButton_min_up->setText(QString::number(arg1 - 1));
    } else {
        ui->pushButton_min_up->setText(QString::number(59));
    }
}


void add_task::on_pushButton_hour_down_clicked()
{
    ui->spinBox_hour->setValue(ui->pushButton_hour_down->text().toInt());

    if (ui->spinBox_hour->value() < 23){
        ui->pushButton_hour_down->setText(QString::number(ui->spinBox_hour->value() + 1));
    } else {
        ui->pushButton_hour_down->setText(QString::number(0));
    }

    if (ui->spinBox_hour->value() != 0){
        ui->pushButton_hour_up->setText(QString::number(ui->spinBox_hour->value() - 1));
    } else {
        ui->pushButton_hour_up->setText(QString::number(23));
    }
}


void add_task::on_pushButton_hour_up_clicked()
{
    ui->spinBox_hour->setValue(ui->pushButton_hour_up->text().toInt());

    if (ui->spinBox_hour->value() < 23){
        ui->pushButton_hour_down->setText(QString::number(ui->spinBox_hour->value() + 1));
    } else {
        ui->pushButton_hour_down->setText(QString::number(0));
    }

    if (ui->spinBox_hour->value() != 0){
        ui->pushButton_hour_up->setText(QString::number(ui->spinBox_hour->value() - 1));
    } else {
        ui->pushButton_hour_up->setText(QString::number(23));
    }
}


void add_task::on_pushButton_min_down_clicked()
{
    ui->spinBox_min->setValue(ui->pushButton_min_down->text().toInt());

    if (ui->spinBox_min->value() < 59){
        ui->pushButton_min_down->setText(QString::number(ui->spinBox_min->value() + 1));
    } else {
        ui->pushButton_min_down->setText(QString::number(0));
    }

    if (ui->spinBox_min->value() != 0){
        ui->pushButton_min_up->setText(QString::number(ui->spinBox_min->value() - 1));
    } else {
        ui->pushButton_min_up->setText(QString::number(59));
    }
}


void add_task::on_pushButton_min_up_clicked()
{
    ui->spinBox_min->setValue(ui->pushButton_min_up->text().toInt());

    if (ui->spinBox_min->value() < 59){
        ui->pushButton_min_down->setText(QString::number(ui->spinBox_min->value() + 1));
    } else {
        ui->pushButton_min_down->setText(QString::number(0));
    }

    if (ui->spinBox_min->value() != 0){
        ui->pushButton_min_up->setText(QString::number(ui->spinBox_min->value() - 1));
    } else {
        ui->pushButton_min_up->setText(QString::number(59));
    }
}


void add_task::on_pushButton_del_clicked()
{
    Database db;
    db.openDatabase("database.db");
    db.deleteTask(id);
    db.close_conn();
    this->close();
}


void add_task::on_pushButton_edit_clicked()
{
    this->name = ui->lineEdit_name->text();
    this->desc = ui->textEdit_desc->toPlainText();
    QLayoutItem *item = ui->verticalLayout_4->itemAt(1);
    if (item) {
        QWidget *widget = item->widget();
        if (widget) {
            QCalendarWidget *calendar = qobject_cast<QCalendarWidget *>(widget);
            if (calendar) {
                this->date = calendar->selectedDate();
            }
        }
    }
    this->hour = ui->spinBox_hour->value();
    this->min = ui->spinBox_min->value();
    if (ui->checkBox_notif->checkState() == Qt::Checked){
        this->notif = "1";
    } else {
        this->notif = "0";
    }
    QDateTime dateTime(date, QTime(hour, min));
    this->dateTimeString = dateTime.toString(Qt::ISODate);

    Database db;
    db.openDatabase("database.db");
    db.updateTask(id, name, desc, dateTimeString, notif);
    db.close_conn();
    this->close();
}

