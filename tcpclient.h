#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QDataStream>
class TcpClient : public QObject
{
    Q_OBJECT
public:
    friend class GDataFactory;
    bool connect_to_tcp_server(QString sIp,int sPort);
private:
    void disconnect_from_server();

private:
    explicit TcpClient();
    void timerEvent(QTimerEvent *event);
    int m_timer;
    int m_timerMonitor;

    static TcpClient* _instance;
    static TcpClient* get_instance()
    {
        if(_instance == nullptr)
            _instance = new TcpClient();
        return _instance;
    }

//    QTcpSocket* m_pTcpSocketClient;
    void send_to_tcp_server(QByteArray data);
    QTcpSocket* m_pTcpSocketClient;
    bool m_pTcpConnectSign;

    QQueue<QByteArray> m_pTcpQueue;
    QMutex m_mutex;

    int m_pMonitorCount;
    QByteArray m_pCurrentSN;
    bool m_pAlreadySendSign;
signals:
    void signal_clear_read_sign();
    void signal_query_PLC_sign();
    void signal_send_Mes_info_to_PLC(const QString SN, const short OK_NG, const short standard_num, const short current_num);
public slots:
    void slot_rev_from_tcp_server();
    void slot_send_SN_to_MES(const QByteArray arraySN);

};

#endif // TCPCLIENT_H
