#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLibrary>

namespace Ui {
class MainWindow;
}

#define EV_NA               0x00
#define EV_SETUP_REQ        0x90 //初始化请求
#define EV_SETUP_RPT 		0x05//初始化结果返回
#define EV_INFO_REQ 		0x8C
#define EV_INFO_RPT 		0x11
#define EV_ACK_PC           0x80

#define EV_NAK_PC           0x81//PC回应NAK
#define EV_ACK_VM           0x01
#define EV_NAK_VM           0x02
#define EV_POLL             0x03
#define EV_PAYIN_RPT        0x06//投币报告
#define EV_COLUMN_REQ       0x8A//获取货道
#define EV_COLUMN_RPT       0x0E//货道报告
#define EV_TRADE_REQ        0x83
#define EV_TRADE_RPT        0x08

#define EV_ACTION_RPT       0x0B
#define EV_STATE_REQ        0x86
#define EV_STATE_RPT        0x0D
#define EV_BUTTON_RPT       0x0C
#define EV_CONTROL_REQ      0x85
#define EV_PAYOUT_REQ       0x89
#define EV_PAYOUT_RPT       0x07
#define EV_CONTROL_RPT      0xA0
#define EV_ACTION_REQ       0xA1
#define EV_ENTER_MANTAIN    0xA2
#define EV_EXIT_MANTAIN     0xA3

#define EV_INITING          0xA4
#define EV_RESTART          0xA5
#define EV_OFFLINE          0xA6
#define EV_ONLINE           0xA7
#define EV_TIMEOUT          0xA8
#define EV_FAIL             0xA9
#define EV_EXIT_MANTAIN     0xA3


typedef struct _st_bill_{
    quint8      recvType;
    quint32     maxRecv;
    quint32     recvChannel[8];
    quint8      changeType;
    quint32     changeChannel[8];


}ST_BILL;


typedef struct _st_coin_{
    quint8 recvType;
    quint32  maxRecv;
    quint32  recvChannel[8];
    quint8 changeType;
    quint32  changeChannel[8];


}ST_COIN;


typedef struct _st_card_{
    quint8 type;
}ST_CARD;


typedef struct _st_bin_{
    quint8 type;//0关闭 1弹簧 2老式升降机 3升降机+传送带 4升降机+弹簧
    quint8 addGoods;//补货方式 0手动 1自动
    quint8 goc;//0关闭出货确认  1开启
    quint8 light;//0不支持 1支持
    quint8 hot;//
    quint8 cool;
    quint8 keyboard;
    quint8 compressor;
}ST_BIN;


typedef struct _st_setup_{
    quint8  language;
    quint32 vmRatio;//VMC主控板比例因子 1表示分 10表示角  100表示元
    quint8  payoutTime;//超时退币  单位秒  255 表示不退币
    quint8  multBuy;//多次购买
    quint8  forceBuy;//强制购买
    quint8  humanSensor;//是否支持人体接近感应
    quint8  gameButton;
    ST_BILL bill;
    ST_COIN coin;
    ST_CARD card;
    ST_BIN	bin;
    ST_BIN  subBin;

}ST_SETUP;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_start_clicked();

private:
    Ui::MainWindow *ui;
    QLibrary lib;

    typedef void  (__stdcall *EV_CALLBACK_HANDLE)(int,const void *);
    static void  __stdcall EV_callBack(int type,const void *ptr);

};

#endif // MAINWINDOW_H
