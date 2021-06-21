#include    "modbus.h"

#include    <QThread>
#include    <QDebug>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
ModbusNetwork::ModbusNetwork(QObject *parent) : QObject(parent)
{
    serialPort = nullptr;
    is_connected = false;
    silence_interval = 2000;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
ModbusNetwork::~ModbusNetwork()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::init(const serial_config_t &serial_config)
{
    // Save serial port configuration
    sp_config = serial_config;

    serialPort = new QSerialPort(this);

    connect(serialPort, &QSerialPort::readyRead,
            this, &ModbusNetwork::receive);

    connect(serialPort, &QSerialPort::errorOccurred,
            this, &ModbusNetwork::errorSerialPort);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool ModbusNetwork::isConnected() const
{
    return is_connected;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::addSlave(Slave *slave)
{
    connect(this, &ModbusNetwork::sendDataToSlaves, slave, &Slave::processData);
    connect(slave, &Slave::sendData, this, &ModbusNetwork::sendData);
    connect(slave, &Slave::logPrint, this, &ModbusNetwork::logSlavePrint);

    slaves.insert(slave->getID(), slave);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
QMap<int, Slave *> ModbusNetwork::getSlaves() const
{
    return slaves;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::clear()
{
    if (slaves.count() == 0)
        return;

    QMap<int, Slave *>::iterator it;

    for (it = slaves.begin(); it != slaves.end(); ++it)
    {
        Slave *slave = it.value();

        disconnect(this, &ModbusNetwork::sendDataToSlaves, slave, &Slave::processData);
        disconnect(slave, &Slave::sendData, this, &ModbusNetwork::sendData);
        disconnect(slave, &Slave::logPrint, this, &ModbusNetwork::logSlavePrint);

        delete slave;
    }

    slaves.clear();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::openConnection()
{
    // Check connection state
    if (is_connected)
    {
        if (serialPort->isOpen())
        {
            closeConnection();
        }
    }
    else
    {
        // Set port parameters
        serialPort->setPortName(sp_config.portName);
        serialPort->setBaudRate(static_cast<QSerialPort::BaudRate>(sp_config.baudrate));
        serialPort->setDataBits(static_cast<QSerialPort::DataBits>(sp_config.dataBits));
        serialPort->setStopBits(static_cast<QSerialPort::StopBits>(sp_config.stopBits));
        serialPort->setParity(static_cast<QSerialPort::Parity>(sp_config.getPariry()));

        silence_interval = (unsigned long) (SILENCE_INTERVAL_MULTIPLE / sp_config.baudrate);

        if (silence_interval < MIN_SILENCE_INTERVAL)
            silence_interval = MIN_SILENCE_INTERVAL;

        // Connection
        if (serialPort->open(QIODevice::ReadWrite))
        {
            is_connected = true;
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::closeConnection()
{
    serialPort->close();
    is_connected = false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::sendData(QByteArray data)
{
    if (serialPort->isOpen())
    {
        QThread::usleep(silence_interval);

        serialPort->write(data);
        serialPort->flush();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::receive()
{
    while(serialPort->bytesAvailable() !=0)
    {
    QByteArray data = serialPort->read(2);

    if((quint8)data[0] == 1 && (quint8)data[1] == 4)
    {
        QByteArray tmp_1 = serialPort->readAll();
        data.append(tmp_1);
        send_data_array(data);
    }
    else
    {
        QByteArray tmp_2 = serialPort->readAll();
        data.append(tmp_2);
        //qDebug()<<data;
        send_data_array(data);
    }
    }


    /*
    QString start = "\n====КОМАНДА====\n";
    QString s0 = "Адрес устройства: " + QString::number(data[0]);
    QString s1 = "Код функции: " + QString::number(data[1]);
    QString s2 = "Количество: " + QString::number(data[2]);
    QString s3 = "Данные: "; //+ QString::number(word(data[3],data[4]));
    QString end = "===============";

    logPrint(start + s0 + "\n" + s1 + "\n" + s2  + "\n" + s3 + "\n " + end);
    */

    //logPrint(data);
    //emit sendDataToSlaves(data);
    //send_data_array(data);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::errorSerialPort(QSerialPort::SerialPortError error)
{
    Q_UNUSED(error)
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusNetwork::logSlavePrint(QString msg)
{
    emit logPrint(msg);
}

