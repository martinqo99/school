/**
 * @file: thread.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Thread class of petri net.
*/

#include <QDebug>
#include <QtXml>
#include <QXmlStreamWriter>
#include <QDomElement>
#include <QIODevice>
#include <Qt>
#include <QtNetwork>

#include "thread.h"
#include "tcpserver.h"

/*!
 * A constructor.
 */
Thread::Thread(int ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    simulinprogress = 0;
}

/*!
 * Run thread.
 * Wait for message from client or disconnect from the client.
 */
void Thread::run()
{
    qDebug()<< socketDescriptor << "Starting thread!";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket,SIGNAL(readyRead()),this,SLOT(handlyReplay()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);

    qDebug()<< socketDescriptor << "Client conneted!";

    exec();
}

/*!
 * Receives a messages from a client and respond to, according to the command.
 */
void Thread::handlyReplay()
{
    /// data
    QByteArray rawdata = socket->readAll();
    ///arguments requirement
    QList<QByteArray> args = rawdata.split('^');
    ///command
    QString command = args.takeFirst();
    ///login
    QString login =  args.takeFirst();


    qDebug() << socketDescriptor << "RAWDATA:" << rawdata;

    ///user control
    if(command == "USERVAL")
    {      
        int  error;
        QString password =  args.takeFirst();
        error = checkUsers(login, password);
        return;
    }
    ///add new user
    else if(command == "USERNEW")
    {
        int  error;
        QString password =  args.takeFirst();
        error = checkLogin(login);
        if(!error)
        {
           addToUserXml(login,password);
        }
    }
    /// save xml file to server
    else if(command == "SAVEXML")
    {
        QString nameAuthor = login;
        QString nameSite = args.takeFirst();
        versionSite = args.takeFirst();
        QString description = args.takeFirst();

        nameFile =nameSite + "_" + versionSite +".xml";

        /// save xml file
        versionFile(args.takeFirst(),nameFile,versionSite);

        /// add info about xml file to SiteXml
        addToSiteXml(nameAuthor,nameFile,versionSite,description);
    }
    /// send xml file from server to client
    else if(command == "LOADXML")
    {
        /// sends a file to the client the choice of the client
        sendXML(args.takeFirst(),"LOADXML^%1^");
    }
    else if(command == "SITEXML")
    {
        /// sends a file containing all information about networks of servers
        sendXML("SiteXml.xml","SITEXML^");
    }
    /// do simulation
    else if(command == "SIMULATION")
    {
        QString tab = args.takeFirst();
        QString version =args.takeFirst();
        QString step = args.takeFirst();
        int value = step.toInt(0,10);
        if(!args.isEmpty())
            transitionStep = args.takeFirst();
        else transitionStep = "";

        QString date =args.takeFirst();

        addToSiteXmlSimulation(tab, version, date, login);

        simulation(value,tab);
    }
    /// stop simulation
    else if(command == "SIMULATIONSTOP")
    {
        stopsimulation();
    }
    else
        qDebug() << socketDescriptor << "Command from client not recognized!";
}

/*!
 * Update info about simulation in SiteXml file.
 */
