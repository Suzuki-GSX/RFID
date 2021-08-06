#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "database.h"

void MainWindow::SerialPortInit()
{
    static bool state=false;
    if(state) return;
    state=true;//这个函数只调用一次

    int maxLen=0;
    serialPort=new QSerialPort(this);
    ui->PortComboBox->clear();
    QString text;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        text=info.portName()+"("+info.description()+")";
        ui->PortComboBox->addItem(text);
        if(text.length()>maxLen)
            maxLen=text.length();
    }
    ui->PortComboBox->view()->setFixedWidth(maxLen*8);

    connect(serialPort,&QSerialPort::errorOccurred,this,&MainWindow::SerialPortError);

    text=ui->PortComboBox->currentText();
    int index=text.indexOf('(');
    serialPort->setPort(QSerialPortInfo(text.left(index)));//设置端口

    serialPort->setBaudRate(ui->BaudComboBox->currentText().toInt());//设置波特率
    QSerialPort::StopBits stopBit=QSerialPort::OneStop;
    switch(ui->StopBitComboBox->currentIndex())
    {
    case 0:stopBit=QSerialPort::OneStop; break;
    case 1:stopBit=QSerialPort::OneAndHalfStop; break;
    case 2:stopBit=QSerialPort::TwoStop; break;
    }
    serialPort->setStopBits(stopBit);//设置停止位

    QSerialPort::DataBits databit=QSerialPort::Data8;
    switch(ui->DataBitComboBox->currentIndex())
    {
    case 0:databit=QSerialPort::Data8; break;
    case 1:databit=QSerialPort::Data7; break;
    case 2:databit=QSerialPort::Data6; break;
    case 3:databit=QSerialPort::Data5; break;
    }
    serialPort->setDataBits(databit);//设置数据位

    QSerialPort::Parity parity=QSerialPort::NoParity;
    switch(ui->CheckComboBox->currentIndex())
    {
    case 0:parity=QSerialPort::NoParity; break;
    case 1:parity=QSerialPort::OddParity; break;
    case 2:parity=QSerialPort::EvenParity; break;
    case 3:parity=QSerialPort::SpaceParity; break;
    case 4:parity=QSerialPort::MarkParity; break;
    }
    serialPort->setParity(parity);//设置校验位
}

void MainWindow::serialPortReadData()//数据接收
{
    QString str;
    QByteArray data = serialPort->readAll();
    const char *c = data;
    if(data.at(0)=='n')
    {
        int num=(data.at(1)-'0')*1000+(data.at(2)-'0')*100+(data.at(3)-'0')*10+(data.at(4)-'0');
        data_show(num);
        //ui->label_num->setText(QString("%2").arg(num));
    }
    if(ui->ReceiveHexCheckBox->checkState() == Qt::Checked)//十六进制显示
    {
        while(*c)
        {
            str+=QString("%1 ").arg(*c,0,16,QLatin1Char(' '));
            c++;
        }
    }
    else //ASCII
    {
        str = QString::fromUtf8(c);
    }
    ui->ReceiveTextBrowser->insertPlainText(str);
}


void MainWindow::on_SendBtn_clicked()//数据发送
{
    QString str = ui->SendTextEdit->document()->toPlainText();
    if(str.isEmpty()) return;
    bool bFlag = str.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
    if(bFlag)//判断是否有中文
    {
        QMessageBox::warning(this,QStringLiteral("存在中文"),
                             QStringLiteral("存在中文，请规范输入。"));
        return;
    }
    if(ui->SendHexCheckBox->checkState() == Qt::Unchecked)//发送ASCII
    {
        QByteArray data = str.toUtf8();
        const char *c = data.data();
        while(*c)//判断字符是否符合格式要求
        {
            if(!(*c>='0'&&*c<='9') && !(*c>='a'&&*c<='z') && !(*c>='A'&&*c<='Z') && *c!='\n')
            {
               QMessageBox::warning(this,QStringLiteral("存在不合格字符"),
                                    QStringLiteral("请输入 0-9 a-f A-F"));
               return;
            }
            c++;
        }
        if(serialPort->isOpen()) serialPort->write(data);//发送
        else QMessageBox::warning(this,QStringLiteral("串口未打开"),QStringLiteral("请先打开串口"));
    }
    else if(ui->SendHexCheckBox->checkState() == Qt::Checked)//勾上了Hex
    {
        QString hexStr;
        bool isOk=false;
        QChar *qc1 = str.data();
        QChar *qc2 = qc1+1;
        QByteArray data;
        int i=0;
        char c1,c2;
        while(1)
        {
            c1=qc1->toLatin1();
            c2=qc2->toLatin1();
            if(!c2)//到头了
            {
                if(c1==' '||c1=='\n'||!c1)
                    break;//检索完了，结束
                else
                {
                    QMessageBox::warning(this,QStringLiteral("错误"),
                                         QStringLiteral("请检查是否符合格式要求，两字符构成一个十六进制数。"));
                    return;
                }
            }

            if(c1=='\n'||c1==' ')//遇到回车或者空格再前进一字节
            {
                qc1++;
                qc2++;
                continue;
            }
            if(c2=='\n'||c2==' ')//因为此时c1不可能是空格或回车,所以如果c2为空格，则构不成十六进制数
            {
                QMessageBox::warning(this,QStringLiteral("错误"),
                                     QStringLiteral("请检查是否符合格式要求，两字符构成一个十六进制数。"));
                return;
            }
            if(!(c1>='0'&&c1<='9')&&!(c1>='a'&&c1<='f')&&!(c1>='A'&&c1<='F')//判断是否有不符合的字符出现
             &&!(c2>='0'&&c1<='9')&&!(c2>='a'&&c2<='f')&&!(c2>='A'&&c2<='F'))
            {
                QMessageBox::warning(this,QStringLiteral("存在无法识别的字符"),
                                     QStringLiteral("请规范输入，存在非十六进制字符。"));
                return;
            }
            hexStr = *qc1;//字符1
            hexStr += *qc2;//在字符1后面加上字符2
            data[i] = (quint8)hexStr.toInt(&isOk,16);
            if(isOk) i++;
            else QMessageBox::warning(this,QStringLiteral("转换出错"),
                                      QStringLiteral("不明原因，转换时出错！"));
            qc1+=2;
            qc2+=2;

        }
        if(serialPort->isOpen()) serialPort->write(data);//发送
        else QMessageBox::warning(this,QStringLiteral("串口未打开"),QStringLiteral("请先打开串口"));
    }
}


