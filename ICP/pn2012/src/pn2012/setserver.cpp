/**
 * @file: setserver.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Set server dialog window.
 */
#include "setserver.h"
#include "ui_setserver.h"
#include <QMessageBox>
#include <QHostAddress>

/*!
 * Constructor, set server and init ui lineedits with default variables.
 */
SetServer::SetServer(QString serv,QString log, QString pass, int port,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetServer)
{
    ui->setupUi(this);
    this->setFixedSize(380,150);
    parentwidget = dynamic_cast<MainWindow *>(parent);
    server = "";
    ui->lineEdit_2->setText(serv);
    ui->lineEdit_3->setText(log);
    ui->lineEdit->setText(QString::number(port));
    ui->lineEdit_4->setText(pass);
}

/*!
 * Destructor.
 */
SetServer::~SetServer()
{
    delete ui;
}

/*!
 * Close button.
 */
void SetServer::on_pushButton_2_clicked()
{
    parentwidget->appmodepossible = 0;
    parentwidget->connected = 0;
    this->close();
}

/*!
 *  Accept button, try connect to server.
 */
void SetServer::on_pushButton_clicked()
{

    server = ui->lineEdit_2->text();
    QString port_string = ui->lineEdit->text();

    login = ui->lineEdit_3->text();
    password = ui->lineEdit_4->text();

    QHostAddress addr;
//    // check ip addres
//    if(!addr.setAddress(server))
//    {
//            QMessageBox::warning(this, tr("Bad ip address"),
//                                  tr("You enetered ipaddress in bad format!"));
//            ui->lineEdit_2->setText("");
//    }

    bool ok;
    port = port_string.toInt(&ok, 10);
    if(!ok)
    {
        QMessageBox::warning(this,"Error with port","Set port number bigger than 1023!");
        ui->lineEdit->setText("");
       //return;
    }
    else
    {
        /// set mainwindow params
        parentwidget->appmodepossible = 1;
        parentwidget->set_server_port(this->server, this->port, this->login, this->password);
        parentwidget->connected = 1;
        parentwidget->doConnect("USERVAL");
        this->close();
    }
}
/*** End of fiel setserver.cpp ***/
