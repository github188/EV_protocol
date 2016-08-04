#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLibrary>
#include "EVprotocol.h"
#include <QHash>
#include <QTimerEvent>

namespace Ui {
class MainWindow;
}




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void EV_fun(int type,const void *ptr);



protected:
    void timerEvent(QTimerEvent *);

signals:
    void EV_callBackSignal(const quint8 type,const void *ptr);
private slots:
    void on_pushButton_start_clicked();

    void on_pushButton_trade_clicked();

    void on_pushButton_stop_clicked();

    void EV_callBackSlot(const quint8 type,const void *ptr);
    void on_pushButton_payout_clicked();

    void on_pushButton_cycleTrade_clicked(bool checked);
    void textChangedSlot();
    void on_pushButton_trade_2_clicked();

    void on_pushButton_bentoPort_clicked(bool checked);

    void on_pushButton_bentoCheck_clicked();

    void on_radioButton_bentoLightOn_clicked(bool checked);

    void on_radioButton_bentoLightOff_clicked(bool checked);

    void on_pushButton_allboxopen_clicked();

private:
    Ui::MainWindow *ui;
    QLibrary lib;

    typedef void  (__stdcall *EV_CALLBACK_HANDLE)(int,const void *);
    static void  __stdcall EV_callBack(int type,const void *ptr);


    bool isAllColumnTest;
    QHash<int,quint32> tradeHash;
    quint32 tradeNum;


    bool isAllBoxOpen;
    quint32 curBoxNum;
    quint32 boxSum;
    int timerId;
};

#endif // MAINWINDOW_H
