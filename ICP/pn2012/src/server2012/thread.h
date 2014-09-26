/**
 * @file: thread.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Thread class of petri net.
*/

#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

#include "place.h"
#include "transition.h"
#include "arc.h"
#include "users.h"
#include "simulation.h"

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(int ID, QObject *parent = 0);
    void run();
    void loadUsers();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void handlyReplay();
    void disconnected();

private:
    QString transitionStep;
    QTcpSocket *socket;
    int socketDescriptor;
    QHash<QTcpSocket*, QString> logins;  ///hash table
    int checkUsers( QString login, QString password);
    void socketWrite(QTcpSocket* socket, QByteArray data);
    void saveFile(QByteArray filetext, QString filename, QString version);
    void versionFile(QByteArray filetext, QString filename, QString version);
    QList <Users *> list;   ///list of users
    void sendXML(QString filename, QString command);
    void simulation(int step,QString file);
    void addToSiteXml(QString nameAuthor, QString nameSite, QString versionSite, QString description);
    void addToUserXml(QString user, QString user_password);
    void stopsimulation();
    Simulation *activSimulation;  ///simulation
    int simulinprogress;  ///simulation progress
    int checkLogin(QString user);
    QString nameFile;  ///name of file
    QString versionSite; ///version of file

    void addToSiteXmlSimulation(QString nameSite, QString versionSite, QString date, QString user);
};

#endif // THREAD_H