void MainWindow::on_ReceiveHexCheckBox_stateChanged(int arg1)//接收数据显示框十六进制显示改变
{
    const QString str = ui->ReceiveTextBrowser->document()->toPlainText();
    if(arg1 == 2)//十六进制
    {
        const QByteArray arr= str.toUtf8();
        ui->ReceiveTextBrowser->clear();
        const char *c = arr.data();
        while(*c)
        {
            if(*c == '\n')
            {
                ui->ReceiveTextBrowser->insertPlainText("\n");
                c++;
                continue;
            }
            ui->ReceiveTextBrowser->insertPlainText(QString("%1 ").arg(*c,0,16,QLatin1Char(' ')));
            c++;
        }
    }
    else if(arg1 == 0)//ASCII
    {
        ui->ReceiveTextBrowser->clear();

        QString hexStr;
        bool isOk=false;
        const QChar *qc1 = str.data();
        const QChar *qc2 = qc1+1;
        QByteArray data;
        int i=0;
        char c1,c2;
        while(1)
        {
            c1=qc1->toLatin1();
            c2=qc2->toLatin1();
            if(!c2) break;  //到头了
            if(c1==' ')//遇到回车或者空格再前进一字节
            {
                qc1++; qc2++;
                continue;
            }
            if(c1=='\n')//遇到回车或者空格再前进一字节
            {
                data[i++] = '\n';
                qc1++; qc2++;
                continue;
            }
            if(c2=='\n'||c2==' ')//因为此时c1不可能是空格或回车,所以如果c2为空格，则构不成十六进制数
            {
                qc1+=2; qc2+=2;
                continue;
            }
            hexStr = *qc1;//字符1
            hexStr += *qc2;//在字符1后面加上字符2
            data[i] = (quint8)hexStr.toInt(&isOk,16);
            if(isOk) i++;
            else QMessageBox::warning(this,QStringLiteral("转换出错"),
                                      QStringLiteral("不明原因，转换时出错！"));
            qc1+=3; qc2+=3;
        }
        ui->ReceiveTextBrowser->insertPlainText(QString::fromUtf8(data));
    }
}

void MainWindow::SerialPortParametersSetting(bool set)//串口参数选项使能
{
    ui->PortComboBox->setEnabled(set);
    ui->BaudComboBox->setEnabled(set);
    ui->StopBitComboBox->setEnabled(set);
    ui->CheckComboBox->setEnabled(set);
    ui->DataBitComboBox->setEnabled(set);
    ui->RefreshBtn->setEnabled(set);
}

void MainWindow::on_OpenBtn_clicked()//打开串口
{
    if(serialPort->isOpen())//关闭串口
    {
        serialPort->close();
        ui->OpenBtn->setText(QStringLiteral("打开串口"));
        SerialPortParametersSetting(true);//使能各参数选项
    }
    else//开打串口
    {
        if(serialPort->portName().size()>2&&serialPort->open(QIODevice::ReadWrite))//打开成功
        {
            connect(serialPort,&QSerialPort::readyRead,this,&MainWindow::serialPortReadData);
            ui->OpenBtn->setText(QStringLiteral("关闭串口"));
            SerialPortParametersSetting(false);//失能各参数选项
        }
    }
}

