#include "serial_port.h"
#include <QDebug>
#include <QRegularExpression>

SerialPort::SerialPort(QObject *parent)
    : QObject(parent),
      m_serialPort(std::make_unique<QSerialPort>(this))
{
    connect(m_serialPort.get(), &QSerialPort::readyRead,
            this, &SerialPort::onDataReceived);
    // 使用字符串连接方式兼容 Qt 6 MSVC 编译
    connect(m_serialPort.get(), SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(onError(QSerialPort::SerialPortError)));
}

SerialPort::~SerialPort()
{
    if (m_serialPort && m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
}

QStringList SerialPort::scanAvailablePorts()
{
    QStringList availablePorts;
    const auto portInfos = QSerialPortInfo::availablePorts();

    for (const auto &info : portInfos)
    {
        availablePorts.append(info.portName());
    }

    return availablePorts;
}

QSerialPortInfo SerialPort::getPortInfo(const QString &portName)
{
    const auto portInfos = QSerialPortInfo::availablePorts();
    for (const auto &info : portInfos)
    {
        if (info.portName() == portName)
        {
            return info;
        }
    }
    return QSerialPortInfo();
}

bool SerialPort::open(const QString &portName,
                      qint32 baudRate,
                      QSerialPort::DataBits dataBits,
                      QSerialPort::StopBits stopBits,
                      QSerialPort::Parity parity,
                      QSerialPort::FlowControl flowControl)
{
    if (!m_serialPort)
    {
        qWarning() << "Serial port object is null";
        return false;
    }

    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(dataBits);
    m_serialPort->setStopBits(stopBits);
    m_serialPort->setParity(parity);
    m_serialPort->setFlowControl(flowControl);

    if (!m_serialPort->open(QIODevice::ReadWrite))
    {
        qWarning() << "Failed to open serial port:" << m_serialPort->errorString();
        emit errorOccurred(m_serialPort->errorString());
        return false;
    }

    qInfo() << "Serial port opened successfully:" << portName
            << "BaudRate:" << baudRate;
    emit connectionStatusChanged(true);
    return true;
}

void SerialPort::close()
{
    if (m_serialPort && m_serialPort->isOpen())
    {
        m_serialPort->close();
        qInfo() << "Serial port closed";
        emit connectionStatusChanged(false);
    }
}

bool SerialPort::isOpen() const
{
    return m_serialPort && m_serialPort->isOpen();
}

qint64 SerialPort::write(const QString &data, DataFormat format)
{
    if (!m_serialPort || !m_serialPort->isOpen())
    {
        emit errorOccurred("Serial port is not open");
        return -1;
    }

    QByteArray byteData;

    switch (format)
    {
    case DataFormat::ASCII:
    case DataFormat::UTF8:
        byteData = data.toUtf8();
        break;
    case DataFormat::HEX:
        byteData = hexStringToByteArray(data);
        break;
    }

    qint64 bytesWritten = m_serialPort->write(byteData);
    if (bytesWritten == -1)
    {
        emit errorOccurred("Failed to write data: " + m_serialPort->errorString());
        return -1;
    }

    m_serialPort->waitForBytesWritten(100);
    emit dataSent(data, format);
    return bytesWritten;
}

qint64 SerialPort::writeRaw(const QByteArray &data)
{
    if (!m_serialPort || !m_serialPort->isOpen())
    {
        emit errorOccurred("Serial port is not open");
        return -1;
    }

    qint64 bytesWritten = m_serialPort->write(data);
    if (bytesWritten == -1)
    {
        emit errorOccurred("Failed to write data: " + m_serialPort->errorString());
        return -1;
    }

    m_serialPort->waitForBytesWritten(100);
    return bytesWritten;
}

QByteArray SerialPort::readAll()
{
    if (!m_serialPort)
    {
        return QByteArray();
    }
    return m_serialPort->readAll();
}

QByteArray SerialPort::read(qint64 maxSize)
{
    if (!m_serialPort)
    {
        return QByteArray();
    }
    return m_serialPort->read(maxSize);
}

qint64 SerialPort::bytesAvailable() const
{
    if (!m_serialPort)
    {
        return 0;
    }
    return m_serialPort->bytesAvailable();
}

bool SerialPort::clearRecvBuffer()
{
    if (!m_serialPort)
    {
        return false;
    }
    return m_serialPort->clear(QSerialPort::AllDirections) ||
           m_serialPort->clear(QSerialPort::Input);
}

bool SerialPort::clearSendBuffer()
{
    if (!m_serialPort)
    {
        return false;
    }
    return m_serialPort->clear(QSerialPort::Output);
}

QString SerialPort::portName() const
{
    if (!m_serialPort)
    {
        return QString();
    }
    return m_serialPort->portName();
}

qint32 SerialPort::baudRate() const
{
    if (!m_serialPort)
    {
        return 0;
    }
    return m_serialPort->baudRate();
}

QString SerialPort::errorString() const
{
    if (!m_serialPort)
    {
        return "Serial port object is null";
    }
    return m_serialPort->errorString();
}

QByteArray SerialPort::hexStringToByteArray(const QString &hexString)
{
    QByteArray result;

    // Remove spaces and convert to uppercase
    QString cleanedHex = hexString.simplified().remove(' ').toUpper();

    // Check if the string contains only valid hex characters
    QRegularExpression hexRegex("^[0-9A-F]*$");
    if (!hexRegex.match(cleanedHex).hasMatch())
    {
        qWarning() << "Invalid hex string:" << hexString;
        return result;
    }

    // Convert pairs of hex characters to bytes
    for (int i = 0; i < cleanedHex.length(); i += 2)
    {
        if (i + 1 < cleanedHex.length())
        {
            QString hexByte = cleanedHex.mid(i, 2);
            bool ok = false;
            uint byte = hexByte.toUInt(&ok, 16);
            if (ok)
            {
                result.append(static_cast<char>(byte));
            }
        }
    }

    return result;
}

QString SerialPort::byteArrayToHexString(const QByteArray &data)
{
    QString result;
    for (unsigned char byte : data)
    {
        result.append(QString("%1 ").arg(byte, 2, 16, QChar('0')).toUpper());
    }
    return result.trimmed();
}

void SerialPort::onDataReceived()
{
    if (!m_serialPort)
    {
        return;
    }

    QByteArray data = m_serialPort->readAll();
    if (data.isEmpty())
    {
        return;
    }

    QString displayData;
    switch (m_dataFormat)
    {
    case DataFormat::ASCII:
    case DataFormat::UTF8:
        displayData = QString::fromUtf8(data);
        break;
    case DataFormat::HEX:
        displayData = byteArrayToHexString(data);
        break;
    }

    emit dataReceived(displayData, m_dataFormat);
}

void SerialPort::onError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
    {
        return;
    }

    QString errorMsg;
    switch (error)
    {
    case QSerialPort::DeviceNotFoundError:
        errorMsg = "Device not found";
        break;
    case QSerialPort::PermissionError:
        errorMsg = "Permission denied";
        break;
    case QSerialPort::OpenError:
        errorMsg = "Device already in use";
        break;
    case QSerialPort::WriteError:
        errorMsg = "Write error";
        break;
    case QSerialPort::ReadError:
        errorMsg = "Read error";
        break;
    case QSerialPort::ResourceError:
        errorMsg = "Resource error";
        break;
    case QSerialPort::UnsupportedOperationError:
        errorMsg = "Unsupported operation";
        break;
    case QSerialPort::UnknownError:
        errorMsg = "Unknown error";
        break;
    default:
        errorMsg = "Other error";
    }

    qWarning() << "Serial port error:" << errorMsg;
    emit errorOccurred(errorMsg);
}
