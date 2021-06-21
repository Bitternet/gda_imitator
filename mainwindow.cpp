#include "mainwindow.h"
#include "ui_mainwindow.h"

#include    <QComboBox>
#include    <QPushButton>
#include    <QSerialPortInfo>
#include    <QPlainTextEdit>
#include    <QFileInfo>
#include    <QHeaderView>
#include    <QAction>
#include    <QFileDialog>
#include    <QStandardPaths>
#include    <QDebug>
#include    <QBitArray>
#include    <QMap>
#include    <QThread>

enum
{
    PORT_LIST_UPDATE_INTERVAL = 100
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    otvet = new QByteArray();
    //slave = new Slave();

    int baud[9] = {600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

    for(int i = 0; i < 9; i++)
           {
              ui->comboBox_baud->addItem(QString::number(baud[i]));
           }

    int bits[2] = {7, 8};

    for(int i = 0; i < 2; i++)
           {
              ui->comboBox_bits->addItem(QString::number(bits[i]));
           }

    double stop_bits[3] = {1, 1.5, 2};

    for(int i = 0; i < 3; i++)
           {
              ui->comboBox_stop_bits->addItem(QString::number(stop_bits[i]));
           }

    QStringList parity =  {"None", "Odd", "Even"};

    for(int i = 0; i < 3; i++)
           {
              ui->comboBox_parity->addItem(parity[i]);
           }

    ui->comboBox_port->setMinimumHeight(30);
    ui->comboBox_baud->setMinimumHeight(30);
    ui->comboBox_bits->setMinimumHeight(30);
    ui->comboBox_stop_bits->setMinimumHeight(30);
    ui->comboBox_parity->setMinimumHeight(30);

    ui->comboBox_baud->setCurrentIndex(4);
    ui->comboBox_bits->setCurrentIndex(1);
    ui->comboBox_stop_bits->setCurrentIndex(0);
    ui->comboBox_parity->setCurrentIndex(0);

    findPort();

    modnet = new ModbusNetwork(this);

    connect(modnet, &ModbusNetwork::logPrint, this, &MainWindow::logPring);

    connect(modnet, &ModbusNetwork::send_data_array, this, &MainWindow::up);

    updateTimer = new QTimer(this);
    updateTimer->setInterval(PORT_LIST_UPDATE_INTERVAL);

    updateReg = new QTimer(this);
    updateReg -> setInterval(10000);

    ports_count = 0;

    connect(updateTimer, &QTimer::timeout,
            this, &MainWindow::updatePortsList);

    connect(updateReg, &QTimer::timeout,
            this, &MainWindow::updateRegisters);

    updateTimer -> start();
    updateReg -> start();

    connect(ui->pushButton_connect, &QPushButton::released,
            this, &MainWindow::onConnectionRelease);
    connect(ui->pushButton_update, &QPushButton::released,
            this, &MainWindow::update);

    ui->pushButton_connect->setStyleSheet("background-color: green; color: white;");

    ui->tabWidget->setVisible(false);

    massiv_str.push_back({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
    massiv_str.push_back({1,2,3,0,5,6,7,8,9,0,10,12,34,23,23,4,5,6,7,8,23,21,23,34,35,3,53,3,6,32,2,1});
    massiv_str.push_back({2,13,34,5,5,6,7,5,9,0,11,58,34,23,55,4,5,62,7,83,3,2,23,34,35,3,3,3,6,2,23,1});
    massiv_str.push_back({1,23,3,0,54,6,7,8,9,0,10,2,34,2,2,4,5,63,7,83,23,21,23,3,3,3,3,3,63,3,2,1});
    massiv_str.push_back({1,2,3,0,5,6,7,8,9,0,10,12,34,23,23,4,5,6,7,8,23,21,23,34,35,3,53,3,6,32,2,1});
    massiv_str.push_back({2,13,34,5,5,6,7,5,9,0,11,58,34,23,55,4,5,62,7,83,3,2,23,34,35,3,3,3,6,2,23,1});
    massiv_str.push_back({1,23,3,0,54,6,7,8,9,0,10,2,34,2,2,4,5,63,7,83,23,21,23,3,3,3,3,3,63,3,2,1});
    massiv_str.push_back({1,2,3,0,5,6,7,8,9,0,10,12,34,23,23,4,5,6,7,8,23,21,23,34,35,3,53,3,6,32,2,1});
    massiv_str.push_back({2,13,34,5,5,6,7,5,9,0,11,58,34,23,55,4,5,62,7,83,3,2,23,34,35,3,3,3,6,2,23,1});
    massiv_str.push_back({1,23,3,0,54,6,7,8,9,0,10,2,34,2,2,4,5,63,7,83,23,21,23,3,3,3,3,3,63,3,2,1});
    massiv_str.push_back({1,2,3,0,5,6,7,8,9,0,10,12,34,23,23,4,5,6,7,8,23,21,23,34,35,3,53,3,6,32,2,1});
    massiv_str.push_back({2,13,34,5,5,6,7,5,9,0,11,58,34,23,55,4,5,62,7,83,3,2,23,34,35,3,3,3,6,2,23,1});
    massiv_str.push_back({1,23,3,0,54,6,7,8,9,0,10,2,34,2,2,4,5,63,7,83,23,21,23,3,3,3,3,3,63,3,2,1});
    massiv_str.push_back({1,2,3,0,5,6,7,8,9,0,10,12,34,23,23,4,5,6,7,8,23,21,23,34,35,3,53,3,6,32,2,1});
    massiv_str.push_back({2,13,34,5,5,6,7,5,9,0,11,58,34,23,55,4,5,62,7,83,3,2,23,34,35,3,3,3,6,2,23,1});
    massiv_str.push_back({1,23,3,0,54,6,7,8,9,0,10,2,34,2,2,4,5,63,7,83,23,21,23,3,3,3,3,3,63,3,2,1});
    massiv_str.push_back({1,2,3,0,5,6,7,8,9,0,10,12,34,23,23,4,5,6,7,8,23,21,23,34,35,3,53,3,6,32,2,1});
    massiv_str.push_back({2,13,34,5,5,6,7,5,9,0,11,58,34,23,55,4,5,62,7,83,3,2,23,34,35,3,3,3,6,2,23,1});
    massiv_str.push_back({1,23,3,0,54,6,7,8,9,0,10,2,34,2,2,4,5,63,7,83,23,21,23,3,3,3,3,3,63,3,2,1});
    massiv_str.push_back({1,2,3,0,5,6,7,8,9,0,10,12,34,23,23,4,5,6,7,8,23,21,23,34,35,3,53,3,6,32,2,1});
    massiv_str.push_back({2,13,34,5,5,6,7,5,9,0,11,58,34,23,55,4,5,62,7,83,3,2,23,34,35,3,3,3,6,2,23,1});
}

MainWindow::~MainWindow()
{
    delete ui;
}


QByteArray MainWindow::bitsToBytes(QBitArray bits)
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << bits;
    return bytes;
}

void MainWindow::findPort()
{
    ui->comboBox_port->clear();

        const auto infos = QSerialPortInfo::availablePorts();

        for (const QSerialPortInfo &info : infos)
        {
            QStringList list;
            list << info.portName();
            ui->comboBox_port->addItem(list.first());
        }
 }

void MainWindow::updatePortsList()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    if (ports.count() != ports_count)
    {
        ports_count = ports.count();
        ui->comboBox_port->clear();

        for (int i = 0; i < ports_count; i++)
            ui->comboBox_port->addItem(ports.at(i).portName());
    }
}

QByteArray MainWindow::convertFloatToValues(float f)
{
    QByteArray array(reinterpret_cast<const char*>(&f), sizeof(f));
    return array;
}

float MainWindow::convertArrayToFloat(QByteArray array_bytes, quint8 position)
{
    QByteArray tmp;

    tmp[0] = array_bytes[position+1];
    tmp[1] = array_bytes[position];
    tmp[2] = array_bytes[position+3];
    tmp[3] = array_bytes[position+2];
    float f;
    if (tmp.size() >= sizeof(f))
    f = *reinterpret_cast<const float*>(tmp.data());
    return f;
}

void MainWindow::updateRegisters()
{
    if (modnet->isConnected())
    {
        i_str++;

       if(i_str < 21)
       send(massiv_str[i_str]);
    }
    //send(21,10);

    //send(61,9);

    //QThread::usleep(100000);
    //QThread::msleep(500);
   // send(61,9);
    //QThread::usleep(100000);




/*
        if(reg == 21)
        {
        QByteArray w;

        quint8 dat[6];
        quint8 adress = 1;
        quint8 func = 4;
        quint16 count = 6;

        w[0] = adress;
        w[1] = func;
        w[2] = hiByte(reg);
        w[3] = loByte(reg);
        w[4] = hiByte(count);
        w[5] = loByte(count);

        dat[0] = adress;
        dat[1] = func;
        dat[2] = hiByte(reg);
        dat[3] = loByte(reg);
        dat[4] = hiByte(count);
        dat[5] = loByte(count);

        quint16 ks = calcCRC16table(dat, 6);

        w[6] = hiByte(ks);
        w[7] = loByte(ks);
        modnet->sendData(w);
        qDebug() << w;
        }*/

        //w[4,5] = calcCRC16simple(quint8 *buff, quint8 size);
}

void MainWindow::send(QVector<int> val_sectors)
{
    QByteArray w;
    quint8 dat[50];
    quint8 z1 = 71;
    quint8 z2 = 68;
    quint8 z3 = 65;
    quint8 z4 = 49;

    quint16 nomer = 999;
    quint16 count_interval = 9;
    count_all += count_interval;
    quint16 level_sound = 180;

    w[0] = z1;
    w[1] = z2;
    w[2] = z3;
    w[3] = z4;
    w[4] = 9;
    w[5] = hiByte(nomer);
    w[6] = loByte(nomer);
    w[7] = 0;//ошибки

    w[40] = hiByte(count_interval);
    w[41] = loByte(count_interval);
    w[42] = hiByte(count_all);
    w[43] = loByte(count_all);
    w[44] = hiByte(level_sound);
    w[45] = loByte(level_sound);

    int j = 0;

    for(int i = 8; i < 40; i++)
    {
         w[i] = val_sectors[j];
         j++;
    }

    for(int i =0; i < 46; i++)
        dat[i] = w[i];

    quint32 crc32 = fromByteArray(w);

    QByteArray c =  convertFloatToValues((float)crc32);

    w[46] = c[0];
    w[47] = c[1];
    w[48] = c[2];
    w[49] = c[3];

    modnet->sendData(w);
    QString s = "";
    foreach(auto r, w)
    s += QString::number(r) + "  ";
    ui->ptSystemLog->appendHtml(s);
    qDebug() << w;
}


void MainWindow::update()
{
    updateRegisters();
}

void MainWindow::onConnectionRelease()
{
    if (modnet->isConnected())
    {
        modnet->closeConnection();

        ui->pushButton_connect->setText("Подключить");
        ui->pushButton_connect->setStyleSheet("background-color: green; color: white;");
        logPring("Устройство отключено");
    }
    else
    {
        serial_config_t sp_config;

        sp_config.portName = ui->comboBox_port->currentText();
        sp_config.baudrate = ui->comboBox_baud->currentText().toInt();
        sp_config.dataBits = ui->comboBox_bits->currentText().toInt();
        sp_config.stopBits = ui->comboBox_stop_bits->currentText().toInt();
        sp_config.parity = ui->comboBox_parity->currentText();

        modnet->init(sp_config);
        modnet->openConnection();

        ui->pushButton_connect->setText("Отключить");
        ui->pushButton_connect->setStyleSheet("background-color: red; color: white;");

        logPring("Устройство подключено");
    }
}

void MainWindow::logPring(QString msg)
{
    ui->ptSystemLog->appendPlainText(msg);
}

void MainWindow::onApplicationQuit()
{
    if (modnet->isConnected())
        modnet->closeConnection();

    QApplication::quit();
}


void MainWindow::up(QByteArray arr)
{
    otvet->append(arr);
    //qDebug()<<otvet->length();

    if(otvet->length() == 275)
    {
    QByteArray array = *otvet;

    ui->lineEdit_reg_7->setText(QString::number(convertArrayToFloat(array, 3)));
    ui->lineEdit_reg_9->setText(QString::number(convertArrayToFloat(array, 7)));
    ui->lineEdit_reg_11->setText(QString::number(convertArrayToFloat(array, 11)));

    ui->lineEdit_reg_21->setText(QString::number(convertArrayToFloat(array,31)));
    ui->lineEdit_reg_23->setText(QString::number(convertArrayToFloat(array,35)));
    ui->lineEdit_reg_25->setText(QString::number(convertArrayToFloat(array,39)));
    ui->lineEdit_reg_29->setText(QString::number(convertArrayToFloat(array,47)));

    ui->lineEdit_reg_61->setText(QString::number(convertArrayToFloat(array,111)));
    ui->lineEdit_reg_63->setText(QString::number(convertArrayToFloat(array,115)));
    ui->lineEdit_reg_65->setText(QString::number(convertArrayToFloat(array,119)));
    ui->lineEdit_reg_68->setText(QString::number(convertArrayToFloat(array,125)));

    ui->lineEdit_reg_71->setText(QString::number((quint16)(word(array[131],array[132]))));
    ui->lineEdit_reg_74->setText(QString::number(convertArrayToFloat(array,137)));
    ui->lineEdit_reg_77->setText(QString::number((quint16)(word(array[143],array[144]))));
    ui->lineEdit_reg_78->setText(QString::number(convertArrayToFloat(array,145)));
    ui->lineEdit_reg_81->setText(QString::number((quint16)(word(array[151],array[152]))));

    quint16 reg_101 = (quint16)(word(array[191],array[192]));

    ui->checkBox_reg_101_3->setChecked(getBit(loByte(reg_101), 3));
    ui->checkBox_reg_101_4->setChecked(getBit(loByte(reg_101), 4));
    ui->checkBox_reg_101_5->setChecked(getBit(loByte(reg_101), 5));

    quint16 reg_111 = (quint16)(word(array[211],array[212]));

    ui->checkBox_reg_111_0->setChecked(getBit(loByte(reg_111), 0));
    ui->checkBox_reg_111_1->setChecked(getBit(loByte(reg_111), 1));
    ui->checkBox_reg_111_2->setChecked(getBit(loByte(reg_111), 2));
    ui->checkBox_reg_111_4->setChecked(getBit(loByte(reg_111), 4));
    ui->checkBox_reg_111_6->setChecked(getBit(loByte(reg_111), 6));
    ui->checkBox_reg_111_8->setChecked(getBit(hiByte(reg_111), 0));
    ui->checkBox_reg_111_13->setChecked(getBit(hiByte(reg_111), 5));
    ui->checkBox_reg_111_15->setChecked(getBit(hiByte(reg_111), 7));

    quint16 reg_112 = (quint16)(word(array[213],array[214]));

    ui->checkBox_reg_112_0->setChecked(getBit(loByte(reg_112), 0));
    ui->checkBox_reg_112_1->setChecked(getBit(loByte(reg_112), 1));
    ui->checkBox_reg_112_2->setChecked(getBit(loByte(reg_112), 2));
    ui->checkBox_reg_112_3->setChecked(getBit(loByte(reg_112), 3));
    ui->checkBox_reg_112_4->setChecked(getBit(loByte(reg_112), 4));
    ui->checkBox_reg_112_7->setChecked(getBit(loByte(reg_112), 7));
    ui->checkBox_reg_112_9->setChecked(getBit(hiByte(reg_112), 1));
    ui->checkBox_reg_112_10->setChecked(getBit(hiByte(reg_112), 2));
    ui->checkBox_reg_112_11->setChecked(getBit(hiByte(reg_112), 3));
    ui->checkBox_reg_112_12->setChecked(getBit(hiByte(reg_112), 4));
    ui->checkBox_reg_112_14->setChecked(getBit(hiByte(reg_112), 6));
    ui->checkBox_reg_112_15->setChecked(getBit(hiByte(reg_112), 7));

    quint16 reg_113 = (quint16)(word(array[215],array[216]));

    ui->checkBox_reg_113_1->setChecked(getBit(loByte(reg_113), 1));
    ui->checkBox_reg_113_2->setChecked(getBit(loByte(reg_113), 2));
    ui->checkBox_reg_113_3->setChecked(getBit(loByte(reg_113), 3));

    quint16 reg_114 = (quint16)(word(array[217],array[218]));

    ui->checkBox_reg_114_3->setChecked(getBit(loByte(reg_114), 3));
    ui->checkBox_reg_114_4->setChecked(getBit(loByte(reg_114), 4));
    ui->checkBox_reg_114_5->setChecked(getBit(loByte(reg_114), 5));

    quint16 reg_115 = (quint16)(word(array[219],array[220]));

    ui->checkBox_reg_115_0->setChecked(getBit(loByte(reg_115), 0));

    quint16 reg_119 = (quint16)(word(array[227],array[228]));

    ui->checkBox_reg_119_0->setChecked(getBit(loByte(reg_119), 0));
    ui->checkBox_reg_119_3->setChecked(getBit(loByte(reg_119), 3));
    ui->checkBox_reg_119_4->setChecked(getBit(loByte(reg_119), 4));
    ui->checkBox_reg_119_7->setChecked(getBit(loByte(reg_119), 7));

    quint16 reg_121 = (quint16)(word(array[231],array[232]));

    if(reg_121 == 0)
    ui->radioButton_reg_121_0->setChecked(true);
    else if(reg_121 == 1)
    ui->radioButton_reg_121_1->setChecked(true);
    else if(reg_121 == 2)
    ui->radioButton_reg_121_2->setChecked(true);

    quint16 reg_133 = (quint16)(word(array[255],array[256]));

    if(reg_133 == 0)
    ui->radioButton_reg_133_0->setChecked(true);
    else if(reg_133 == 1)
    ui->radioButton_reg_133_1->setChecked(true);
    else if(reg_133 == 6)
    ui->radioButton_reg_133_6 ->setChecked(true);
    else if(reg_133 == 16)
    ui->radioButton_reg_133_16 ->setChecked(true);
    else if(reg_133 == 4)
    ui->radioButton_reg_133_4 ->setChecked(true);
    else if(reg_133 == 25)
    ui->radioButton_reg_133_25 ->setChecked(true);
    else if(reg_133 == 19)
    ui->radioButton_reg_133_19 ->setChecked(true);

    otvet->clear();
    }

}
