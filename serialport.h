#ifndef serialport_H
#define serialport_H

#include <QWidget>
#include <QSerialPort>

namespace Ui {
class SerialPort;
}

class SerialPort : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPort(QWidget *parent = nullptr);
    ~SerialPort();

protected:
    void init();
    void scanPort();
    void openSerialPort();
    void updateOpenStatus();
    void closeSerialPort();
    void updateCloseStatus();

protected slots:
    void slotSwitchPort();
    void slotSendData();
    void slotReceiveData();
    void slotClearText();

private:
    Ui::SerialPort *ui;

    QSerialPort *m_pSerialPort;
};

#endif // serialport_H

