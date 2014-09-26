/**
 * @file: users.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Users class of petri net.
*/

#ifndef USERS_H
#define USERS_H

#include <QDebug>

class Users
{
public:
    Users();
    QString login;  ///login
    QString password; ///password
};

#endif // USERS_H
