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


    connect(this,SIGNAL(EV_callBackSignal(quint8,const void*)),
            this,SLOT(EV_callBackSlot(quint8,const void*)),Qt::QueuedConnection);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::EV_callBackSlot(const quint8 type,const void *ptr)
{
    QString str = "";
    bool ok;
    if(type == EV_INITING)
    {
      str = "EV_INITING\n";
      ui->label_vmcState->setText(tr("EV_INITING"));
    }
    else if(type == EV_OFFLINE)
    {
       str = "EV_OFFLINE\n";
       ui->label_vmcState->setText(tr("EV_OFFLINE"));
    }
    else if(type == EV_SETUP_REQ)
    {
       str = "EV_SETUP_REQ\n";
    }
    else if(type == EV_SETUP_RPT)
    {
       str = "EV_SETUP_RPT\n";
       ST_SETUP *setup = (ST_SETUP *)ptr;
       str =  "EV_SETUP_RPT:";
       str += QString("language:%1 ").arg(setup->language);
       str += QString("multBuy:%1 ").arg(setup->multBuy);
       str += QString("payoutTime:%1 ").arg(setup->payoutTime);
       str += QString("Bill_recv_type:%1 ").arg(setup->bill.recv_type);
       str += "\n";
    }
    else if(type == EV_STATE_RPT)
    {
       ST_STATE *state = (ST_STATE *)ptr;
       str =  "EV_STATE_RPT:";


       if(state->vmcState == EV_STATE_DISCONNECT)
       {
            ui->label_vmcState->setText("EV_STATE_DISCONNECT");
            str += QString("State:%1 ").arg("EV_STATE_DISCONNECT");
       }
       else if(state->vmcState == EV_STATE_NORMAL)
       {
           str += QString("State:%1 ").arg("EV_STATE_NORMAL");
           ui->label_vmcState->setText("EV_STATE_NORMAL");
       }
       else if(state->vmcState == EV_STATE_FAULT)
       {
           str += QString("State:%1 ").arg("EV_STATE_FAULT");
           ui->label_vmcState->setText("EV_STATE_FAULT");
       }
       str += "\n";
    }
    else if(type == EV_ENTER_MANTAIN)
    {
        ui->label_vmcState->setText("EV_ENTER_MANTAIN");
    }
    else if(type == EV_EXIT_MANTAIN)
    {
        ui->label_vmcState->setText("EV_EXIT_MANTAIN");
    }
    else if(type == EV_REQUEST_FAIL)
    {
        ST_PC_REQ *req = (ST_PC_REQ *)ptr;
        str += ("EV_REQUEST_FAIL:");
        str += QString("type:%1,err:%2\n").arg(req->type)
                .arg(req->err);

    }
    else if(type == EV_TRADE_RPT)
    {
        ST_TRADE *trade = (ST_TRADE *)ptr;
        str += ("EV_TRADE_RPT:");
        str += QString("cabinet:%1,column:%2 rst= %3 amount:%4\n").arg(trade->cabinet)
                .arg(trade->column).arg(trade->result).arg(trade->remainAmount);

        ui->label_remainAmount->setText(QString("%1").arg(trade->remainAmount));


    }
    else if(type == EV_PAYIN_RPT)
    {
        ST_PAYIN_RPT *payin = (ST_PAYIN_RPT *)ptr;
        QString strAmount = QString("%1").arg(payin->reamin_amount);
        ui->label_remainAmount->setText(strAmount);
        str += ("EV_PAYIN_RPT:");
        str += QString("type:%1,amount:%2 remain: %3\n").arg(payin->payin_type)
                .arg(payin->payin_amount).arg(payin->reamin_amount);

    }
    else if(type == EV_PAYOUT_RPT)
    {
        ST_PAYOUT_RPT *payout = (ST_PAYOUT_RPT *)ptr;
        QString strAmount = QString("%1").arg(payout->reamin_amount);
        ui->label_remainAmount->setText(strAmount);
        str += ("EV_PAYOUT_RPT:");
        str += QString("type:%1,amount:%2 remain: %3\n").arg(payout->payout_type)
                .arg(payout->payout_amount).arg(payout->reamin_amount);

    }
    else
        str.clear();


    qDebug()<<"type="<<type;
    if(!str.isEmpty())
        ui->textEdit->textCursor().insertText(str);
}

//注意次函数是在子线程调用，不能在这里做UI更新的操作
void MainWindow::EV_fun(int type, const void *ptr)
{
    emit EV_callBackSignal(type,ptr);
}



void  __stdcall MainWindow::EV_callBack(int type, const void *ptr)
{
    if(mainThis)
    {
        mainThis->EV_fun(type,ptr);
    }


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
    bool ok;
    EV_trade trade = (EV_trade)lib.resolve("EV_trade");
    if(trade)
    {

        quint8 column = ui->lineEdit_trade_column->text().toUInt(&ok);
        quint32 cost = ui->lineEdit_trade_cost->text().toUInt(&ok);
        quint8 type = cost == 0  ?  1: 0;

        qDebug()<<"Trade:"<<"column:"<<column<<"cost:"<<cost<<"type:"<<type;
        trade(1,column,type,cost);
    }
}

void MainWindow::on_pushButton_stop_clicked()
{
    typedef void (*EV_vmcStop)(void);
    EV_vmcStop stop = (EV_vmcStop)lib.resolve("EV_vmcStop");
    if(stop)
    {
        stop();
    }
}

void MainWindow::on_pushButton_payout_clicked()
{
    typedef int (*EV_payout)(long);
    EV_payout payout = (EV_payout)lib.resolve("EV_payout");
    if(payout)
    {
        payout(100);
    }
}
