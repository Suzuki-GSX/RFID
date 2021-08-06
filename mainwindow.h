#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ui_MainWindow.h"
#include <stdio.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_Read_clicked();
    void on_Add_clicked();
    void on_Delete_clicked();
    void on_Change_clicked();
    void on_Recover_clicked();
    void on_Out_clicked();
    void data_show(int id_recv);

    /*串口*****/
        void serialPortReadData();//串口数据接收
        void on_SendBtn_clicked();//数据发送
        void on_ReceiveHexCheckBox_stateChanged(int arg1);//接收数据显示框十六进制显示改变
        void on_OpenBtn_clicked();//打开串口
        void on_BaudComboBox_currentIndexChanged(const QString &arg1);//波特率改变
        void on_StopBitComboBox_currentIndexChanged(int index);//停止位改变
        void on_DataBitComboBox_currentIndexChanged(int index);//数据位改变
        void on_CheckComboBox_currentIndexChanged(int index);//校验位改变
        void on_RefreshBtn_clicked();//刷新
        void on_ReceiveClearBtn_clicked();//清除接收
    /*串口*****/


        void on_SendHexCheckBox_stateChanged(int arg1);

private:
        Ui::MainWindow *ui;
        QTimer *timer;

        /*串口*****/
            QSerialPort *serialPort;
            void SerialPortInit();//串口初始化
            void SerialPortParametersSetting(bool set);//串口各参数ComBox使能设置
            void SerialPortError(QSerialPort::SerialPortError error);//错误槽函数
        /*串口*****/


        QString addText;
        int wt;
        int wxx;
        int hyk;






};
#endif // MAINWINDOW_H
