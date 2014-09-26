/**
 * @file: simulation.h
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Simulation class of petri net.
*/

#ifndef SIMULATION_H
#define SIMULATION_H
#include <QDomDocument>
#include <QGraphicsItem>
#include <QMap>

#include "place.h"
#include "transition.h"

class Simulation
{
public:
    Simulation(QDomDocument doc, int descriptor);
    ~Simulation();
    QString getNextStep(int mode, int *stateerror);
    void setTransitionStep(QString tsstep);
    int descriptor;
    void addLoadPlace(QString name, QString tokens);
    void addLoadArc(QString name, QString uniqname, int direction, QString endplace, QString endtransition);
    void addLoadTransition(QString name, QString condition, QString operation);
    QMap<Arc *, Place *> fromPlacesArcs;
    QStringList conditions;
    QString operation;
    QMap<QString, int> placeIndex;
    int mystep;
    QString mytransition;
    int doCheck();

private:
    int findNext(Place *place, bool *ok);
    int searchPlace(QString place,int *value);
    void initAnalysis(Transition *transition);
    int doAnalysis(QString *result, Transition *transition);
    QList<QGraphicsItem *> items;
    QList<Transition *> transitions;
    QList<Place *> places;
    QList<Arc *> arcs;
    Transition *actualtransition;
};

#endif // SIMULATION_H
