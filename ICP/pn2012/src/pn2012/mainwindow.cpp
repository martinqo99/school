/**
 * @file: mainwindow.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The mainwindow widget class functions.
 *         Project to ICP at BUT FIT 2011/2012. This application simulate
 *         Petri Nets Simulation client.
 *         Some part of the codes are from official qt framework page with examples
 *         and also example from book C++ gui programming with Qt4(graphic chapter).
 */

#include <QDebug>
#include <QtXml>
#include <QFileDialog>
#include <QDate>
#include <QXmlStreamWriter>
#include <QDomElement>
#include <QIODevice>
#include <QMessageBox>
#include <Qt>
#include <QtNetwork>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QSet>
#include <QGraphicsItem>
#include <QDesktopServices>
#include <QWebView>
#include <QPrinter>
#include <QPainter>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "searchdialog.h"
#include "place.h"
#include "transition.h"
#include "transitionproperties.h"
#include "placeproperties.h"
#include "arcproperties.h"
#include "visualoptions.h"
#include "setuser.h"
#include "setserver.h"
#include "fileoptions.h"

/*!
 * Constructor for mainwindow.cpp
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("pn2012");
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), SLOT(handleReply()));
    connect(ui->actionSave_at_server, SIGNAL(triggered()), SLOT(sendXML()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                    SLOT(gotError(QAbstractSocket::SocketError)));

    this->font = 1;
    shape = 0;
    login = "admin";
    password = "admin";
    port = 7123;
    server = "127.0.0.1";
    appmodepossible = 0;
    connected = 0;
    resultDialog = 0;
    simulinprogress = 0;
    tab_list_check.append(0);
    ui->textEdit->setText("Status bar:");
    setActions(EDITING);
    loadColors();
    initMainWindow();
    initTreeView();
    initSignals();

    /// show connections settings
    QTimer::singleShot(0, this, SLOT(on_actionSet_server_triggered()));

    QDir dirServerSites(QCoreApplication::applicationDirPath()+"/ServerSites");
    if(!dirServerSites.exists())
        dirServerSites.mkdir(QCoreApplication::applicationDirPath()+"/ServerSites");
}

/*!
 * Destructor for mainwindow.cpp
 */
MainWindow::~MainWindow()
{
    if(connected)
        socket->disconnectFromHost();
    delete ui;
    delete placesItem;
    delete transitionsItem;
    delete arcsItem;
}

/*!
 * Init basic things for mainwindow.cpp
 * This function is triggered with starting application.
 */
void MainWindow::initMainWindow()
{
    /// clean tab widget
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(0);

    /// set graphics view
    QGraphicsView *graphview = new QGraphicsView(this);
    CustomGraphicsScene *viewscene = new CustomGraphicsScene(this);
    graphview->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    graphview->setDragMode(QGraphicsView::RubberBandDrag);
    graphview->setContextMenuPolicy(Qt::CustomContextMenu);
    graphview->setScene(viewscene);
    connect(graphview,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenu(QPoint)));

    /// append graphics to list
    graphicslist.append(graphview);
    scenelist.append(viewscene);

    ui->tabWidget->addTab(graphview,tr("new_sim_1"));

    nametablist.append("newm_sim_1");
    versionlist.append("1.0");
    ui->tabWidget->setTabsClosable(true);

    /// connect tab close signal with removeTab slot
    graphicstabscount = 1;
    graphicstabsindex = 1;
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(removeTab(int)));

    /// tab init number
    activetabindex = 0;

    /// tab layout sequence
    seqnumberlist.append(0);
}

/*!
 * Set menu actions.
 */
void MainWindow::setActions(int mode)
{
    int tempmode;
    if(mode == EDITING)
    {
        ui->actionSimulate->setEnabled(false);
        ui->actionShow_result->setEnabled(false);
        ui->actionStop->setEnabled(false);
        ui->actionAdd_arc->setEnabled(true);
        ui->actionNew_net->setEnabled(true);
        ui->actionTransition->setEnabled(true);
        ui->actionPlace->setEnabled(true);
        ui->actionRemove->setEnabled(true);
        ui->properties_mode->setEnabled(true);
        tempmode = EDITING;
    }
    else
    {
        ui->actionSimulate->setEnabled(true);
        ui->actionShow_result->setEnabled(true);
        ui->actionStop->setEnabled(true);
        ui->actionAdd_arc->setEnabled(false);
        ui->actionNew_net->setEnabled(false);
        ui->actionTransition->setEnabled(false);
        ui->actionPlace->setEnabled(false);
        ui->actionRemove->setEnabled(false);
        ui->properties_mode->setEnabled(false);
        tempmode = SIMULATION;
    }

    /// go through all scene and setflags
    foreach(CustomGraphicsScene *scene,scenelist)
    {
        /// set flags for items
        QList<QGraphicsItem *> items = scene->items();
        foreach(QGraphicsItem *item, items)
        {
            Transition *transition = dynamic_cast<Transition *>(item);
            Place *place = dynamic_cast<Place *>(item);
            Arc *arc = dynamic_cast<Arc *>(item);
            if(transition)
            {
                transition->setCustomFlags(tempmode);
            }
            else if(place)
            {
                place->setCustomFlags(tempmode);
            }
            else if(arc)
            {
                ;
            }
        }
        items.clear();
    }
}

/*!
 * Set colors for all scenes items.
 */
