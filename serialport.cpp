#include "serialport.h"
#include "ui_serialport.h"
#include <QDateTime>
#include <QSerialPortInfo>


SerialPort::SerialPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialPort)
{
    ui->setupUi(this);

    m_pSerialPort = nullptr;
    init();

    //扫描串口信号连接
    connect(ui->btnScan, &QPushButton::clicked,
            this, &SerialPort::scanPort);
    //串口打开信号连接
    connect(ui->btnSwitch, &QPushButton::clicked,
            this, &SerialPort::slotSwitchPort);
    //数据发送信号连接
    connect(ui->btnSend, &QPushButton::clicked,
            this, &SerialPort::slotSendData);
    //清除文本信号连接
    connect(ui->btnClear, &QPushButton::clicked,
            this, &SerialPort::slotClearText);

}

SerialPort::~SerialPort()
{
    delete ui;
    delete m_pSerialPort;
}

//初始化函数
void SerialPort::init()
{
    m_pSerialPort = new QSerialPort(this);
    scanPort();
    ui->comboBoxData->setCurrentIndex(3);
    ui->btnSend->setEnabled(false);
}

//扫描串口
void SerialPort::scanPort()
{
    ui->comboBoxPort->clear();
    QStringList SerialPortNameList;

    //遍历本机可用串口
    foreach(const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
    {
        SerialPortNameList << info.portName();
    }

    ui->comboBoxPort->addItems(SerialPortNameList);

    if (SerialPortNameList.isEmpty())
    {
        ui->status->setChecked(false);
    }
}

//******槽函数实现******//

void SerialPort::slotSwitchPort()
{
    if (m_pSerialPort->isOpen())
        closeSerialPort();
    else
        openSerialPort();
}

//串口打开函数
void SerialPort::openSerialPort()
{
    m_pSerialPort->clear();

    //设置端口号
    m_pSerialPort->setPortName(ui->comboBoxPort->currentText());

    if (!m_pSerialPort->open(QIODevice::ReadWrite))
    {
        ui->textEditInfo->append(ui->comboBoxPort->currentText() + " Open Fail!");
        return;
    }

    //设置波特率
    switch (ui->comboBoxBaud->currentIndex())
    {
    case 0:
        m_pSerialPort->setBaudRate(QSerialPort::Baud9600);
        break;
    case 1:
        m_pSerialPort->setBaudRate(QSerialPort::Baud19200);
        break;
    case 2:
        m_pSerialPort->setBaudRate(QSerialPort::Baud115200);
        break;
    default:
        m_pSerialPort->setBaudRate(QSerialPort::Baud9600);
        break;
    }

    //设置数据位
    switch (ui->comboBoxData->currentIndex())
    {
    case 0:
        m_pSerialPort->setDataBits(QSerialPort::Data5);
        break;
    case 1:
        m_pSerialPort->setDataBits(QSerialPort::Data6);
        break;
    case 2:
        m_pSerialPort->setDataBits(QSerialPort::Data7);
        break;
    case 3:
        m_pSerialPort->setDataBits(QSerialPort::Data8);
        break;
    default:
        m_pSerialPort->setDataBits(QSerialPort::Data8);
        break;
    }

    //设置校验位
    switch (ui->comboBoxParity->currentIndex())
    {
    case 0:
        m_pSerialPort->setParity(QSerialPort::NoParity);
        break;
    case 1:
        m_pSerialPort->setParity(QSerialPort::EvenParity);
        break;
    case 2:
        m_pSerialPort->setParity(QSerialPort::OddParity);
        break;
    case 3:
        m_pSerialPort->setParity(QSerialPort::SpaceParity);
        break;
    case 4:
        m_pSerialPort->setParity(QSerialPort::MarkParity);
        break;
    default:
        m_pSerialPort->setParity(QSerialPort::NoParity);
        break;
    }

    //设置停止位
    switch (ui->comboBoxStop->currentIndex())
    {
    case 0:
        m_pSerialPort->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        m_pSerialPort->setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        m_pSerialPort->setStopBits(QSerialPort::TwoStop);
        break;
    default:
        m_pSerialPort->setStopBits(QSerialPort::OneStop);
        break;
    }

    //设置流控位
    switch (ui->comboBoxFlow->currentIndex())
    {
    case 0:
        m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl);
        break;
    case 1:
        m_pSerialPort->setFlowControl(QSerialPort::HardwareControl);
        break;
    case 2:
        m_pSerialPort->setFlowControl(QSerialPort::SoftwareControl);
        break;
    default:
        m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl);
        break;
    }

    //数据接收信号的连接
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &SerialPort::slotReceiveData);
    updateOpenStatus();
}

void SerialPort::updateOpenStatus()
{
    ui->btnSwitch->setChecked(true);
    ui->btnSwitch->setText("关闭");
    ui->status->setChecked(true);
    ui->btnSend->setEnabled(true);

    ui->comboBoxPort->setEnabled(false);
    ui->comboBoxBaud->setEnabled(false);
    ui->comboBoxData->setEnabled(false);
    ui->comboBoxParity->setEnabled(false);
    ui->comboBoxStop->setEnabled(false);
    ui->comboBoxFlow->setEnabled(false);
    ui->btnScan->setEnabled(false);
}

//串口关闭槽函数
void SerialPort::closeSerialPort()
{
    if (!m_pSerialPort->isOpen())
        return;
    m_pSerialPort->clear();
    m_pSerialPort->close();
    updateCloseStatus();
}

void SerialPort::updateCloseStatus()
{
    ui->btnSwitch->setChecked(false);
    ui->btnSwitch->setText("打开");
    ui->status->setChecked(false);
    ui->btnSend->setEnabled(false);

    ui->comboBoxPort->setEnabled(true);
    ui->comboBoxBaud->setEnabled(true);
    ui->comboBoxData->setEnabled(true);
    ui->comboBoxParity->setEnabled(true);
    ui->comboBoxStop->setEnabled(true);
    ui->comboBoxFlow->setEnabled(true);
    ui->btnScan->setEnabled(true);
}

//数据发送槽函数
void SerialPort::slotSendData()
{
    if (!m_pSerialPort->isOpen())
    {
        return;
    }

    //test
    //ui->lineEditSend->setText("00 03 3A 98 00 01 08 EC");
    QString strSend     = ui->lineEditSend->text();
    QByteArray byteSend = strSend.toUtf8();
    QDateTime datatime;

    if (ui->radioButtonHex->isChecked())
    {
        byteSend = QByteArray::fromHex(byteSend); //byte转hex
        ui->textEditInfo->append(datatime.currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]")
                                + " Send Hex: " + '\n' + strSend + '\n');
    } else {
        ui->textEditInfo->append(datatime.currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]")
                                + " Send Ascii: " + '\n' + strSend + '\n');
    }
    m_pSerialPort->write(byteSend);

}

//数据接收槽函数
void SerialPort::slotReceiveData()
{
    QDateTime datatime;
    QString strReceive;
    QByteArray byteReceive = m_pSerialPort->readAll();
    QString strTmp = byteReceive.toHex();
    for (int i = 0; i < strTmp.length(); i += 2)
        strReceive += strTmp.mid(i, 2) + " ";

    if (ui->radioButtonHex->isChecked())
    {
        ui->textEditInfo->append(datatime.currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]")
                                + " Recv Hex: " + '\n' + strReceive + '\n');
    } else {
        ui->textEditInfo->append(datatime.currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]")
                                + " Recv Ascii: " + '\n' + byteReceive + '\n');
    }
}

//清除文本信息槽函数
void SerialPort::slotClearText()
{
    ui->textEditInfo->clear();
}
