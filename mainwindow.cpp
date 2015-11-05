#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 场地编号
    QStringList changdiStringList;
    for(int i = 0; i <= 42; ++i) {
        changdiStringList.append(QString("%1").arg(i));
    }
    ui->comboBoxChangdi->addItems(changdiStringList);

    // 中文或者外文
    QStringList langStringList;
    langStringList.append(QString("中文"));
    langStringList.append(QString("外文"));
    ui->comboBoxLang->addItems(langStringList);

    // 山上、山下、京外
    QStringList placeStringList;
    placeStringList.append(QString("山下"));
    placeStringList.append(QString("山上"));
    placeStringList.append(QString("京外"));
    ui->comboBoxPlace->addItems(placeStringList);

    database = QSqlDatabase::addDatabase("QSQLITE");
    QString dbpath;

#ifdef Q_OS_MAC
dbpath = "/tmp/xiaozu.db";
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
                                         " lang varchar(32), "
                                         " place varchar(32), "
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
    if (receipt.isEmpty()) {
        qDebug() << "receipt should not be empty";
        QMessageBox::information(this, "", "收据号不能为空");
        return;
    }

    QString changdi = ui->comboBoxChangdi->currentText();
    QString lang = ui->comboBoxLang->currentText();
    QString place = ui->comboBoxPlace->currentText();
    qDebug() << receipt << changdi << lang << place;
    ui->lineEditReceipt->clear();
    QString logdate = QDate::currentDate().toString("yyyy-MM-dd");

    QSqlQuery query;
    QString insert = QString("insert into xiaozu (logdate, receipt, lang, place, changdi) "
                             " values ('%1', '%2', '%3', '%4', '%5');"
                             ).arg(logdate).arg(receipt).arg(lang).arg(place).arg(changdi);
    query.exec(insert);
    qDebug() << insert;
    qDebug() << query.lastError().text();
    database.commit();

    model->select();
    ui->tableView->reset();
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
        QString lang = model->record(i).value("lang").toString();
        QString place = model->record(i).value("place").toString();
        QString changdi = model->record(i).value("changdi").toString();
        out << logdate << "," << receipt << "," << lang << "," << place << "," << changdi << "\n";
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
    model->setHeaderData(2, Qt::Horizontal, "语言");
    model->setHeaderData(3, Qt::Horizontal, "山上、山下或京外");
    model->setHeaderData(4, Qt::Horizontal, "场地编号");
    ui->tableView->setModel(model);
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
