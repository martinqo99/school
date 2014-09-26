/**
 * @file: mainwindow.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: Header file for mainwindow.cpp.
 *         Some part of the codes are from official qt framework page with examples
 *         and also example from book C++ gui programming with Qt4(graphic chapter).
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QStandardItemModel>
#include <QList>
#include <QItemSelection>
#include <QTimer>
#include <QGridLayout>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsItem>
#include <QDomElement>
#include <QtXml>
#include <QTcpSocket>

#include "about.h"
#include "searchdialog.h"
#include "place.h"
#include "customgraphicsscene.h"
#include "setserver.h"

#define SIMULATION 1
#define EDITING 2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void setColors(QColor tratout,QColor tratext, QColor traback,QColor platext, QColor plaout, QColor plaback, QColor arccol,QString font,QString shape);
    int appmodepossible;
    int resultDialog;
    int connected;
    void doConnect(QString command);
    void set_server_port(QString server, int port , QString login, QString password);
    void set_fileoptions(QString namesite, QString versionsite, QString discription);
    void set_user(QString user, QString user_password);
    ~MainWindow();
    QString filename;



private slots:
    /// quit application slot
    void on_action_Quit_triggered();
    /// new tab/net
    void on_actionNew_net_triggered();
    /// tab changed
    void tabChanged(int index);
    /// graphics tabs
    void removeTab(int);
    /// treeview
    void selectionChangedSlot(const QItemSelection &, const QItemSelection &);
    /// about dialog
    void on_actionAbout_triggered();
    /// search dialog
    void on_actionSearch_nets_triggered();
    /// create new place
    void on_actionPlace_triggered();
    /// create new transition
    void on_actionTransition_triggered();
    /// connect place and transition with arc
    void on_actionAdd_arc_triggered();
    /// remove selected items
    void on_actionRemove_triggered();
    /// properties dialog
    void on_properties_mode_triggered();
    /// visual options dialog
    void on_actionVisual_options_triggered();
    /// context menu on right click
    void contextMenu(const QPoint &pos);
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSet_server_triggered();
    void sendXML();
    void gotError(QAbstractSocket::SocketError error);
    void handleReply();
    void on_actionSimulation_triggered();
    void on_actionEditing_triggered();
    void on_actionSimulate_triggered();
    void on_actionStop_triggered();
    void on_actionPrevious_step_triggered();
    void searchSignalHandler();
    void on_actionNew_user_triggered();
    void on_actionHelp_triggered();
    void on_actionShow_result_triggered();
    void on_actionExport_to_pdf_triggered();

private:
    QDomElement ContactToNodePlace (QDomDocument&,QString name, QString tokens, double x, double y);
    QDomElement ContactToNodeTransition (QDomDocument&,QString name, QString condition, QString operation, double x, double y);
    QDomElement ContactToNodeArc (QDomDocument&, int direction, QString name, QString uniqname, QString place, QString transition);
    void saveFiles(QString);
    void addLoadPlace(QString name, QString tokens, double x, double y);
    void addLoadTransition(QString name, double x, double y, QString condition, QString operation);
    void addLoadArc(QString name, QString uniqname, int direction, QString endplace, QString endtransition);
    void setupPlaceLoad(Place *place);
    void setupTransitionLoad(Transition *transition);
    void new_tab(QString name_site,QString version_site);
    void socketWrite(QByteArray data);
    void setActions(int mode);
    void loadNetToView(QString net, int mode);
    /// pair of Place and Transition
    typedef QPair<Place *, Transition *> SelectedPair;
    Ui::MainWindow *ui;
    /// return selected transition/place/arc from graphics scene
    Transition *selectedTransition();
    Place *selectedPlace();
    Arc *selectedArc();
    void refreshTreeView();
    void appendItemToTreeView(QGraphicsItem *item);
    /// load colors and visual options from file
    void loadColors();
    void initMainWindow();
    void initTreeView();
    void initSignals();
    void initStatusText();
    /// do simulation
    void doSimulation(int simulation);
    /// graphics, tabs, tabwidget, petrinets
    QList<QGraphicsView *> graphicslist;
    QList<CustomGraphicsScene *> scenelist;
    QList<int> seqnumberlist;
    QStringList nametablist;
    QStringList versionlist;
    int graphicstabscount;
    int graphicstabsindex;
    /// actual index of tab
    int activetabindex;
    /// tree view variables
    QStandardItemModel *treemodel;
    QStandardItem *rootNode;
    QStandardItem *placesItem;
    QStandardItem *transitionsItem;
    QStandardItem *arcsItem;
    /// simulation
    QTimer *timer;
    /// new windows, dialogs
    About *about;
    /// setup Place
    void setupPlace(Place *place);
    /// setup Transition
    void setupTransition(Transition *transition);
    /// get pair Transition-Place
    SelectedPair selectedTransPlacePair() const;
    /// colors, set colors
    QColor transbackground;
    QColor transtext;
    QColor transoutline;
    QColor placebackground;
    QColor placeoutline;
    QColor placetext;
    QColor arccolor;
    QString server;
    /// port to connect on server
    int port;
    /// user properities
    QString login;
    QString password;
    QString user;
    QString user_password;
    /// connection
    QTcpSocket* socket;
    void saveFileFromServer(QByteArray filetext, QString filename);
    QString namesite_m;
    QString versionsite_m;
    QString discription_m;
    /// Application mode => SIMULATION/EDITING
    int appmode;
    /// 0 full simulation, 1 represent step forwards, 2 step backwards
    int simulstep;
    /// if is simulation in progress
    int simulinprogress;
    /// transition name for simulation step
    QString transitionSim;
    QList<int> tab_list_check;
    /// simulation Step result
    void simulationStep(QString result);
    /// refresh net from server
    void refreshFromServer();
    /// if is animation => maybe implemented in future
    int animateSimulation;
    /// type of font
    int font;
    /// type of shape of place
    int shape;
signals:
    void searchSignal();
};

#endif // MAINWINDOW_H
/*** End of file mainwindow.h ***/
