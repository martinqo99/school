/**
 * @file: tcpserver.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Tcp server class of petri net.
*/

#include <QDebug>
#include <QDir>
#include <QtNetwork>
#include <QDomElement>

#include "tcpserver.h"

/*!
 * A constructor
 */
TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent)
{
    ///maximum number of pending accepted connections
    this->setMaxPendingConnections(50);

    ///create Sites directory
    QDir dirSites(QCoreApplication::applicationDirPath()+"/Sites");
    if(!dirSites.exists())
        dirSites.mkdir(QCoreApplication::applicationDirPath()+"/Sites");

    ///create Resources directory
    QDir dirResources(QCoreApplication::applicationDirPath()+"/Resources");
    if(!dirResources.exists())
    {
        qDebug() << "Server first start. Creating resources!";
        dirResources.mkdir(QCoreApplication::applicationDirPath()+"/Resources");
    }


//    QFile siteXml(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
//    if(!siteXml.exists())
//        createSiteXml();

    ///copy file from project resources to application directory
    QFile siteXml(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
    if(!siteXml.exists())
        copyFile(":test/examples/SiteXml.xml","/SiteXml.xml");

    ///copy file from project resources to Sites directory
    QFile site(QCoreApplication::applicationDirPath()+"/Sites/test2_1.0.xml");
    if(!site.exists())
    {
        copyFile(":test/examples/Sites/test2_1.0.xml","/Sites/test2_1.0.xml");
        copyFile(":test/examples/Sites/test2_1.1.xml","/Sites/test2_1.1.xml");
        copyFile(":test/examples/Sites/test2_1.2.xml","/Sites/test2_1.2.xml");
        copyFile(":test/examples/Sites/test3_1.0.xml","/Sites/test3_1.0.xml");
        copyFile(":test/examples/Sites/test3_1.1.xml","/Sites/test3_1.1.xml");
        copyFile(":test/examples/Sites/test3_1.2.xml","/Sites/test3_1.2.xml");
    }
    ///create UserXml file
    QFile userXml(QCoreApplication::applicationDirPath()+"/Resources/UserXml.xml");
    if(!userXml.exists())
        createUserXml();
}

/*!
 * Copy file from source path to destination path.
 */
void TcpServer::copyFile(QString source, QString destination)
{
    QDomDocument document;
    QFile file(source);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        document.setContent(&file);
        file.close();
    }

    QFile newfile(QCoreApplication::applicationDirPath()+ destination);
    if (!newfile.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

    QTextStream ts(&newfile);
    ts << document.toString();;
    newfile.close();
}

/*!
 * Start server listenning on the port.
 */
void TcpServer::StartServer(int portnumber)
{
    this->port = portnumber;
    if(!this->listen(QHostAddress::Any,portnumber))
       qDebug() << "Cant run server!";
    else
       qDebug() << "Server is running on port" << portnumber << " ...";
}

/*!
 * Get connection from client and create new thread.
 */
void TcpServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting ...";
    Thread *thread = new Thread(socketDescriptor,this);
    connect(thread, SIGNAL(finished()),thread, SLOT(deleteLater()));
    thread->start();
}

//void TcpServer::createSiteXml()
//{
//    qDebug() << "Creating sitexml....!";
//    QDomDocument doc;

//    QDomNode xmlNode = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
//    doc.insertBefore(xmlNode, doc.firstChild());

//    QDomElement root = doc.createElement( "Sites" );
//    doc.appendChild( root );

//    QFile file(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
//       return;

//    QTextStream ts(&file);
//    ts << doc.toString();;

//    file.close();
//}

/*!
 * Create UserXml file, that will contain the created accounts on the servers.
 */
void TcpServer::createUserXml()
{
    qDebug() << "Creating userxml....!";
    QDomDocument doc;

    QDomNode xmlNode = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());

    QString pass = "admin";
    QString new_pass = QCryptographicHash::hash(pass.toAscii(),QCryptographicHash::Sha1).toHex();

    QDomElement root = doc.createElement( "Server_Users" );
    doc.appendChild( root );
    QDomElement site = doc.createElement("User");
    root.appendChild(site);
    site.setAttribute("login","admin");
    site.setAttribute("password",new_pass);

    QFile file(QCoreApplication::applicationDirPath()+"/Resources/UserXml.xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

    QTextStream ts(&file);
    ts << doc.toString();;

    file.close();
}