void MainWindow::setColors(QColor tratout, QColor tratext, QColor traback, QColor platext, QColor plaout, QColor plaback, QColor arccol,QString font,QString shape)
{
    /// set colors
    transbackground = traback;
    transtext = tratext;
    transoutline = tratout;
    placebackground = plaback;
    placeoutline = plaout;
    placetext = platext;
    arccolor = arccol;

    if(font == "Arial")
        this->font = 1;
    else
        this->font = 0;

    if(shape == "Ellipse")
        this->shape = 0;
    else
        this->shape = 1;

    /// repaint colors of graphics Items at scenes
    foreach(CustomGraphicsScene *scene,scenelist)
    {
        QList<QGraphicsItem *> items = scene->items();
        foreach(QGraphicsItem *item, items)
        {
            Transition *transition = dynamic_cast<Transition *>(item);
            Place *place = dynamic_cast<Place *>(item);
            Arc *arc = dynamic_cast<Arc *>(item);
            if(transition)
            {
                transition->setFont(this->font);
                transition->setColors(transoutline,transtext,transbackground);
            }
            else if(place)
            {
                place->setShapeFont(this->shape,this->font);
                place->setColors(placeoutline,placetext,placebackground);   
            }
            else if(arc)
            {
                arc->setFont(this->font);
                arc->setColor(arccolor);
            }
        }
        items.clear();
    }
}

/*!
 * This function initialize tree view.
 */
void MainWindow::initTreeView()
{
    /// set tree view
    treemodel = new QStandardItemModel(this);
    QStringList label;
    label << "Petri net:";
    treemodel->setHorizontalHeaderLabels(label);
    rootNode = treemodel->invisibleRootItem();

    placesItem = new QStandardItem("Places");
    placesItem->setFlags(placesItem->flags() & ~Qt::ItemIsEditable);
    transitionsItem =  new QStandardItem("Transitions");
    transitionsItem->setFlags(transitionsItem->flags() & ~Qt::ItemIsEditable);
    arcsItem = new QStandardItem("Arcs");
    arcsItem->setFlags(arcsItem->flags() & ~Qt::ItemIsEditable);

    rootNode->appendRow(placesItem);
    rootNode->appendRow(transitionsItem);
    rootNode->appendRow(arcsItem);

    ui->treeView->setModel(treemodel);
    ui->treeView->expandAll();

    /// set selection changed signal/slot
    QItemSelectionModel *selectionModel= ui->treeView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));
}

/*!
 * Append one item to treeview.
 */
void MainWindow::appendItemToTreeView(QGraphicsItem *item)
{
    Transition *transition = dynamic_cast<Transition *>(item);
    Place *place = dynamic_cast<Place *>(item);
    Arc *arc = dynamic_cast<Arc *>(item);

    if(place)
    {
        QStandardItem *temp = new QStandardItem(place->label());
        temp->setFlags(temp->flags() & ~Qt::ItemIsEditable);
        placesItem->appendRow(temp);
    }
    else if(transition)
    {
        QStandardItem *temp = new QStandardItem(transition->label());
        temp->setFlags(temp->flags() & ~Qt::ItemIsEditable);
        transitionsItem->appendRow(temp);
    }
    else if(arc)
    {
        QStandardItem *temp = new QStandardItem(arc->getName());
        temp->setFlags(temp->flags() & ~Qt::ItemIsEditable);
        arcsItem->appendRow(temp);
    }
}

/*!
 * Initialize signals for some widgets,...
 */
void MainWindow::initSignals()
{
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
    connect(this,SIGNAL(searchSignal()),this,SLOT(searchSignalHandler()));
}

/*!
 * Initialize text in status bar.
 */
void MainWindow::initStatusText()
{
    ui->textEdit->setText("Status bar:");
}

/*!
 * Do Simulation in graphicsview.
 */
void MainWindow::doSimulation(int simulation)
{
    if(connected)
    {
        simulstep = simulation;
        doConnect("SIMULATION");
    }
}

/*!
 * Call full simulation.
 */
void MainWindow::on_actionSimulate_triggered()
{
    if(!simulinprogress)
    {
        doSimulation(0);
        ui->textEdit->setText("Status bar:\nSimulation of the entire site. Max 25 iterations.");
    }
    else
        ui->textEdit->setText("<font color=#FF0000>Status bar:\nPlease exit step simulation if you want to simulate multiple nets, or run another client(server is concurrent)</font>");
}

/*!
 * Stop stop/init simulation.
 */
void MainWindow::on_actionStop_triggered()
{
    simulinprogress = 0;
    doConnect("SIMULATIONSTOP");
}

/*!
 * Call previous step of simulation
 */
void MainWindow::on_actionPrevious_step_triggered()
{
    ui->textEdit->setText("Status bar:\nSimulation - one previous step.");
    doSimulation(2);
}

/*!
 * Simple slot function for creating new simulation graphics scene.
 */
void MainWindow::on_actionNew_net_triggered()
{
    ui->textEdit->setText("Status bar:\nNew tab was added.");
    tab_list_check.append(0);
    /// add to list new graphicscene
    graphicstabscount++;
    graphicstabsindex++;
    QGraphicsView *graphview = new QGraphicsView(this);
    //QGraphicsScene *viewscene = new QGraphicsScene(this);
    CustomGraphicsScene * viewscene = new CustomGraphicsScene(this);
    graphview->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    graphview->setContextMenuPolicy(Qt::ActionsContextMenu);
    graphview->setDragMode(QGraphicsView::RubberBandDrag);
    graphview->setScene(viewscene);
    graphicslist.append(graphview);
    scenelist.append(viewscene);

    /// add tab to tabwidget
    QString number;
    number = number.setNum(graphicstabsindex);
    ui->tabWidget->addTab(graphview,tr("new_sim_")+number);
    nametablist.append("new_sim_"+number);
    versionlist.append("1.0");
    /// tab sequence layout
    seqnumberlist.append(0);
}