void Thread::addToSiteXmlSimulation(QString nameSite, QString versionSite, QString date, QString user)
{
    QFile file(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
    QDomDocument doc;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
    }
    else
    {
        qDebug() << "Error with opening file!";
        return;
    }

    file.close();

    QDomElement root = doc.documentElement();
    if( root.tagName() != "Sites" )
    {
        qDebug() << "Bad format XML file!";
        return;
    }

    QDomNode n = root.firstChild();

    while( !n.isNull() )
    {
      QDomElement e = n.firstChildElement();
      e = e.nextSiblingElement("Site");
      if(e.tagName() == "Site")
      {
         if(e.text() == nameSite)
         {
            e = e.nextSiblingElement("Version");
            if(e.text() == versionSite)
            {
               e = e.nextSiblingElement("Time_Simulation");

               QDomElement simulationTime = doc.createElement("Time_Simulation");
               n.appendChild(simulationTime);
               simulationTime.appendChild(doc.createTextNode(date));
               n.replaceChild(simulationTime,e);

               e = n.firstChildElement();
               e = e.nextSiblingElement("Login_Simulation");

               QDomElement simulationLogin = doc.createElement("Login_Simulation");
               n.appendChild(simulationLogin);
               simulationLogin.appendChild(doc.createTextNode(user));
               n.replaceChild(simulationLogin,e);
            }
         }
      }
      n = n.nextSibling();
    }

    file.remove();

    QFile file1(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

    QTextStream ts( &file1 );
    ts << doc.toString();;

    file1.close();
}

/*!
 * Reload xml file that is stored on the server and send it to the client.
 */
void Thread::sendXML(QString filename,QString command)
{
    /// send SiteXml file
    if(filename == "SiteXml.xml")
    {
        QFile inputFile(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
        QByteArray read;
        socket->write("SITEXML^");
        /// QTcpSocket client;
        if(!(inputFile.open(QIODevice::ReadOnly)))
        {
            qDebug() << "Error with opening file!";
            return;
        }
        read.clear();
        read = inputFile.readAll();

        if(read.size()==0)
        return;

        socket->write(read);
        socket->waitForBytesWritten();
        read.clear();
        inputFile.close();
    }
    /// send a file which the user chose
    else
    {
        qDebug() << filename;
        QFile inputFile(QCoreApplication::applicationDirPath()+"/Sites/"+filename);

        QByteArray read;

        QByteArray request;
        request += QString(command).arg(filename);
        socket->write(request);

        if(!(inputFile.open(QIODevice::ReadOnly)))
        {
            qDebug() << "Error with opening file!";
            return;
        }

        read.clear();
        read = inputFile.readAll();

        if(read.size()==0)
        return;

        socket->write(read);
        socket->waitForBytesWritten();
        read.clear();
        inputFile.close();
    }
}

/*!
 * Do simulation chosen petri net.
 */
void Thread::simulation(int step,QString filename)
{
    QString stepresult;
    qDebug() << socketDescriptor << "Simulation...";
    if(!simulinprogress)
    {
        QFile file(QCoreApplication::applicationDirPath()+"/Sites/"+filename);
        QDomDocument doc;
        /// open file
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            if(!doc.setContent(&file))
            {
                file.close();
                qDebug() << socketDescriptor << "Error reading file.";
                return;
            }
        }
        else
        {
            qDebug() << socketDescriptor << "Error reading file.";
            return;
        }

        qDebug() << socketDescriptor << "Creating simulation class";
        activSimulation = new Simulation(doc,socketDescriptor);
        simulinprogress = 1;
        file.close();
    }
    else
        qDebug() << "simulation in progress";

    /// set transition step
    activSimulation->setTransitionStep(transitionStep);
    int state;

    /// call up the simulation class
    QByteArray response;
    if(activSimulation->doCheck())
    {
        /// if petri net is not in good format
        delete activSimulation;
        simulinprogress = 0;
        response += QString("SIMULATIONERROR^");
        this->socketWrite(socket,response);
        socket->waitForBytesWritten();
        return;
    }
    else
        stepresult.append(activSimulation->getNextStep(step,&state));

    /// if is full simulation
    if(step == 0)
    {
        delete activSimulation;
        simulinprogress = 0;
    }

    /// send it
    /// state => 1 == error | 0 == ok | 2 == nothing to do
    if(state == 1)
        response += QString("SIMULATIONERROR^");
    else
        response += QString("SIMULATION^%1^").arg(stepresult);
    this->socketWrite(socket,response);
    socket->waitForBytesWritten();
    qDebug() << socketDescriptor << "Simulation completed.";
}

/*!
 * Check whether a file has been stored on the server.
 * If file exists, the file will be save as a new version.
 */
void Thread::versionFile(QByteArray filetext, QString filename, QString version)
{
    QFile file(QCoreApplication::applicationDirPath()+"/Sites/"+filename);
    if(file.exists())
    {
        filename.truncate(filename.length() - 8);
        double old_version = version.toDouble();

        double new_version = old_version + 0.1;
        if(version.contains(".9"))
        {
            versionSite = QString::number(new_version);
            versionSite = versionSite +".0";
        }
        else
        {
            versionSite = QString::number(new_version);
        }
        nameFile = filename + "_" + versionSite + ".xml";
        QFile file(QCoreApplication::applicationDirPath()+"/Sites/"+nameFile);
        if(file.exists())
        {
           versionFile(filetext,nameFile,versionSite);
        }
        else
        {
          saveFile(filetext,nameFile,versionSite);
        }

    }
    else
    {
        saveFile(filetext,filename,versionSite);
    }
}

/*!
 * Save file from the client on the server.
 */
void Thread::saveFile(QByteArray filetext, QString filename, QString version)
{
    QFile file(QCoreApplication::applicationDirPath()+"/Sites/"+filename);
    if(!(file.open(QIODevice::WriteOnly| QIODevice::Text)))
    {
       qDebug("Can not open file!");
       return;
    }
    file.write(filetext);
    file.close();
}

/*!
 * Login control.
 */
int Thread::checkUsers(QString login, QString password)
{
    loadUsers();

    QByteArray request;
    for(int i=0; i < list.length(); i++)
    {
        if(list[i]->login == login)
        {
            if(list[i]->password != password)
            {
               qDebug() << "Bad password.";
               request = "BADPSWD^";
               this->socketWrite(socket,request);
               return 1;
            }
            qDebug() << socketDescriptor << "User" << list[i]->login << "exist.";
            break;
        }
        else
        {
            if(i == list.length()-1)
            {
                qDebug() << "User does not exist.";
                request = "BADUSER^";
                this->socketWrite(socket,request);
                return 1;
            }
        }
    }
    return 0;
}

/*!
 * Send message to client.
 */
void Thread::socketWrite(QTcpSocket* socket, QByteArray data)
{
    socket->write(data);
}

/*!
 * Load information about user from UserXml file.
 * Load login and password.
 */
void Thread::loadUsers()
{
    QFile file(QCoreApplication::applicationDirPath()+"/Resources/UserXml.xml");
    QDomDocument doc;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        if(!doc.setContent(&file))
        {
            file.close();
            qDebug() << "Error reading file";
            return;
        }
    }
    else
    {
        qDebug() << "Error reading file";
        return;
    }
    QDomElement root = doc.documentElement();
    if( root.tagName() != "Server_Users" )
    {
        qDebug() << "Wrong format xml file!";
        return;
    }
    else
    {
        QDomNode n = root.firstChild();
        while( !n.isNull() )
        {
            QDomElement e = n.toElement();
            if( !e.isNull() )
            {
                if(e.tagName() == "User")
                {
                    Users *user = new Users();
                    user->login = e.attribute("login","");
                    user->password = e.attribute("password","");
                    list.append(user);
                }
            }
            n = n.nextSibling();
        }
    }
    file.close();
}

