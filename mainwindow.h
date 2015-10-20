#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QLineEdit>
#include <QMenu>
#include <QDebug>
#include <QtSql>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QFile>
#include <QDate>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QSqlDatabase database;
    QSqlTableModel *model;
    int setModel();
    ~MainWindow();

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonExport_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
