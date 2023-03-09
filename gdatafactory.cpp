#include "gdatafactory.h"
#include "tcpclient.h"
#include <QDebug>
#include <QFile>
#include "QsLog.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

GDataFactory* GDataFactory::_instance = nullptr;
void GDataFactory::load_json_config(char *file_name)
{
    QFile file(file_name);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QLOG_INFO()<<"load json config file success!";
        JsonValue = file.readAll();
        file.close();

        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(JsonValue.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_INFO()<<"parse json file failed!";
            return;
        }
        else {
            QLOG_INFO()<<"parse json file success!";
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains("IPConfigs"))
        {
            QJsonValue arrayValue = jsonObject.value(QStringLiteral("IPConfigs"));
            if(arrayValue.isArray())
            {
                QJsonArray array = arrayValue.toArray();
                for (int i=0;i<array.size();i++)
                {
                    QJsonValue iconArray = array.at(i);
                    QJsonObject icon = iconArray.toObject();
                    ConfigInfo.insert("MES_IP",icon["MES_IP"].toString());
                    ConfigInfo.insert("PLC_IP",icon["PLC_IP"].toString());
                }
            }
        }

        if(jsonObject.contains("PortConfigs"))
        {
            QJsonValue portValue = jsonObject.value(QStringLiteral("PortConfigs"));
            if(portValue.isObject())
            {
                QJsonObject obj = portValue.toObject();
                if(obj.contains("MES_PORT"))
                    ConfigInfo.insert("MES_PORT",obj.value("MES_PORT").toString());
                if(obj.contains("PLC_PORT"))
                    ConfigInfo.insert("PLC_PORT",obj.value("PLC_PORT").toString());
                if(obj.contains("LOG_LEVEL"))
                    ConfigInfo.insert("LOG_LEVEL",obj.value("LOG_LEVEL").toString());
            }
        }

    }
    else
    {
        QLOG_INFO()<<"load json config file failed!";
        return;
    }

    file.flush();
    file.close();
}

QString GDataFactory::get_config_para(const QString keyname)
{
    return ConfigInfo[keyname];
}

QString GDataFactory::bytes_to_str(QByteArray data)
{
    QString str_data;
    str_data.clear();
    quint8 n_data = 0;
    for(int i=0; i<data.size(); i++)
    {
        n_data = static_cast<quint8>(data[i]) ;
        if(n_data < 16) str_data.append('0');
        str_data.append(QString::number(n_data, 16));
        str_data.append(' ');
    }
    return str_data;
}

void GDataFactory::connections_initialization()
{
    connect(get_tcp_client(),SIGNAL(signal_query_PLC_sign()),get_udp_client(),SLOT(slot_query_PLC_sign()));
    connect(get_udp_client(),SIGNAL(signal_send_SN_to_MES(const QByteArray)),get_tcp_client(),SLOT(slot_send_SN_to_MES(const QByteArray)));
    connect(get_tcp_client(),SIGNAL(signal_send_Mes_info_to_PLC(const QString,const short,const short,const short)),
            get_udp_client(),SLOT(slot_send_Mes_info_to_PLC(const QString,const short,const short,const short)));
    connect(get_tcp_client(),SIGNAL(signal_clear_read_sign()),get_udp_client(),SLOT(slot_clear_read_sign()));
}

quint8 GDataFactory::get_last_ip(const int id)
{
    if (id == 0)
    {
        QString localHostName = QHostInfo::localHostName();
        QString localIP = "";
        QHostInfo info = QHostInfo::fromName(localHostName);
        foreach(QHostAddress addr,info.addresses())
            if(addr.protocol() == QAbstractSocket::IPv4Protocol)
            {
                localIP = addr.toString();
            }
        return process_ip_string(localIP);
    }
    else
    {
        return process_ip_string(get_config_para("PLC_IP"));
    }
}

quint8 GDataFactory::process_ip_string(const QString ipstr)
{
    int nSize = ipstr.size();
    int nCount = 0;
    QString mData;
    mData.clear();
    QChar mChar;
    for(int idx = 0; idx<nSize; idx++)
    {
        mChar = ipstr[idx];
        if(mChar == '.')
        {
            nCount++;
        }
        else if(mChar != '.' && nCount == 3 && (mChar >= '0' || mChar <= '9'))
        {
            mData.append(mChar);
        }
    }
    return static_cast<quint8>(mData.toInt());
}

GDataFactory::GDataFactory(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QByteArray>("QByteArray");
}