/*!
 * Diconnect from client.
 */
void Thread::disconnected()
{
    qDebug()<< socketDescriptor << "Disconected!";
    socket->deleteLater();
    return;
}

/*!
 * Update information about the saved file on the server.
 * Information is added to SiteXml file.
 */
void Thread::addToSiteXml(QString nameAuthor, QString nameSite, QString versionSite, QString description)
{
    QFile file(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
    QDomDocument doc;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
    }
    else
    {
        return;
    }

    file.close();

    QDomElement root = doc.documentElement();

    if( root.tagName() != "Sites" )
    {
        qDebug()<< "Wrong format xml file!";
        return;
    }

    QDomElement site = doc.createElement("Site");
    root.appendChild(site);

    QDomElement author = doc.createElement("Author");
    site.appendChild(author);
    author.appendChild(doc.createTextNode(nameAuthor));

    QDomElement siteName = doc.createElement("Site");
    site.appendChild(siteName);
    siteName.appendChild(doc.createTextNode(nameSite));

    QDomElement version = doc.createElement("Version");
    site.appendChild(version);
    version.appendChild(doc.createTextNode(versionSite));

    QDomElement simulationTime = doc.createElement("Time_Simulation");
    site.appendChild(simulationTime);
    simulationTime.appendChild(doc.createTextNode(""));

    QDomElement simulationLogin = doc.createElement("Login_Simulation");
    site.appendChild(simulationLogin);
    simulationLogin.appendChild(doc.createTextNode(""));

    QDomElement info = doc.createElement("Description");
    site.appendChild(info);
    info.appendChild(doc.createTextNode(description));

    file.remove();

    QFile file1(QCoreApplication::applicationDirPath()+"/SiteXml.xml");
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<< "subor file 1";
       return;
    }

    QTextStream ts( &file1 );
    ts << doc.toString();;

    file1.close();
}

/*!
 * Add new user information - login and password to UserXml file.
 */
void Thread::addToUserXml(QString user, QString user_password)
{

    QFile file(QCoreApplication::applicationDirPath()+"/Resources/UserXml.xml");
    QDomDocument doc;
    if(file.open(QIODevice::ReadOnly))
    {
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
    }
    else
        return;

    file.close();

    QDomElement root = doc.documentElement();

    if( root.tagName() != "Server_Users" )
    {
        qDebug()<< "Wrong format xml file!";
        return;
    }

    QDomElement site = doc.createElement("User");
    root.appendChild(site);
    site.setAttribute("login",user.replace(" ","_"));
    site.setAttribute("password",user_password.replace(" ","_"));

    QFile file1(QCoreApplication::applicationDirPath()+"/Resources/UserXml.xml");
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

    QTextStream ts( &file1 );
    ts << doc.toString();;

    file1.close();

    QByteArray request;
    request = "USERADD^";
    this->socketWrite(socket,request);
}

/*!
 * Stop simulation.
 */
void Thread::stopsimulation()
{
    if(simulinprogress == 1)
    {
        delete activSimulation;
        simulinprogress = 0;
    }
}

/*!
 * To create an account check whether the user does not have an account.
 */
int Thread::checkLogin(QString user)
{
    loadUsers();

    QByteArray request;
    for(int i=0; i < list.length(); i++)
    {
        if(list[i]->login == user)
        {

            request = "USEREXIST^";
            this->socketWrite(socket,request);
            return 1;
        }
    }
    return 0;
}
