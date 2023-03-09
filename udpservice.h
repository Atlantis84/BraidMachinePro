#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <QMutex>
class UdpService : public QThread
{
    Q_OBJECT
public:
    friend class GDataFactory;
private:
    static UdpService* get_instance()
    {
        if(_instance == nullptr)
            _instance = new UdpService();
        return _instance;
    }

    void release_res();
    void send_message_to_plc(const uchar function_code, const short data_address,const short data_length/*length is word length*/,const QByteArray data_array);
private:
    UdpService();
    ~UdpService();
    static UdpService* _instance;
    QUdpSocket *m_pUdpSocket;
    QByteArray make_plc_message(const uchar function_code, const short data_address,const short data_length/*length is word length*/,const QByteArray data_array);
    QMutex m_mutex;

signals:
    void signal_proc_plc_msg(const QByteArray data_array);
    void signal_send_SN_to_MES(const QByteArray arraySN);
    void signal_proc_real_data(QByteArray data);
public slots:
    void slot_clear_read_sign();
    void slot_proc_real_data(QByteArray data);
    void slot_receive_message();
    void slot_proc_plc_msg(const QByteArray data_array);
    void slot_query_PLC_sign();
    void slot_send_Mes_info_to_PLC(const QString SN, const short OK_NG, const short standard_num, const short current_num);
};

#endif // UDPSERVICE_H