/*!
 * Simulation mode triggered.
 */
void MainWindow::on_actionSimulation_triggered()
{
    if(appmodepossible)
    {
        ui->textEdit->setText("Status bar:\nSimulation mode.");
        setActions(SIMULATION);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You are offline(Only edit mode is enabled).");
        msgBox.exec();
    }
}

/*!
 * Edit mode triggered.
 */
void MainWindow::on_actionEditing_triggered()
{
    ui->textEdit->setText("Status bar:\nEditing mode.");
    setActions(EDITING);
}

/*!
 * Slot for changing tab.
 */
void MainWindow::tabChanged(int index)
{
    activetabindex = index;
    refreshTreeView();
}

/*!
 * Slot for tabClose trigger. This slot removes selected tab.
 */
void MainWindow::removeTab(int tabIndex)
{
    /// if it is not last tab
    if(graphicstabscount != 1)
    {
        tab_list_check.removeAt(tabIndex);
        ui->tabWidget->removeTab(tabIndex);
        QGraphicsView *temp1 = graphicslist.at(tabIndex);
        CustomGraphicsScene *temp2 = scenelist.at(tabIndex);
        delete temp1;
        delete temp2;
        graphicslist.removeAt(tabIndex);
        nametablist.removeAt(tabIndex);
        scenelist.removeAt(tabIndex);
        seqnumberlist.removeAt(tabIndex);
        graphicstabscount--;
    }
}

/*!
 * Slot if selection is changed.
 */
void MainWindow::selectionChangedSlot(const QItemSelection &, const QItemSelection &)
{
    /// get the text of the selected item
    const QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();

    /// find out the hierarchy level of the selected item
    int hierarchyLevel=1;
    QModelIndex seekRoot = index;
    while(seekRoot.parent() != QModelIndex())
    {
        seekRoot = seekRoot.parent();
        hierarchyLevel++;
    }
    QString showString = QString("%1, Level %2").arg(selectedText)
                         .arg(hierarchyLevel);
    //setWindowTitle(showString);
}

/*!
 * Exit application signal.
 */
void MainWindow::on_action_Quit_triggered()
{
    this->close();
}

/*!
 * Run about dialog.
 */
void MainWindow::on_actionAbout_triggered()
{
    about = new About(this);
    about->show();
}

/*!
 * Run search dialog.
 */
void MainWindow::on_actionSearch_nets_triggered()
{
    if(!connected)
    {
        QMessageBox::information(this,"Connection message","You are not connected to server.!");
        return;
    }

    doConnect("SITEXML");

}

/*!
 * Create new place and setup it.
 */
void MainWindow::on_actionPlace_triggered()
{
    ui->textEdit->setText("Status bar:\nPlace added.");
    Place *place = new Place(placetext,placeoutline,placebackground);
    QString number;
    place->setShapeFont(shape,font);
    number = number.setNum(seqnumberlist[activetabindex]);
    place->setText("p"+number,"1,2,3");
    setupPlace(place);
    QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(place);
    appendItemToTreeView(item);
}

/*!
 *  Create new transition and setup it.
 */
void MainWindow::on_actionTransition_triggered()
{
    ui->textEdit->setText("Status bar:\nTransition added.");
    Transition *transition = new Transition(transtext,transoutline,transbackground);
    QString number;
    transition->setFont(font);
    number = number.setNum(seqnumberlist[activetabindex]);
    transition->setText("x < 4","y = 5","trans"+number);
    setupTransition(transition);
    QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(transition);
    appendItemToTreeView(item);
}

/*!
 * Setup place to graphics scene.
 */
void MainWindow::setupPlace(Place *place)
{
    place->setPos(QPoint(100+ (100 * (seqnumberlist[activetabindex] % 5)),
                         100+ (70 *  ((seqnumberlist[activetabindex] / 5) % 7))));
    CustomGraphicsScene *scene = scenelist.at(activetabindex);
    scene->addItem(place);
    seqnumberlist[activetabindex]++;
    scene->clearSelection();
    place->setSelected(true);
}

/*!
 * Setup transition to graphics scene.
 */
void MainWindow::setupTransition(Transition *transition)
{
    transition->setPos(QPoint(100+(100 *(seqnumberlist[activetabindex] % 5)),100 + (70 * ((seqnumberlist[activetabindex] /5) % 7))));
    //QGraphicsScene *scene = scenelist.at(activetabindex);
    CustomGraphicsScene *scene = scenelist.at(activetabindex);
    scene->addItem(transition);
    seqnumberlist[activetabindex]++;
    scene->clearSelection();
    transition->setSelected(true);
}

/*!
 * Return selected pair Transition-Place/Place-Transition.
 */
MainWindow::SelectedPair MainWindow::selectedTransPlacePair() const
{
    QList<QGraphicsItem *> items = scenelist[activetabindex]->selectedItems();
    if(items.count() == 2)
    {
        Transition *transition = dynamic_cast<Transition *>(items.first());
        Place *place = dynamic_cast<Place *>(items.last());
        if(transition && place)
            return SelectedPair(place,transition);
        else
        {
            transition = dynamic_cast<Transition *>(items.last());
            place = dynamic_cast<Place *>(items.first());
            if(transition && place)
                return SelectedPair(place,transition);
        }
    }
    return SelectedPair();
}

