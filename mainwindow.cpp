#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QDate>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set lineEdit
    QRegExp regxChangdi("[1-9][0-9]$");
    QValidator *validChangdi = new QRegExpValidator(regxChangdi, ui->lineEditChangdi);
    ui->lineEditChangdi->setValidator(validChangdi);


    QRegExp regxReceipt("^[A-Ba-b][0-9][0-9][0-9][0-9]$");
    QValidator *validReceipt = new QRegExpValidator(regxReceipt, ui->lineEditReceipt);
    ui->lineEditReceipt->setValidator(validReceipt);

    database = QSqlDatabase::addDatabase("QSQLITE");
    QString dbpath;
#ifdef Q_OS_MAC
QFileInfo macPath("/Volumes/xiaozu/database/");
if (macPath.isDir()) {
    dbpath = QString("%1/xiaozu.db").arg(macPath.path());
} else {
    dbpath = "/tmp/xiaozu.db";
}
#else
dbpath = "xiaozu.db";
#endif

    database.setDatabaseName(dbpath);
    if (!database.open()) {
        qDebug() << "Can not open xiaozu.db";
        QMessageBox::information(this, "", "无法连接数据库");
    } else {
        QSqlQuery query;
        QString createTableSql = QString("Create table xiaozu ("
                                         " logdate varchar(32),"
                                         " receipt varchar(32), "
                                         " changdi varchar(32)"
                                         ");");
        query.exec(createTableSql);
        qDebug() << createTableSql;
        database.commit();
        setModel();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSave_clicked()
{
    QString receipt = ui->lineEditReceipt->text().toUpper();
    QString changdi = ui->lineEditChangdi->text().trimmed();

    if (receipt.isEmpty() || changdi.isEmpty()) {
        QMessageBox::information(this, "", "收据号和场地不能为空");
        return;
    }

    if (receipt.length() != 5) {
        QMessageBox::information(this, "", "收据号的长度应该是5，如: A0001 或者 B0001");
        ui->lineEditReceipt->setFocus();
        return;
    }

    ui->lineEditReceipt->clear();
    ui->lineEditChangdi->clear();

    QString logdate = QDate::currentDate().toString("yyyy-MM-dd");
    QSqlQuery query;
    QString insert = QString("insert into xiaozu (logdate, receipt, changdi) "
                             " values ('%1', '%2', '%3');"
                             ).arg(logdate).arg(receipt).arg(changdi);
    query.exec(insert);
    qDebug() << insert;
    qDebug() << query.lastError().text();
    database.commit();

    model->select();
    ui->tableView->reset();
    ui->lineEditReceipt->setFocus();
}

void MainWindow::on_pushButtonExport_clicked()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, "打开保存文件路径", "", "txt (*.txt)");
    qDebug() << fileName;
    QFile data(fileName);
    if (!data.open(QFile::WriteOnly| QIODevice::Truncate)) {
        QMessageBox::information(this, "", "打开文件错误");
        return;
    }

    QTextStream out(&data);
    model->select();
    for(int i = 0; i < model->rowCount(); ++i) {
        QString logdate = model->record(i).value("logdate").toString();
        QString receipt = model->record(i).value("receipt").toString();
        QString changdi = model->record(i).value("changdi").toString();
        out << logdate << "," << receipt << "," << changdi << "\n";
    }

    data.close();
}

int MainWindow::setModel()
{
    model = new QSqlTableModel(this);
    model->setTable("xiaozu");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setSort(0, Qt::AscendingOrder);
    model->setHeaderData(0, Qt::Horizontal, "日期");
    model->setHeaderData(1, Qt::Horizontal, "收据号");
    model->setHeaderData(2, Qt::Horizontal, "场地编号");
    ui->tableView->setModel(model);
    ui->tableView->alternatingRowColors();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    model->select();
    ui->tableView->reset();
    return true;
}

void MainWindow::on_pushButton_clicked()
{
    QSqlQuery query;
    QString createTableSql = QString("delete from xiaozu");
    query.exec(createTableSql);
    qDebug() << createTableSql;
    database.commit();

    model->select();
    ui->tableView->reset();
}
