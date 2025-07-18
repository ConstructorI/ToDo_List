#include "mainwindow.h"
#include "add_task.h"
#include "database.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setStyle(QStyleFactory::create("Fusion"));
    this->setWindowIcon(QIcon("icon.ico"));
    update_table();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(this->windowIcon());
    trayIcon->setToolTip("ToDo List");
    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction("Развернуть", this, &MainWindow::showNormal);
    trayMenu->addAction("Выход", qApp, &QApplication::quit);
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);
    trayIcon->show();

    ui->tableView->setStyleSheet("QTableView::item:focus { border: 0px; }");
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setItemDelegateForColumn(1, new DateTimeDelegate(this));

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(60000);
    connect(timer, &QTimer::timeout, [&]() {
        for (int x = 0; x < time_list.size(); x++){
            QDateTime dateTime = QDateTime::fromString(time_list.at(x).at(2), Qt::ISODate);
            if (dateTime.date() == QDate::currentDate() && time_list.at(x).at(3) == "1"){
                if (QTime::currentTime().hour() == dateTime.time().hour() &&
                    QTime::currentTime().minute() == dateTime.time().minute()) {
                    trayIcon->showMessage(time_list.at(x).at(0),
                                          time_list.at(x).at(1),
                                          QSystemTrayIcon::Information, 3000);
                }
            }
        }
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    add_task ui_add_task(nullptr, false);
    ui_add_task.setModal(true);
    ui_add_task.exec();
    update_table();
}

void MainWindow::update_table(){
    Database db;
    db.openDatabase("database.db");

    sqlmodel = new QSqlQueryModel();
    QSqlQuery query(db.db_r());
    query.prepare("SELECT * FROM tasks");
    if (!query.exec()) {
        delete sqlmodel;
    }
    sqlmodel->setQuery(query);
    while (sqlmodel->canFetchMore()) {
        sqlmodel->fetchMore();
    }

    model = new QStandardItemModel(this);
    QStringList headers;
    for(int i = 0; i < sqlmodel->columnCount(); ++i) {
        if (i == 0 || i == 2 || i == 4){
            continue;
        } else {
            headers << sqlmodel->headerData(i, Qt::Horizontal).toString();
        }
    }
    model->setHorizontalHeaderLabels(headers);
    for(int row = 0; row < sqlmodel->rowCount(); ++row) {
        QList<QStandardItem*> items;
        for(int col = 0; col < sqlmodel->columnCount(); ++col) {
            if (col == 0 || col == 2 || col == 4){
                continue;
            } else {
                QStandardItem* item = new QStandardItem(
                    sqlmodel->data(sqlmodel->index(row, col)).toString()
                    );
                items.append(item);
            }
        }
        model->appendRow(items);
    }

    ui->tableView->setModel(model);
    ui->tableView->show();
    model->setHorizontalHeaderLabels({"Задача", "Срок"});

    time_list = db.get_data_times();
    db.close_conn();
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    int id = sqlmodel->data(sqlmodel->index(row, 0)).toInt();
    QString name = sqlmodel->data(sqlmodel->index(row, 1)).toString();
    QString desc = sqlmodel->data(sqlmodel->index(row, 2)).toString();
    QString date = sqlmodel->data(sqlmodel->index(row, 3)).toString();
    QString notif = sqlmodel->data(sqlmodel->index(row, 4)).toString();

    add_task ui_add_task(nullptr, true);
    ui_add_task.id = id;
    qDebug() << id;
    ui_add_task.name = name;
    ui_add_task.desc = desc;
    ui_add_task.dateTimeString = date;
    ui_add_task.notif = notif;
    ui_add_task.fill_interface();
    ui_add_task.setModal(true);
    ui_add_task.exec();
    update_table();
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    QLayoutItem *item;
    while ((item = ui->verticalLayout_3->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    int row = index.row();
    QString name = sqlmodel->data(sqlmodel->index(row, 1)).toString();
    QString desc = sqlmodel->data(sqlmodel->index(row, 2)).toString();
    QString date = sqlmodel->data(sqlmodel->index(row, 3)).toString();
    QDateTime dateTime = QDateTime::fromString(date, Qt::ISODate);
    date = dateTime.toString("dd.MM.yyyy HH:mm");

    QString notif;
    if (sqlmodel->data(sqlmodel->index(row, 4)).toString() == "1") {
        notif = "🔔";
    } else {
        notif = "";
    }

    QLabel *name_lbl = new QLabel();
    QTextEdit *desc_TE = new QTextEdit();
    QLabel *date_lbl = new QLabel();
    name_lbl->setText(name);
    name_lbl->setAlignment(Qt::AlignHCenter);
    desc_TE->setReadOnly(true);
    desc_TE->document()->setDocumentMargin(0);
    desc_TE->setText(desc);
    desc_TE->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    desc_TE->setStyleSheet("QTextEdit { border: 0px; }");
    desc_TE->setMaximumWidth(280);
    date_lbl->setText(date + " " + notif);
    ui->verticalLayout_3->insertWidget(0, name_lbl);
    ui->verticalLayout_3->insertWidget(1, date_lbl);
    ui->verticalLayout_3->insertWidget(2, desc_TE);
}