/*!
 * Load colors from visualoptions.xml
 */
void MainWindow::loadColors()
{
    /// open file
    QDomDocument document;
    bool defaultxml = true;
    QFile file(":/configures/configure/visualoptions.xml");
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        if(!document.setContent(&file))
            defaultxml = true;
        else
            defaultxml = false;
        file.close();
    }

    /// if not open, set default colors
    if(defaultxml)
    {
        transtext = QColor("#000000");
        transbackground = QColor("#ffffff");
        transoutline = QColor("#000000");
        placetext = QColor("#000000");
        placebackground = QColor("#ffffff");
        placeoutline = QColor("#000000");
        arccolor = QColor("#000000");
        this->font = 1;
        this->shape = 0;
        return;
    }

    QDomElement root = document.firstChildElement();
    /// transition colors
    QDomNodeList node = root.elementsByTagName("transition");
    QDomNode itemnode = node.at(0);
    QDomElement itemele = itemnode.toElement();
    QStringList list = itemele.text().split('#');
    transtext = QColor("#" + list.at(3));
    transbackground = QColor("#" + list.at(2));
    transoutline = QColor("#" + list.at(1));
    list.clear();
    /// place colors
    node = root.elementsByTagName("place");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    list = itemele.text().split('#');
    placetext = QColor("#"+list.at(1));
    placebackground = QColor("#"+list.at(2));
    placeoutline = QColor("#"+list.at(3));
    list.clear();
    /// arc color
    node = root.elementsByTagName("arc");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    list = itemele.text().split('#');
    arccolor = QColor("#"+list.at(1));
    /// shape and font
    node = root.elementsByTagName("font");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    QString font1 = itemele.text();
    node = root.elementsByTagName("shape");
    itemnode = node.at(0);
    itemele = itemnode.toElement();
    QString shape1 = itemele.text();

    if(font1 == "Arial")
        this->font = 1;
    else
        this->font = 0;

    if(shape1 == "Ellipse")
        this->shape = 0;
    else
        this->shape = 1;
}

/*!
 * Connect transition with place.
 */
void MainWindow::on_actionAdd_arc_triggered()
{
    SelectedPair pair = selectedTransPlacePair();
    if(pair == SelectedPair())
        return;
    else
    {
        ui->textEdit->setText("Status bar:\nArc added.");
        QString number;
        number = number.setNum(seqnumberlist[activetabindex]);
        seqnumberlist[activetabindex]++;
        Arc *arc = new Arc(pair.second,pair.first,1,arccolor,"Arc"+number);
        scenelist[activetabindex]->addItem(arc);
        arc->setFont(font);
        QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(arc);
        appendItemToTreeView(item);
    }
}

/*!
 * Remove selected items.
 */
void MainWindow::on_actionRemove_triggered()
{
    QList<QGraphicsItem *> items = scenelist[activetabindex]->selectedItems();
    QMutableListIterator<QGraphicsItem *> i(items);

    ///
    while(i.hasNext())
    {
        Arc *arc = dynamic_cast<Arc *>(i.next());
        Transition *transition = dynamic_cast<Transition *>(i.value());
        Place *place = dynamic_cast<Place *>(i.value());
        if(arc)
        {
            arc->removeArc();
            delete arc;
            i.remove();

        }
        else if(transition)
        {
            ;
        }
        else if(place)
        {
            ;
        }
    }
    qDeleteAll(items);
    refreshTreeView();
    ui->textEdit->setText("Status bar:\nItem was removed.");
}

/*!
 * When properties dialog of transition/place/link is clicked.
 */
void MainWindow::on_properties_mode_triggered()
{
    Transition *transition = selectedTransition();
    Place *place = selectedPlace();
    Arc *arc = selectedArc();

    if(transition)
    {
        TransitionProperties dialog(transition,this);
        dialog.exec();
    }
    else if(place)
    {
        PlaceProperties dialog(place,this);
        dialog.exec();
    }
    else if(arc)
    {
        ArcProperties dialog(arc,this);
        dialog.exec();
    }
}

/*!
 * Return selected transition.
 */
Transition *MainWindow::selectedTransition()
{
    QList<QGraphicsItem *> items = scenelist[activetabindex]->selectedItems();
    if(items.count() == 1)
    {
        Transition *transition = dynamic_cast<Transition *>(items.first());
        if(transition) return transition;
    }
    return 0;
}

/*!
 * Return selected Place.
 */
Place *MainWindow::selectedPlace()
{
    QList<QGraphicsItem *> items = scenelist[activetabindex]->selectedItems();
    if(items.count() == 1)
    {
        Place *place = dynamic_cast<Place *>(items.first());
        if(place) return place;
    }
    return 0;
}

/*!
 * Return selected Arc.
 */
Arc *MainWindow::selectedArc()
{
    QList<QGraphicsItem *> items = scenelist[activetabindex]->selectedItems();
    if(items.count() == 1)
    {
        Arc *arc = dynamic_cast<Arc *>(items.first());
        if(arc) return arc;
    }
    return 0;
}

/*!
 * When the graphicsscene/tab is changed, refresh the treeview.
 */
