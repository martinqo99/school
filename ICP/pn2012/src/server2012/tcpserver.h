/**
 * @file: tcpserver.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Tcp server class of petri net.
*/

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QDebug>
#include <QList>

#include "thread.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);
    void StartServer(int portnumber);

signals:

public slots:

protected:
    void incomingConnection(int socketDescriptor);

private:
    int port;
    //void createSiteXml();
    void createUserXml();
    void copyFile(QString source, QString destination);


};

#endif // TCPSERVER_H
