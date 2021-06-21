#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QListWidget>
#include <QTableWidget>
#include "modbus.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ModbusNetwork   *modnet;
    QTimer          *updateTimer;
    QTimer          *updateReg;
    int             ports_count;

    void findPort();
    void onConnectionRelease();
    void updatePortsList();
    void updateRegisters();
    void clearSlave();

    QByteArray bitsToBytes(QBitArray bits);

    QByteArray convertFloatToValues(float f);
    float convertArrayToFloat(QByteArray array_bytes, quint8 position);

    quint16 count_all = 0;

    QVector<int> val_sectors;
    QVector<QVector<int>> massiv_str;
    int i_str = 0;

    void update();
    Slave *slave;
    void send(QVector<int> val_sectors);
    void onApplicationQuit();

    QByteArray *otvet;


private slots:
    void logPring(QString msg);
    void up(QByteArray arr);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