void MainWindow::refreshTreeView()
{
    placesItem->removeRows(0,placesItem->rowCount());
    transitionsItem->removeRows(0,transitionsItem->rowCount());
    arcsItem->removeRows(0,arcsItem->rowCount());

    QList<QGraphicsItem *>items = scenelist[activetabindex]->items();
    foreach(QGraphicsItem *item,items)
        appendItemToTreeView(item);
}

/*!
 * Show visual options.
 */
void MainWindow::on_actionVisual_options_triggered()
{
    VisualOptions dialog(this,transoutline,transtext,transbackground,placetext,placeoutline,placebackground,arccolor,font,shape);
    dialog.exec();
}

/*!
 * Create context menu when right click on graphics scene.
 */
void MainWindow::contextMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionTransition);
    menu->addAction(ui->actionPlace);
    menu->addAction(ui->properties_mode);
    menu->exec(graphicslist[activetabindex]->mapToGlobal(pos));
}

/*!
 * Set server options
 */
void MainWindow::set_server_port(QString server1, int port1, QString login1, QString password1)
{
    server = server1;
    port = port1;
    login = login1;
    password = password1;
}

/*!
 * Set file options.
 */
void MainWindow::set_fileoptions(QString namesite, QString versionsite, QString discription)
{
    namesite_m = namesite;
    versionsite_m = versionsite;
    discription_m = discription;
}

/*!
 * Create new Tab when the XML file is open
 */
void MainWindow::new_tab(QString name_site, QString version_site)
{
    /// add to list new graphicscene
    graphicstabscount++;
    graphicstabsindex++;
    QGraphicsView *graphview = new QGraphicsView(this);
    CustomGraphicsScene * viewscene = new CustomGraphicsScene(this);
    graphview->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    graphview->setContextMenuPolicy(Qt::ActionsContextMenu);
    graphview->setDragMode(QGraphicsView::RubberBandDrag);
    graphview->setScene(viewscene);
    graphicslist.append(graphview);
    scenelist.append(viewscene);

    /// add tab to tabwidget
    ui->tabWidget->addTab(graphview,name_site);
    nametablist.append(name_site);
    versionlist.append(version_site);

    /// tab sequence layout
    seqnumberlist.append(0);
    activetabindex = scenelist.count()-1;
    ui->tabWidget->setCurrentIndex(activetabindex);
}

/*!
 * Add Place to graphicsscene.
 */
void MainWindow::setupPlaceLoad(Place *place)
{
    CustomGraphicsScene *scene = scenelist.at(activetabindex);
    scene->addItem(place);
    scene->clearSelection();
}

/*!
 * Add Transition to graphicsscene.
 */
void MainWindow::setupTransitionLoad(Transition *transition)
{
    CustomGraphicsScene *scene = scenelist.at(activetabindex);
    scene->addItem(transition);
    scene->clearSelection();
}

/*!
 * Load net to graphicsview.
 */
void MainWindow::loadNetToView(QString filename,int mode)
{
    /// open file
    QFile file(filename);
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
        return;

    /// get root element
    QDomElement root = doc.documentElement();
    if( root.tagName() != "Petriho_site" )
    {
        QMessageBox::warning(this,"Error","Wrong XML file format!");
        return;
    }

    qDebug() << "seqnum" << seqnumberlist[activetabindex] ;
    QFileInfo pathInfo( filename );
    QString Name( pathInfo.completeBaseName());
    if(mode==1)
        new_tab(Name,"1.0");

    /// get count of items in scene
    seqnumberlist[activetabindex] = root.attribute("count","").toInt(0,10);

    /// go through nodes and load items to scene
    QDomNode n = root.firstChild();
    while( !n.isNull() )
    {
      QDomElement e = n.toElement();
      if( !e.isNull() )
      {
        if( e.tagName() == "Place" )
        {
          QString name = e.attribute("name","");
          double x = e.attribute("X","").toDouble();
          double y = e.attribute("Y","").toDouble();
          QString tokens = e.attribute("tokens","");
          addLoadPlace(name.replace("_"," "),tokens.replace("_"," "),x,y);
        }
        else if( e.tagName() == "Transition" )
        {
          QString name = e.attribute("name","");
          double x = e.attribute("X","").toDouble();
          double y = e.attribute("Y","").toDouble();
          QString condition = e.attribute("condition","");
          QString operation = e.attribute("operation","");
          addLoadTransition(name.replace("_"," "),x,y,condition.replace("_"," "),operation.replace("_"," "));
        }
        else if( e.tagName() == "Arc" )
        {
          QString name = e.attribute("label","");
          QString uniqname = e.attribute("uniqname","");
          int direction = e.attribute("direction","").toInt();
          QString endplace = e.attribute("endplace","");
          QString endtransition = e.attribute("endtransition","");
          addLoadArc(name.replace("_"," "), uniqname.replace("_"," "),direction,endplace.replace("_"," "),endtransition.replace("_"," "));
        }
      }
      n = n.nextSibling();
    }
    file.close();
}

/*!
 * Open file dialog to open XML file and read contains of file
 */
void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open xml file."),QApplication::applicationDirPath()+"/../../examples/","Xml files (*.xml)");
    if(filename.isEmpty())
        return;
    else
    {
        tab_list_check.append(0);
        ui->textEdit->setText("Status bar:\nFile was opened from the client.");
        loadNetToView(filename,1);
    }
}

/*!
 * Load Place from XML file and add to scene
 */