void MainWindow::on_BaudComboBox_currentIndexChanged(const QString &arg1)//波特率改变
{
    if(!serialPort->setBaudRate(arg1.toInt()))
    {
        QMessageBox::warning(this,QStringLiteral("错误！"),QStringLiteral("波特率设置错误:\n")+arg1);
        int index = ui->BaudComboBox->findText(QString("%1").arg(serialPort->baudRate()));
        ui->BaudComboBox->setCurrentIndex(index);
    }
}

void MainWindow::on_StopBitComboBox_currentIndexChanged(int index)//停止位改变
{
    QSerialPort::StopBits stopBit = QSerialPort::OneStop;
    switch (index) {
    case 0:stopBit = QSerialPort::OneStop;
        break;
    case 1:stopBit = QSerialPort::OneAndHalfStop;
        break;
    case 2:stopBit = QSerialPort::TwoStop;
        break;
    }

    if(!serialPort->setStopBits(stopBit))
    {
        QMessageBox::warning(this,QStringLiteral("错误！"),QStringLiteral("停止位设置错误:\n")+index);
        int index = ui->StopBitComboBox->findText(QString("%1").arg(serialPort->stopBits()));
        ui->StopBitComboBox->setCurrentIndex(index);
    }
}

void MainWindow::on_DataBitComboBox_currentIndexChanged(int index)//数据位改变
{
    QSerialPort::DataBits databit;
    switch(index){
    case 0: databit = QSerialPort::Data8;
        break;
    case 1: databit = QSerialPort::Data7;
        break;
    case 2: databit = QSerialPort::Data6;
        break;
    case 3: databit = QSerialPort::Data5;
        break;
    }

    if(!serialPort->setDataBits(databit))
    {
        QMessageBox::warning(this,QStringLiteral("错误"),QStringLiteral("数据位设置错误！"));
        int a ;
        databit = serialPort->dataBits();
        switch (databit) {
        case QSerialPort::Data5 : a = 0;
            break;
        case QSerialPort::Data6 : a = 1;
            break;
        case QSerialPort::Data7 : a = 2;
            break;
        case QSerialPort::Data8 : a = 3;
            break;
        case QSerialPort::UnknownDataBits: a = 3;
            break;
        }
        ui->DataBitComboBox->setCurrentIndex(a);
    }
}

void MainWindow::on_CheckComboBox_currentIndexChanged(int index)//校验位改变
{
    QSerialPort::Parity parity = QSerialPort::NoParity;
    switch (index) {
    case 0:parity = QSerialPort::NoParity;
        break;
    case 1:parity = QSerialPort::OddParity;
        break;
    case 2:parity = QSerialPort::EvenParity;
        break;
    case 3:parity = QSerialPort::SpaceParity;
        break;
    case 4:parity = QSerialPort::MarkParity;
        break;
    }

    if(!serialPort->setParity(parity))
    {
        QMessageBox::warning(this,QStringLiteral("错误！"),QStringLiteral("校验位设置错误:\n")+index);
        int index = ui->CheckComboBox->findText(QString("%1").arg(serialPort->stopBits()));
        ui->CheckComboBox->setCurrentIndex(index);
    }
}

void MainWindow::on_RefreshBtn_clicked()//刷新
{
    int maxLen=0;
    ui->PortComboBox->clear();
    QString text;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        text = info.portName()+"("+info.description()+")";
        ui->PortComboBox->addItem(text);
        if(text.length() > maxLen) maxLen = text.length();
    }
    ui->PortComboBox->view()->setFixedWidth(maxLen*8);
    text = ui->PortComboBox->currentText();
    int index = text.indexOf('(');
    serialPort->setPort(QSerialPortInfo(text.left(index)));
}

void MainWindow::on_ReceiveClearBtn_clicked()//清除接收
{
    ui->ReceiveTextBrowser->clear();
}

void MainWindow::SerialPortError(QSerialPort::SerialPortError error)
{
    QString str;
    switch (error) {
    case QSerialPort::NoError:str = "No Error";
        return;
        break;
    case QSerialPort::DeviceNotFoundError:str = "Device Not Found Error";
        break;
    case QSerialPort::PermissionError:str = "Permission Error";
        break;
    case QSerialPort::ParityError:str = "Parity Error";
        break;
    case QSerialPort::FramingError:str = "Framing Error";
        break;
    case QSerialPort::BreakConditionError:str = "Break Condition Error";
        break;
    case QSerialPort::WriteError:str = "Write Error";
        break;
    case QSerialPort::ReadError:str = "Read Error";
        break;
    case QSerialPort::ResourceError:str = "Resource Error";
        break;
    case QSerialPort::UnsupportedOperationError:str = "Unsupported Operation Error";
        break;
    case QSerialPort::UnknownError:str = "Unknown Error";
        break;
    case QSerialPort::TimeoutError:str = "Timeout Error";
        break;
    case QSerialPort::NotOpenError:str = "Not Open Error";
        break;
    case QSerialPort::OpenError:str = "Open Error";
        break;
    }
    QMessageBox::warning(this,str,serialPort->errorString());
}
