#include "tcpclient.h"
#include <QDebug>
#include "QsLog.h"
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QTimerEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "gdatafactory.h"
#include <QTextCodec>
#include <QThread>
#include <QCoreApplication>
TcpClient* TcpClient::_instance = nullptr;
TcpClient::TcpClient()
{
    m_pTcpSocketClient = nullptr;
    m_pMonitorCount = 0;
    m_pAlreadySendSign = false;
}

int exitSign =0;
void TcpClient::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timer)
    {
        emit signal_query_PLC_sign();
//        killTimer(m_timer);
    }
    else if(event->timerId() == m_timerMonitor)
    {
        m_pMonitorCount++;
        if(m_pMonitorCount == 5)
        {
            exitSign++;
            disconnect_from_server();
            killTimer(m_timer);
            m_pMonitorCount = 0;
            QLOG_ERROR()<<u8"第"<<QString::number(exitSign)<<u8"次超时重连!";
            connect_to_tcp_server(GDataFactory::get_instance()->get_config_para("MES_IP"),GDataFactory::get_instance()->get_config_para("MES_PORT").toInt());
        }
    }
    else {
        ;
    }
}

bool TcpClient::connect_to_tcp_server(QString sIp, int sPort)
{
    if(m_pTcpSocketClient == nullptr)
        m_pTcpSocketClient = new QTcpSocket(this);
    if(m_pTcpSocketClient->state() != QAbstractSocket::UnconnectedState)
    {
        QLOG_ERROR()<<"the connect state is not UnconnectedState, cannot connect to server!";
        return false;
    }

    m_pTcpSocketClient->connectToHost(sIp,sPort);
    if(m_pTcpSocketClient->waitForConnected(1000))
    {
        connect(m_pTcpSocketClient,&QTcpSocket::readyRead,this,&TcpClient::slot_rev_from_tcp_server);
        QLOG_WARN()<<u8"connect to MES tcp server SUCCESS!";
        m_pAlreadySendSign = false;
        m_timer = startTimer(1000);
        exitSign = 0;
        return true;
    }
    else {
        QLOG_ERROR()<<u8"connect to MES tcp server FAILED!";
        return false;
    }
}

void TcpClient::disconnect_from_server()
{
    m_pTcpSocketClient->disconnectFromHost();
    QThread::msleep(300);
    QLOG_WARN()<<m_pTcpSocketClient->state();
    if(m_pTcpSocketClient->state() == QAbstractSocket::UnconnectedState)
    {
        disconnect(m_pTcpSocketClient,&QTcpSocket::readyRead,this,&TcpClient::slot_rev_from_tcp_server);
        QLOG_INFO()<<"the client disconnect from server SUCCESS!";
    }
    else
        QLOG_ERROR()<<"the client disconnect from server FAILED!";
}

void TcpClient::send_to_tcp_server(QByteArray data)
{
    QJsonObject  root;
    root.insert("ProductSn",QString(data));
    QLOG_WARN()<<u8"发送至MES的SN信息:"<<QString(data);

    QJsonDocument doc_data(root);
    QByteArray request_data = doc_data.toJson(QJsonDocument::Compact);
    QLOG_WARN()<<u8"发送至MES的JSON信息:"<<QString(request_data);

    m_pTcpSocketClient->write(request_data,request_data.length());
    m_pTcpSocketClient->flush();
}

void TcpClient::slot_rev_from_tcp_server()
{
    emit signal_clear_read_sign();
    m_pAlreadySendSign = false;
    killTimer(m_timerMonitor);
    m_pMonitorCount = 0;

    QJsonParseError jError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(m_pTcpSocketClient->readAll(),&jError);
    QJsonObject jsonObject = jsonDocument.object();

    QLOG_WARN()<<jsonObject.value("ProductSn").toString();
    QLOG_WARN()<<jsonObject.value("ResultCode").toString();
    QLOG_WARN()<<jsonObject.value("ErrorMessage").toString();
    QLOG_WARN()<<jsonObject.value("StandNum").toString();
    QLOG_WARN()<<jsonObject.value("PackNum").toString();

    short rc;
    if(jsonObject.value("ResultCode").toString() == "OK")
        rc = 1;
    else if(jsonObject.value("ResultCode").toString() == "NG")
        rc = 2;
    else
    {
        QLOG_ERROR()<<"the message from Mes exist ERROR!";
        return;
    }

    short a,b;
    a = jsonObject.value("StandNum").toString().toShort();
    b = jsonObject.value("PackNum").toString().toShort();
    emit signal_send_Mes_info_to_PLC(jsonObject.value("ProductSn").toString(),rc,a,b);
}

void TcpClient::slot_send_SN_to_MES(const QByteArray arraySN)
{
    if(m_pAlreadySendSign == false)
        m_pAlreadySendSign = true;
    else
        return;
    m_pCurrentSN = arraySN;
    m_timerMonitor = startTimer(1000);
    send_to_tcp_server(arraySN);
}
