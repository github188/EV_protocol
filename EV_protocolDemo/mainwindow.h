#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLibrary>
#include "EVprotocol.h"

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



signals:
    void EV_callBackSignal(const quint8 type,const void *ptr);
private slots:
    void on_pushButton_start_clicked();

    void on_pushButton_trade_clicked();

    void on_pushButton_stop_clicked();

    void EV_callBackSlot(const quint8 type,const void *ptr);
    void on_pushButton_payout_clicked();

private:
    Ui::MainWindow *ui;
    QLibrary lib;

    typedef void  (__stdcall *EV_CALLBACK_HANDLE)(int,const void *);
    static void  __stdcall EV_callBack(int type,const void *ptr);





};

#endif // MAINWINDOW_H
