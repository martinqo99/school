/**
 * @file: main.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The main function.
 *         Project to ICP at BUT FIT 2011/2012. This application simulate petri nets.
 *         Petri Nets Simulation client.
 *         Some part of the codes are from official qt framework page with examples
 *         and also example from book C++ gui programming with Qt4(graphic chapter).
 */

#include <QtGui/QApplication>
#include "mainwindow.h"

/*!
 * Run the program.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
/*** End of file main.cpp ***/
