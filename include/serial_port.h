#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QByteArray>
#include <memory>

/**
 * @class SerialPort
 * @brief 串口通信管理类
 *
 * 提供完整的串口管理功能，包括端口扫描、连接、读写等操作
 */
class SerialPort : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 串口数据格式枚举
     */
    enum class DataFormat
    {
        ASCII,  ///< ASCII 格式
        HEX,    ///< HEX 格式
        UTF8    ///< UTF-8 格式
    };
    Q_ENUM(DataFormat)

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit SerialPort(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~SerialPort() override;

    /**
     * @brief 扫描可用的串口
     * @return 可用串口列表
     */
    static QStringList scanAvailablePorts();

    /**
     * @brief 获取串口信息
     * @param portName 串口名称
     * @return 串口信息
     */
    static QSerialPortInfo getPortInfo(const QString &portName);

    /**
     * @brief 打开串口
     * @param portName 串口名称
     * @param baudRate 波特率
     * @param dataBits 数据位
     * @param stopBits 停止位
     * @param parity 校验位
     * @param flowControl 流控制
     * @return 成功返回true，失败返回false
     */
    bool open(const QString &portName,
              qint32 baudRate = QSerialPort::Baud115200,
              QSerialPort::DataBits dataBits = QSerialPort::Data8,
              QSerialPort::StopBits stopBits = QSerialPort::OneStop,
              QSerialPort::Parity parity = QSerialPort::NoParity,
              QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl);

    /**
     * @brief 关闭串口
     */
    void close();

    /**
     * @brief 判断串口是否已打开
     * @return 已打开返回true，未打开返回false
     */
    bool isOpen() const;

    /**
     * @brief 发送数据
     * @param data 要发送的数据
     * @param format 数据格式
     * @return 发送的字节数，失败返回-1
     */
    qint64 write(const QString &data, DataFormat format = DataFormat::ASCII);

    /**
     * @brief 发送原始数据
     * @param data 要发送的原始数据
     * @return 发送的字节数，失败返回-1
     */
    qint64 writeRaw(const QByteArray &data);

    /**
     * @brief 读取所有可用数据
     * @return 读取的数据
     */
    QByteArray readAll();

    /**
     * @brief 读取指定字节数的数据
     * @param maxSize 最大字节数
     * @return 读取的数据
     */
    QByteArray read(qint64 maxSize);

    /**
     * @brief 获取待读取的字节数
     * @return 字节数
     */
    qint64 bytesAvailable() const;

    /**
     * @brief 清空接收缓冲区
     * @return 成功返回true
     */
    bool clearRecvBuffer();

    /**
     * @brief 清空发送缓冲区
     * @return 成功返回true
     */
    bool clearSendBuffer();

    /**
     * @brief 获取当前串口名称
     * @return 串口名称
     */
    QString portName() const;

    /**
     * @brief 获取当前波特率
     * @return 波特率
     */
    qint32 baudRate() const;

    /**
     * @brief 获取错误信息
     * @return 错误描述
     */
    QString errorString() const;

    /**
     * @brief 十六进制字符串转字节数组
     * @param hexString HEX格式字符串
     * @return 转换后的字节数组
     */
    static QByteArray hexStringToByteArray(const QString &hexString);

    /**
     * @brief 字节数组转十六进制字符串
     * @param data 字节数组
     * @return HEX格式字符串
     */
    static QString byteArrayToHexString(const QByteArray &data);

signals:
    /**
     * @brief 数据接收信号
     * @param data 接收到的数据
     * @param format 数据格式
     */
    void dataReceived(const QString &data, DataFormat format);

    /**
     * @brief 数据发送信号
     * @param data 发送的数据
     * @param format 数据格式
     */
    void dataSent(const QString &data, DataFormat format);

    /**
     * @brief 错误信号
     * @param error 错误描述
     */
    void errorOccurred(const QString &error);

    /**
     * @brief 连接状态改变信号
     * @param isOpen 是否已连接
     */
    void connectionStatusChanged(bool isOpen);

private slots:
    /**
     * @brief 处理数据接收
     */
    void onDataReceived();

    /**
     * @brief 处理错误
     */
    void onError(QSerialPort::SerialPortError error);

private:
    std::unique_ptr<QSerialPort> m_serialPort;  ///< 串口对象
    DataFormat m_dataFormat = DataFormat::ASCII; ///< 数据格式
};

#endif // SERIAL_PORT_H
