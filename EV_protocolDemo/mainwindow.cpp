#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMessageBox>



MainWindow  *mainThis = NULL;//定义全局main指针

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainThis = this;
    lib.setFileName("EVprotocol");
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



void MainWindow::EV_fun(int type, const void *ptr)
{
    switch(type)
    {
        case EV_SETUP_RPT:
            ST_SETUP *setup = (ST_SETUP *)ptr;
            qDebug()<<"lang:"<<setup->language;
            ui->textEdit->insertPlainText("EV_SETUP_RPT");
            break;

    }

    qDebug()<<"type="<<type;

}



void  __stdcall MainWindow::EV_callBack(int type, const void *ptr)
{
    if(mainThis)
        mainThis->EV_fun(type,ptr);

}

void MainWindow::on_pushButton_start_clicked()
{
    typedef int (*EV_vmcStart)(char *,EV_CALLBACK_HANDLE);

    EV_vmcStart vmcStart = (EV_vmcStart)lib.resolve("EV_vmcStart");
    if(vmcStart)
    {
        QByteArray portArr = ui->lineEdit_port->text().toUtf8();
        int ret = vmcStart(portArr.data(),MainWindow::EV_callBack);
        if(ret == -1)
        {
            QMessageBox::warning(this,tr("COM"),tr("Open serialport fail!"),QMessageBox::Yes);
        }
        return;
    }
    QMessageBox::warning(this,tr("COM"),tr("Load dll failed!"),QMessageBox::Yes);
}

void MainWindow::on_pushButton_trade_clicked()
{
    typedef int (*EV_trade)(int ,int,int,int);

    EV_trade trade = (EV_trade)lib.resolve("EV_trade");
    if(trade)
    {
        qDebug()<<"Trade:";
        trade(1,11,1,0);
    }
}
