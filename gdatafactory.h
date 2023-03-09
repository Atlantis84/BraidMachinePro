#ifndef GDATAFACTORY_H
#define GDATAFACTORY_H

#include <QObject>
#include "tcpclient.h"
#include "udpservice.h"
#include <QHostInfo>
#define READ_PLC  0x01 //function code of read plc
#define WRITE_PLC 0x02 //function code of write plc

#define ADDRESS_D10000 0x2710
#define ADDRESS_D10010 0x271A

#define ADDRESS_D10020 0x2724
class GDataFactory : public QObject
{
    Q_OBJECT

private:
    explicit GDataFactory(QObject *parent = nullptr);


    static GDataFactory* _instance;
    QMap<QString,QString> ConfigInfo;
    QString JsonValue;

    quint8 process_ip_string(const QString ipstr);

public:

    static GDataFactory* get_instance()
    {
        if(_instance == nullptr)
            _instance = new GDataFactory();
        return _instance;
    }

    TcpClient* get_tcp_client()
    {
        return TcpClient::get_instance();
    }

    UdpService* get_udp_client()
    {
        return UdpService::get_instance();
    }

    void load_json_config(char* file_name);
    QString get_config_para(const QString keyname);

    QString bytes_to_str(QByteArray data);
    void connections_initialization();
    quint8 get_last_ip(const int id);
signals:

public slots:
};

#endif // GDATAFACTORY_H