void MainWindow::addLoadPlace(QString name, QString tokens, double x, double y)
{
    Place *place = new Place(placetext,placeoutline,placebackground);
    place->setText(name,tokens);
    place->setPos(x,y);
    place->setShapeFont(shape,font);
    setupPlaceLoad(place);
    QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(place);
    appendItemToTreeView(item);
}

/*!
 * Load Transition from XML file and add to scene
 */
void MainWindow::addLoadTransition(QString name, double x, double y, QString condition, QString operation)
{
    Transition *transition = new Transition(transtext,transoutline,transbackground);
    transition->setText(condition,operation,name);
    transition->setPos(x,y);
    transition->setFont(font);
    setupTransitionLoad(transition);
    QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(transition);
    appendItemToTreeView(item);
}

/*!
 * Load Arc from XML file and add to scene
 */
void MainWindow::addLoadArc(QString name, QString uniqname, int direction, QString endplace, QString endtransition)
{
    Transition *endt = NULL;
    Place *endp = NULL;

    /// go through items and find transition and place
    foreach(QGraphicsItem* item, scenelist[activetabindex]->items())
    {
        Transition *transition = dynamic_cast<Transition *>(item);
        Place *place = dynamic_cast<Place *>(item);

        if(place)
        {
           if(place->label() == endplace)
               endp = place;
        }
        else if(transition)
        {
            if(transition->label() == endtransition)
                endt = transition;
        }
    }

    Arc *arc = new Arc(endt,endp,direction,arccolor,uniqname);
    arc->setLabel(name);
    arc->setFont(font);
    CustomGraphicsScene *scene = scenelist.at(activetabindex);
    scene->addItem(arc);
    scene->clearSelection();
    QGraphicsItem *item = dynamic_cast<QGraphicsItem *>(arc);
    appendItemToTreeView(item);
}

/*!
 * Open file dialog to save xml file
 */
void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"Save xml file",QApplication::applicationDirPath()+"/../../examples/","Xml file *.xml");
    if(filename.isEmpty())
        return;
    else
    {
        ui->textEdit->setText("Status bar:\nFile has been stored on the client.");
        QFileInfo pathInfo(filename);
        ui->tabWidget->setTabText(activetabindex,pathInfo.fileName().section(".",0,0)); //section for cut xml
        nametablist.replace(activetabindex,pathInfo.fileName().section(".",0,0)); //section for cut xml
        saveFiles(filename);
    }
}

/*!
 * Create XML file
 */
void MainWindow::saveFiles(QString filename)
{
    QDomDocument doc;

    QDomNode xmlNode = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());

    QDomElement root = doc.createElement( "Petriho_site" );
    root.setAttribute("count",++seqnumberlist[activetabindex]);
    doc.appendChild( root );

    /// create elements
    foreach( QGraphicsItem* item, scenelist[activetabindex]->items())
    {
        Transition *transition = dynamic_cast<Transition *>(item);
        Place *place = dynamic_cast<Place *>(item);
        Arc *arc = dynamic_cast<Arc *>(item);

        if(place)
        {
            QPointF p =place->pos();
            root.appendChild(ContactToNodePlace(doc,place->label(),place->tokens(),p.x(),p.y()));
        }
        else if(transition)
        {
           QPointF t = transition->pos();
           root.appendChild(ContactToNodeTransition(doc, transition->label(), transition->condition(), transition->operation(), t.x(),t.y()));
        }
        else if(arc)
        {
           root.appendChild(ContactToNodeArc(doc, arc->getDirection(), arc->getLabel(), arc->getName(), arc->toPlace()->label(),arc->toTransition()->label()));
        }
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("Can not open file!");
       return;
    }

    QTextStream ts( &file );
    ts << doc.toString();;

    file.close();
}

/*!
 * Create element Place with attributes to XML
 */
QDomElement MainWindow::ContactToNodePlace(QDomDocument &d,QString name, QString tokens, double x, double y)
{
   QDomElement cn = d.createElement( "Place" );
   cn.setAttribute( "name", name.replace(" ","_") );
   cn.setAttribute( "X", x );
   cn.setAttribute( "Y", y );
   cn.setAttribute( "tokens", tokens.replace(" ","_") );

   return cn;
}

/*!
 * Create Element Arc with attributes to XML
 */
QDomElement MainWindow::ContactToNodeArc(QDomDocument &d, int direction, QString name, QString uniqname, QString place, QString transition)
{
   QDomElement cn = d.createElement( "Arc" );
   cn.setAttribute( "label", name.replace(" ","_") );
   cn.setAttribute( "uniqname", uniqname.replace(" ","_"));
   cn.setAttribute( "direction", direction );
   cn.setAttribute( "endplace", place.replace(" ","_") );
   cn.setAttribute( "endtransition", transition.replace(" ","_") );

   return cn;
}

/*!
 * Create element Transition with attributes to XML
 */
QDomElement MainWindow::ContactToNodeTransition(QDomDocument &d, QString name, QString condition, QString operation, double x, double y)
{
   QDomElement cn = d.createElement( "Transition" );
   cn.setAttribute( "name", name.replace(" ","_") );
   cn.setAttribute( "condition", condition.replace(" ","_") );
   cn.setAttribute( "operation", operation.replace(" ","_") );
   cn.setAttribute( "X", x );
   cn.setAttribute( "Y", y );

   return cn;
}

/*!
 * Show Server option
 */
void MainWindow::on_actionSet_server_triggered()
{
    if(connected)
        socket->disconnectFromHost();
    SetServer dialog(server,login,password,port,this);
    dialog.exec();
}

