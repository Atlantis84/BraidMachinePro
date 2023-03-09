#include <QCoreApplication>
#include "gdatafactory.h"
#include "QsLog.h"
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
using namespace QsLogging;
#include <QProcess>
#include <QDebug>
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
void rev_log(const QString &strMsg,Level level)
{
    //test git
    if(level == 0)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
        qDebug()<<strMsg.right(strMsg.length()-strMsg.indexOf(">")-1);
    }
    else if(level == 1)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
        qDebug()<<strMsg.right(strMsg.length()-strMsg.indexOf(">")-1);
    }
    else if(level == 2)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
        qDebug()<<strMsg.right(strMsg.length()-strMsg.indexOf(">")-1);
    }
    else if(level == 3)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
        qDebug()<<strMsg.right(strMsg.length()-strMsg.indexOf(">")-1);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    else{
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
        qDebug()<<strMsg.right(strMsg.length()-strMsg.indexOf(">")-1);
    }
}

int print_function_menu()
{
    std::cout<<"***************************************************\n";
    std::cout<<"    1. start tcp client.\n";
    std::cout<<"    2. send data to tcp server.\n";
    std::cout<<"    3. disconnect from tcp server.\n";
    std::cout<<"    4. read gbb data.\n";
    std::cout<<"    5. exit data manager.\n";
    std::cout<<"    6. start/stop store offline data. \n";
    std::cout<<"    7. start/stop load offline data. \n";
    std::cout<<"    0. exit.\n";
    std::cout<<"---->Input index:";

    int idx;
    std::cin>>idx;
    return idx;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Logger& logger = Logger::instance();
    logger.setLoggingLevel(QsLogging::TraceLevel);

    const QString sLogPath(QDir(QCoreApplication::applicationDirPath()).filePath("log.txt"));
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(sLogPath, EnableLogRotation, MaxSizeBytes(512*1024), MaxOldLogCount(5)));
    logger.addDestination(fileDestination);

    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination);

    DestinationPtr controlDestination(DestinationFactory::MakeFunctorDestination(&rev_log));
    logger.addDestination(controlDestination);

    GDataFactory::get_instance()->load_json_config(QByteArray(QCoreApplication::applicationDirPath().append("/config.json").toLatin1()).data());
    GDataFactory::get_instance()->connections_initialization();
    GDataFactory::get_instance()->get_tcp_client()->connect_to_tcp_server(GDataFactory::get_instance()->get_config_para("MES_IP"),GDataFactory::get_instance()->get_config_para("MES_PORT").toInt());

    return a.exec();
}
