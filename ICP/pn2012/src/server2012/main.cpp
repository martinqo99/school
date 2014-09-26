/**
 * @file: main.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Main function.
*/

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QHostAddress>
#include <QStringList>
#include <QTextCodec>
#include <tcpserver.h>
#include <QString>

/*!
 * Main function.
 * Function take argument - port number.
 * Then the server is start.
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(argc == 2)
    {
        QString portarg = QString::fromStdString(argv[1]);
        bool ok;
        int port = portarg.toInt(&ok,10);
        if(ok)
        {
            TcpServer server;
            server.StartServer(port);
            a.exec();
        }
        else
            qDebug() << "Please, run server with port argument!";
    }
    else
        qDebug() << "Please, run server with port argument!";
    return 0;
}