/*!
 * Connect to server and send request.
 */
void MainWindow::doConnect(QString command)
{
    QByteArray request;

    //zahashovanie hesla
    QString pass = QCryptographicHash::hash(password.toAscii(),
                                            QCryptographicHash::Sha1).toHex();

    /// send specified command to server
    if(command == "USERNEW")
    {
        QString new_pass = QCryptographicHash::hash(user_password.toAscii(),QCryptographicHash::Sha1).toHex();
        request += QString("USERNEW^%1^%2^").arg(user).arg((new_pass));
    }
    else if(command == "USERVAL")
    {
        socket->connectToHost(server, port);
        request += QString("USERVAL^%1^%2^").arg(login).arg((pass));
    }
    else if(command == "SAVEXML")
    {
        /// request saving xml file to server
        request += QString("SAVEXML^%1^%2^%3^%4^").arg(login)
        .arg(namesite_m).arg(versionsite_m)
        .arg(discription_m);
    }
    else if(command == "LOADXML")
    {
        ui->textEdit->setText("Status bar:\nFile from the server has been opened.");
        /// request to load file from server
        request += QString("LOADXML^%1^%2^").arg(login).arg(filename);
    }
    else if(command == "SITEXML")
    {
        /// request to load sitexml file which contains all xml at server
        request += QString("SITEXML^%1^").arg(login);
    }
    else if(command == "SIMULATION")
    {
        QDateTime date = QDateTime::currentDateTime();
        QString dateString = date.toString("dd.MM.yyyy hh:mm:ss");
        QString vfile = nametablist.at(activetabindex);
        vfile.remove(0,vfile.length() - 3);

        request += QString("SIMULATION^%1^%2.xml^%3^%4^%5^%6^").arg(login)
               .arg(nametablist.at(activetabindex)).arg(vfile)
       .arg(simulstep).arg(transitionSim).arg(dateString);
    }
    else if(command == "SIMULATIONSTOP")
    {
        request += QString("SIMULATIONSTOP^%1^").arg(login);
        refreshFromServer();
    }

    this->socketWrite(request);
}

/*!
 * Send xml file to server.
 */
void MainWindow::sendXML()
{
    if(!connected)
    {
        QMessageBox::information(this,"Connection message","You are not connected to server.!");
        return;
    }

    FileOptions dialog(login,versionlist.at(activetabindex),this);
    dialog.exec();
    if(resultDialog)
    {
        doConnect("SAVEXML");

        QDomDocument doc;

        QDomNode xmlNode = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        doc.insertBefore(xmlNode, doc.firstChild());

        QDomElement root = doc.createElement( "Petriho_site" );
        root.setAttribute("count",++seqnumberlist[activetabindex]);
        doc.appendChild( root );

        /// go through items from scene and create xml elements
        foreach( QGraphicsItem* item, scenelist[activetabindex]->items())
        {
            Transition *transition = dynamic_cast<Transition *>(item);
            Place *place = dynamic_cast<Place *>(item);
            Arc *arc = dynamic_cast<Arc *>(item);

            if(place)
            {
                QPointF p =place->pos();
                root.appendChild(ContactToNodePlace(doc,place->label(),place->tokens(),p.x(),p.y()));
            }
            else if(transition)
            {
               QPointF t = transition->pos();
               root.appendChild(ContactToNodeTransition(doc, transition->label(), transition->condition(), transition->operation(), t.x(),t.y()));
            }
            else if(arc)
            {
               root.appendChild(ContactToNodeArc(doc, arc->getDirection(), arc->getLabel(), arc->getName(), arc->toPlace()->label(),arc->toTransition()->label()));
            }
        }

        QByteArray docfile;
        docfile.append(doc.toString()); /// = doc.toString();

        socket->write(docfile);
        socket->waitForBytesWritten();
        ui->textEdit->setText("Status bar:\nFile has been stored on the server.");
    }
}

/*!
 * Write data to socket.
 */
void MainWindow::socketWrite(QByteArray data)
{
    socket->write(data);
}

/*!
 * When error occurs critical mes
 */
void MainWindow::gotError(QAbstractSocket::SocketError error)
{
    QMessageBox::critical(this, tr("Network error"), socket->errorString());
    connected = 0;
}

/*!
 * Save file from server on disk.
 */
void MainWindow::saveFileFromServer(QByteArray filetext, QString filename)
{
    if(connected)
    {
        QFile file;

        if(filename == "SiteXml.xml")
        {
            file.setFileName(QCoreApplication::applicationDirPath()+"/"+filename);
        }
        else
        {
            file.setFileName(QCoreApplication::applicationDirPath()+"/ServerSites/"+filename);
        }
        if(!(file.open(QIODevice::WriteOnly)))
        {
            qDebug("Cant open file!");
            return;
        }
        file.write(filetext);
        file.close();

        if(filename != "SiteXml.xml")
        {
            tab_list_check.append(1);
            loadNetToView(QCoreApplication::applicationDirPath()+"/ServerSites/"+filename,1);
        }
    }
}

/*!
 * Run one simulation step.
 */
