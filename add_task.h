#ifndef ADD_TASK_H
#define ADD_TASK_H

#include <QDialog>
#include <QDate>
#include <QStyleFactory>
#include <QCalendarWidget>
#include <QPainter>

namespace Ui {
class add_task;
}

class add_task : public QDialog
{
    Q_OBJECT

public:
    explicit add_task(QWidget *parent = nullptr, bool edit_mode = true);
    ~add_task();

    bool is_edit;
    int id;
    QString name;
    QString desc;
    QDate date;
    int hour;
    int min;
    QString notif;
    QString dateTimeString;

    void fill_interface();
    void set_spinboxes(int c_hour,
                       int c_min);

private slots:
    void on_pushButton_add_clicked();

    void on_spinBox_hour_valueChanged(int arg1);

    void on_spinBox_min_valueChanged(int arg1);

    void on_pushButton_hour_down_clicked();

    void on_pushButton_hour_up_clicked();

    void on_pushButton_min_down_clicked();

    void on_pushButton_min_up_clicked();

    void on_pushButton_del_clicked();

    void on_pushButton_edit_clicked();

private:
    Ui::add_task *ui;
};

class CustomCalendar : public QCalendarWidget {
public:
    using QCalendarWidget::QCalendarWidget;

    CustomCalendar(QWidget *parent = nullptr) : QCalendarWidget(parent) {
        setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
        setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
        setGridVisible(false);
    }

protected:
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const override {
        if (date.month() == monthShown() && date.year() == yearShown()) {
            QCalendarWidget::paintCell(painter, rect, date);
            if (date == QDate::currentDate()) {
                painter->save();
                QPen pen(QColor(100, 150, 255), 2);
                painter->setPen(pen);
                painter->drawRect(rect.adjusted(1, 1, -1, -1));
                painter->restore();
            }
        }
        else {
            painter->fillRect(rect, palette().window());
        }
    }
};

#endif // ADD_TASK_H
