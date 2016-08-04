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
    ui->textEdit->setReadOnly(true);

    QFont font = this->font();
    font.setPointSize(14);
    ui->textEdit->setFont(font);
    connect(ui->textEdit,SIGNAL(textChanged()),
            this,SLOT(textChangedSlot()));
    connect(this,SIGNAL(EV_callBackSignal(quint8,const void*)),
            this,SLOT(EV_callBackSlot(quint8,const void*)),Qt::QueuedConnection);
    isAllColumnTest = false;
    isAllBoxOpen = false;
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
        int res = trade->result;

        if(tradeHash.contains(res)){
             quint32 temp = tradeHash.value(res);
             temp++;
             tradeHash[res] = temp;
        }
        else{
            tradeHash[res] = 1;
        }
        str += QString("出货%1次\n").arg(++tradeNum);
        QHash<int, quint32>::const_iterator i;
        for(i = tradeHash.constBegin();i != tradeHash.constEnd();i++){
            str += QString("[%1]:%2\n").arg(i.key()).arg(i.value());
        }


        ui->label_remainAmount->setText(QString("%1").arg(trade->remainAmount));
        if(isAllColumnTest){
            on_pushButton_trade_clicked();
        }

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





void MainWindow::textChangedSlot()
{
    ui->textEdit->moveCursor(QTextCursor::End);
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

void MainWindow::on_pushButton_cycleTrade_clicked(bool checked)
{
    if(checked){
        ui->pushButton_cycleTrade->setText("停止检测");
        on_pushButton_trade_clicked();
        isAllColumnTest = true;
        tradeHash.clear();
        tradeNum = 0;
    }
    else{
        ui->pushButton_cycleTrade->setText("循环出货");
        isAllColumnTest = false;
    }
}

void MainWindow::on_pushButton_trade_2_clicked()
{
    typedef int (*EV_bentoOpen)(int cabinet, int box);
    EV_bentoOpen bentoOpen = (EV_bentoOpen)lib.resolve("EV_bentoOpen");
    if(bentoOpen)
    {
        bool ok;
        quint8 cabinet = ui->lineEdit_bentoCabinet->text().toInt(&ok);
        quint8 box = ui->lineEdit_trade_column_2->text().toInt(&ok);
        bentoOpen(cabinet,box);
    }

}

void MainWindow::on_pushButton_bentoPort_clicked(bool checked)
{
    if(checked){
        typedef int (*EV_bentoRegister)(char *);

        EV_bentoRegister bentoRegister = (EV_bentoRegister)lib.resolve("EV_bentoRegister");
        if(bentoRegister)
        {
            QByteArray portArr = ui->lineEdit_port_2->text().toUtf8();
            int ret = bentoRegister(portArr.data());
            if(ret == -1)
            {
                ui->pushButton_bentoPort->setChecked(false);
                QMessageBox::warning(this,tr("COM"),tr("Open serialport fail!"),QMessageBox::Yes);
            }
            ui->pushButton_bentoPort->setText(tr("关闭串口"));
            return;
        }
        ui->pushButton_bentoPort->setChecked(false);
        QMessageBox::warning(this,tr("COM"),tr("Load dll failed!"),QMessageBox::Yes);
    }
    else{
        typedef int (*EV_bentoRelease)(void);
        EV_bentoRelease bentoRelease = (EV_bentoRelease)lib.resolve("EV_bentoRelease");
        if(bentoRelease){
            bentoRelease();
        }
        curBoxNum = 200;
        ui->pushButton_bentoPort->setText(tr("打开串口"));
    }
}

void MainWindow::on_pushButton_bentoCheck_clicked()
{
    typedef int (*EV_bentoCheck)(int,ST_COLUMN_RPT *);
    EV_bentoCheck bentoCheck = (EV_bentoCheck)lib.resolve("EV_bentoCheck");
    if(bentoCheck){
        bool ok;
        quint8 cabinet = ui->lineEdit_bentoCabinet->text().toInt(&ok);
        ST_COLUMN_RPT st_bento;
        int ret = bentoCheck(cabinet,&st_bento);

        if(ret == 1){
            ui->lineEdit_bentoBoxNum->setText(QString("%1").arg(st_bento.sum));
            boxSum = st_bento.sum;
        }
        else{
            boxSum = 0;
        }

        qDebug()<<"EV_bentoCheck:"<<st_bento.sum<<st_bento.id;

    }

}

void MainWindow::on_radioButton_bentoLightOn_clicked(bool checked)
{

    typedef int (*EV_bentoLight)(int cabinet, int flag);
    EV_bentoLight bentoLight = (EV_bentoLight)lib.resolve("EV_bentoLight");
    if(bentoLight == NULL) return;
    bool ok;
    quint8 cabinet = ui->lineEdit_bentoCabinet->text().toInt(&ok);

    qDebug()<<"BentoLight:"<<checked;
    if(checked){
        bentoLight(cabinet,1);
    }
    else{
        bentoLight(cabinet,0);
    }
}

void MainWindow::on_radioButton_bentoLightOff_clicked(bool checked)
{
    typedef int (*EV_bentoLight)(int cabinet, int flag);
    EV_bentoLight bentoLight = (EV_bentoLight)lib.resolve("EV_bentoLight");
    if(bentoLight == NULL) return;
    bool ok;
    quint8 cabinet = ui->lineEdit_bentoCabinet->text().toInt(&ok);
    if(checked){
        bentoLight(cabinet,0);
    }

}

void MainWindow::on_pushButton_allboxopen_clicked()
{

    on_pushButton_bentoCheck_clicked();


    isAllBoxOpen = true;
    curBoxNum = 1;
    startTimer(100);
}


void MainWindow::timerEvent(QTimerEvent *e)
{
    qDebug()<<"timerEvent:"<<e->timerId()<<" curBox"<<curBoxNum<<" boxSum:"<<boxSum;
    killTimer(e->timerId());
    if(curBoxNum < (boxSum + 1)){
        typedef int (*EV_bentoOpen)(int cabinet, int box);
        EV_bentoOpen bentoOpen = (EV_bentoOpen)lib.resolve("EV_bentoOpen");
        if(bentoOpen)
        {
            bool ok;
            quint8 cabinet = ui->lineEdit_bentoCabinet->text().toInt(&ok);
            bentoOpen(cabinet,curBoxNum);
        }
        curBoxNum++;
        startTimer(10);
    }
    else{
    }
}