void MainWindow::simulationStep(QString result)
{
    QDomDocument doc;
    doc.setContent(result);
    QDomElement root = doc.documentElement();
    QDomNode n = root.firstChild();
    QList<QGraphicsItem *> items = scenelist[activetabindex]->items();

    /// go through xml code and set new tokens to places
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName() == "place")
            {
            QString name = e.attribute("name","");
            QString tokens = e.text();

            foreach(QGraphicsItem *item, items)
            {
                Place *place = dynamic_cast<Place *>(item);
                if(place)
                {
                    if(place->label()==name)
                        place->setText(name,tokens);
                }
            }
            }
        }
        n = n.nextSibling();
    }
    items.clear();
}

/*!
 * Refresh graphics items.
 */
void MainWindow::refreshFromServer()
{
    QList<QGraphicsItem *> items = scenelist[activetabindex]->items();
    QMutableListIterator<QGraphicsItem *> i(items);
    while(i.hasNext())
    {
        Arc *arc = dynamic_cast<Arc *>(i.next());
        Transition *transition = dynamic_cast<Transition *>(i.value());
        Place *place = dynamic_cast<Place *>(i.value());
        if(arc)
        {
            arc->removeArc();
            delete arc;
            i.remove();

        }
        else if(transition)
        {
            ;
        }
        else if(place)
        {
            ;
        }
    }

    /// load net to view
    qDeleteAll(items);
    loadNetToView(QCoreApplication::applicationDirPath()+"/ServerSites/"+nametablist.at(activetabindex)+".xml",0);
    refreshTreeView();
    graphicslist.at(activetabindex)->update();
    ui->textEdit->setText("Status bar:\nSimulation was stopped and initialized.");
}

/*!
 * Handler when reply from server.
 */
void MainWindow::handleReply()
{
    /// accepting data
    QByteArray rawdata = socket->readAll();
    /// arguments
    QList<QByteArray> data = rawdata.split('^');
    /// command from server
    QByteArray command = data.takeFirst();
    if(command == "BADPSWD")
    {
        QMessageBox::information(this,"Response from server","Bad password!");
        socket->disconnectFromHost();
        connected = 0;
    }
    else if(command == "USEREXIST")
    {
        QMessageBox::information(this,"Response from server","User exist!");
        socket->disconnectFromHost();
        connected = 0;
    }
    else if(command == "USERADD")
    {
        QMessageBox::information(this,"Response from server","User added!");
        //connected = 0;
    }
    else if(command == "BADUSER")
    {
        QMessageBox::information(this,"Response from server","User not exist!");
        //socket->waitForBytesWritten();
        socket->disconnectFromHost();
        connected = 0;
    }
    else if(command == "LOADXML")
    {
        QByteArray file = data.takeFirst();
        /// get file from server
        saveFileFromServer(data.takeFirst(),file);
    }
    else if(command == "SITEXML")
    {
        /// get sitexml file
        saveFileFromServer(data.takeFirst(),"SiteXml.xml");
        emit searchSignal();
    }
    else if(command == "SIMULATION")
    {
        simulationStep(data.takeFirst());
    }
    else if(command == "SIMULATIONERROR")
    {
        ui->textEdit->setText("<font color=#FF0000>Status bar:\nError with simulation.Try to repair your net</font>");
    }
}

/*!
 * Search dialog triggered.
 */
void MainWindow::searchSignalHandler()
{
    SearchDialog searchdialog(this);
    searchdialog.exec();
}

/*!
 * New user dialog triggered.
 */
void MainWindow::on_actionNew_user_triggered()
{
    if(!connected)
    {
        QMessageBox::information(this,"Connection message","You are not connected to server.!");
        return;
    }
    SetUser dialog(this);
    dialog.exec();
}

/*!
 * Set user password and login.
 */
void MainWindow::set_user(QString user1, QString user_password1)
{
    user = user1;
    user_password = user_password1;
}

/*!
 * Help dialog triggered.
 */
void MainWindow::on_actionHelp_triggered()
{
    QWebView *view = new QWebView();
    view->load(QUrl("qrc:/configures/configure/index.html"));
    view->show();
    //QDesktopServices::openUrl(QUrl("qrc:/configures/configure/index.html"));
}

/*!
 * Run simulation.
 */
void MainWindow::on_actionShow_result_triggered()
{
    if(tab_list_check.at(activetabindex) == 0)
    {
        QMessageBox::information(this,"Simulation message","You must load file from server!");
        initStatusText();
        return;
    }
    Transition *transition = selectedTransition();
    if(transition)
    {
        simulinprogress = 1;
        transitionSim.clear();
        transitionSim.append(transition->name());
        initStatusText();
        doSimulation(1);
        ui->textEdit->setText("Status bar:\nSimulation - one next step.");
    }
    else
    {
        ui->textEdit->setText("<font color=#FF0000>Status bar:\nPlease select one transition for simulation step.</font>");
    }
}

/*!
 * Export to pdf triggered.
 */
void MainWindow::on_actionExport_to_pdf_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
    QString(), "*.pdf");
    QPrinter *pdfPrinter = new QPrinter;
    QPainter *pdfPainter = new QPainter;
    pdfPrinter->setOutputFormat(QPrinter::PdfFormat);
    pdfPrinter->setPaperSize(QSize(scenelist.at(activetabindex)->width(), scenelist.at(activetabindex)->height()), QPrinter::Point);
    pdfPrinter->setFullPage(true);
    pdfPrinter->setOutputFileName(fileName);
    pdfPainter->begin(pdfPrinter);
    scenelist.at(activetabindex)->render(pdfPainter);
    pdfPainter->end();
    delete pdfPainter;
    delete pdfPrinter;
}
/*** End of file mainwindow.cpp ***/
