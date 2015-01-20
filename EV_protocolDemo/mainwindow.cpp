#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QLibrary>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLibrary lib("EVprotocol");
    if(lib.load())
    {
        qDebug()<<"Load OK..";
    }

#ifdef _WIN32
    ui->lineEdit_port->setText("COM1");
#else
    ui->lineEdit_port->setText("ttyS0");
#endif


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_start_clicked()
{

}
