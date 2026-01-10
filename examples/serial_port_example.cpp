/**
 * @file serial_port_example.cpp
 * @brief 串口通信使用示例
 *
 * 此文件展示了如何使用 SerialPort 类进行基本的串口通信
 */

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include "../include/serial_port.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // ========== 示例 1: 基本串口扫描 ==========
    qInfo() << "Available serial ports:";
    const auto ports = SerialPort::scanAvailablePorts();
    for (const auto &port : ports)
    {
        qInfo() << "  -" << port;
    }

    if (ports.isEmpty())
    {
        qWarning() << "No serial ports available!";
        return 1;
    }

    // ========== 示例 2: 打开串口 ==========
    SerialPort serialPort;

    if (!serialPort.open(ports.first(), 115200))
    {
        qCritical() << "Failed to open serial port:" << serialPort.errorString();
        return 1;
    }

    qInfo() << "Serial port opened successfully:" << serialPort.portName();

    // ========== 示例 3: 连接信号 ==========
    QObject::connect(&serialPort, &SerialPort::dataReceived,
                     [](const QString &data, SerialPort::DataFormat format) {
                         QString formatStr = (format == SerialPort::DataFormat::HEX) ? "HEX" : "ASCII";
                         qInfo() << "Received (" << formatStr << "):" << data;
                     });

    QObject::connect(&serialPort, &SerialPort::dataSent,
                     [](const QString &data, SerialPort::DataFormat format) {
                         QString formatStr = (format == SerialPort::DataFormat::HEX) ? "HEX" : "ASCII";
                         qInfo() << "Sent (" << formatStr << "):" << data;
                     });

    QObject::connect(&serialPort, &SerialPort::errorOccurred,
                     [](const QString &error) {
                         qWarning() << "Serial port error:" << error;
                     });

    // ========== 示例 4: 发送 ASCII 数据 ==========
    qInfo() << "Sending ASCII data...";
    serialPort.write("Hello, Serial Port!", SerialPort::DataFormat::ASCII);

    // 等待一些数据
    QThread::msleep(100);

    // ========== 示例 5: 发送 HEX 数据 ==========
    qInfo() << "Sending HEX data...";
    serialPort.write("48 65 6C 6C 6F", SerialPort::DataFormat::HEX);

    // 等待一些数据
    QThread::msleep(100);

    // ========== 示例 6: 发送带换行符的数据 ==========
    qInfo() << "Sending data with newline...";
    serialPort.write("AT+RST\n", SerialPort::DataFormat::ASCII);

    // 等待一些数据
    QThread::msleep(1000);

    // ========== 示例 7: 读取数据 ==========
    qInfo() << "Available bytes:" << serialPort.bytesAvailable();
    if (serialPort.bytesAvailable() > 0)
    {
        QByteArray data = serialPort.readAll();
        qInfo() << "Raw data:" << data.toHex();
    }

    // ========== 示例 8: 关闭串口 ==========
    serialPort.close();
    qInfo() << "Serial port closed";

    return 0;
}
