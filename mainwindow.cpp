#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont font=this->font();
    font.setPixelSize(13);
    font.setFamily(QStringLiteral("宋体"));
    this->setFont(font);//设置字体

    SerialPortInit();//串口初始化
    timer=new QTimer(this);
    timer->setSingleShot(true);
    this->setWindowTitle(tr("QT+RFID"));

    wt=1;
    wxx=0;
    hyk=0;

    QSqlDatabase db=createConnection();

    connect(ui->pushButton_read,SIGNAL(clicked()),this,SLOT(on_Read_clicked()));
    connect(ui->pushButton_add,SIGNAL(clicked()),this,SLOT(on_Add_clicked()));
    connect(ui->pushButton_delete,SIGNAL(clicked()),this,SLOT(on_Delete_clicked()));
    connect(ui->pushButton_change,SIGNAL(clicked()),this,SLOT(on_Change_clicked()));
    connect(ui->pushButton_recover,SIGNAL(clicked()),this,SLOT(on_Recover_clicked()));
    connect(ui->pushButton_out,SIGNAL(clicked()),this,SLOT(on_Out_clicked()));

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Read_clicked()
{
    if(serialPort->isOpen())
        serialPort->write("card\r\n");
    if(!serialPort->isOpen())
    {
        QMessageBox::warning(this,QStringLiteral("串口未打开"),
                             QStringLiteral("数据发送失败，请先打开串口再试"));
        return;
    }
}
/***************扣款、充值**************/

void MainWindow::on_Add_clicked()
{
    QString text=ui->lineEdit_money->text();//输入的数
    QString money=ui->label_money->text();//余额
    QString state=ui->label_state->text();//状态
    QString str;

    if(text.isEmpty())
    {
        QMessageBox::information(this,tr("null"),
                                 QStringLiteral("输入金额"));
        return ;
    }
    if(state ==QString::fromLocal8Bit("lost"))
    {
        QMessageBox::information(this,tr("null"),
                                 QStringLiteral("此卡已挂失"));
        return ;
    }
    else
    {
        int a=text.toInt();
        int b=money.toInt();
        a+=b;

        QSqlQuery query;
        QString select_sql;
        int id;

        QString update_sql = "update student set money = :money where id = :id";
        query.prepare(update_sql);

        if(wt==1)
            id=1032;
        else if(wxx==1)
            id=1033;
        else if(hyk==1)
            id=1133;

        query.bindValue(":money", a);
        query.bindValue(":id", id);
        if(!query.exec())
        {
            qDebug() << query.lastError();
        }
        else
        {
            qDebug() << "updated!";
        }

    }
}

void MainWindow::on_Delete_clicked()
{
    QString text=ui->lineEdit_money->text();//输入的数
    QString money=ui->label_money->text();//余额
    QString state=ui->label_state->text();//状态
    QString str;

    if(text.isEmpty())
    {
        QMessageBox::information(this,tr("null"),
                                 QStringLiteral("输入金额"));
        return ;
    }
    if(state ==QString::fromLocal8Bit("lost"))
    {
        QMessageBox::information(this,tr("null"),
                                 QStringLiteral("此卡已挂失"));
        return ;
    }
    else
    {
        int a=text.toInt();
        int b=money.toInt();
        a=b-a;

        QSqlQuery query;
        QString select_sql;
        int id;

        QString update_sql = "update student set money = :money where id = :id";
        query.prepare(update_sql);

        if(wt==1)
            id=1032;
        else if(wxx==1)
            id=1033;
        else if(hyk==1)
            id=1133;

        query.bindValue(":money", a);
        query.bindValue(":id", id);
        if(!query.exec())
        {
            qDebug() << query.lastError();
        }
        else
        {
            qDebug() << "updated!";
        }

    }
}

/***************扣款、充值**************/

void MainWindow::on_Change_clicked()
{
    QSqlQuery query;
    QString select_sql;
    int id;

    QString update_sql = "update student set state = :state where id = :id";
    query.prepare(update_sql);

    if(wt==1)
        id=1032;
    else if(wxx==1)
        id=1033;
    else if(hyk==1)
        id=1133;

    query.bindValue(":state", "lost");
    query.bindValue(":id", id);
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "updated!";
    }
}



void MainWindow::on_Recover_clicked()
{
    QSqlQuery query;
    QString select_sql;
    int id;

    QString update_sql = "update student set state = :state where id = :id";
    query.prepare(update_sql);

    if(wt==1)
        id=1032;
    else if(wxx==1)
        id=1033;
    else if(hyk==1)
        id=1133;

    query.bindValue(":state", "normal");
    query.bindValue(":id", id);
    if(!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "updated!";
    }
}

void MainWindow::on_Out_clicked()
{
    wt=0;
    wxx=0;
    hyk=0;

}


void MainWindow::data_show(int id_recv)
{
    QSqlQuery query;
    QString select_sql;
    if(id_recv==1032)
    {
        wt=1;
        qDebug() << "1032 ok ";
        select_sql = "select id, name, money, state from student where id=1032;";
    }
    else if(id_recv==1033)
    {
        wxx=1;
        select_sql = "select id, name, money, state from student where id=1033;";
    }
    else if(id_recv==1113)
    {
        hyk=1;
        select_sql = "select id, name, money, state from student where id=1113;";
    }

    if(!query.exec(select_sql))
    {
        qDebug()<<query.lastError();
    }
    else
    {
        while(query.next())
        {
            qDebug() << "shuju ok ";
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            int money = query.value(2).toInt();
            QString state = query.value(3).toString();
            ui->label_num->setText(QString("%2").arg(id));
            ui->label_name->setText(QString("%2").arg(name));
            ui->label_money->setText(QString("%2").arg(money));
            ui->label_state->setText(QString("%2").arg(state));
        }
        qDebug() << "else ok ";
    }

}


