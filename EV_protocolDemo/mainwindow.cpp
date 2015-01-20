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

void  __stdcall MainWindow::EV_callBack(int type, const void *ptr)
{
    if(mainThis){}
    switch(type)
    {
        case EV_SETUP_RPT:
            ST_SETUP *setup = (ST_SETUP *)ptr;
            qDebug()<<"lang:"<<setup->language;

            break;

    }
    qDebug()<<"type="<<type;
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
            QMessageBox::warning(this,tr("COM"),tr("Open fail!"),QMessageBox::Yes);
        }
        return;
    }
}
