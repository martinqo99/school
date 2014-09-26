/**
 * @file: simulation.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The Simulation class of petri net.
*/

#include "simulation.h"
#include <QDebug>

/*!
 * A constructor, add places/transitions/arcs to list.
 */
Simulation::Simulation(QDomDocument doc,int descriptor)
{
    this->descriptor = descriptor;
    QDomElement root = doc.documentElement();
    if( root.tagName() != "Petriho_site" )
    {
        qDebug() << descriptor <<"SIMULATION" << "Error with xml format!";
        return;
    }

    /// go through xml an add to lists.
    QDomNode n = root.firstChild();
    while(!n.isNull())
    {
      QDomElement e = n.toElement();
      if(!e.isNull())
      {
        if(e.tagName() == "Info")
        {
            ;
        }
        else if( e.tagName() == "Place" )
        {
          QString name = e.attribute("name","");
          QString tokens = e.attribute("tokens","");
          addLoadPlace(name.replace("_"," "),tokens.replace("_"," "));
        }
        else if( e.tagName() == "Transition" )
        {
          QString name = e.attribute("name","");
          QString condition = e.attribute("condition","");
          QString operation = e.attribute("operation","");
          addLoadTransition(name.replace("_"," "),condition.replace("_"," "),operation.replace("_"," "));
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
}

/*!
 * A destructor.
 */
Simulation::~Simulation()
{
    foreach(Arc *arc,arcs)
        delete arc;
    arcs.clear();
    foreach(Transition *trans,transitions)
        delete trans;
    transitions.clear();
    foreach(Place *plac,places)
        delete plac;
    places.clear();
}

/*!
 * Go through all transitions and get simulation result.
 */
QString Simulation::getNextStep(int mode,int *stateerror)
{
    QString responseMessage("<simulation_step>");
    if(mode == 0)
    {
        int a = 1; int iter = 0;
        while(a != 0 && iter != 25)
        {
            iter++;
            a = 0;
            foreach(Transition *trans,transitions)
            {
                int i = 0;
                /// find all arcs and get places from and places to transition
                foreach(Arc *arc, trans->myArcs)
                {
                    /// to place = 0
                    if(arc->getDirection() == 0)
                    {
                        ;
                    }
                    /// from place = 1
                    else
                    {
                        bool ok;
                        arc->getLabel().toInt(&ok);
                        if(ok)
                        {
                            trans->appendCondition(" & temp"+QString::number(i)+" == "+arc->getLabel());
                            arc->setLabel("temp"+QString::number(i));
                        }
                        fromPlacesArcs.insert(arc,arc->getPlace());
                    }
                    i++;
                }
                initAnalysis(trans);
                QString result;
                int retval = doAnalysis(&result,trans);
                if(retval == 1)
                {
                    *stateerror = 0;
                    responseMessage.append(result);
                    a = 1;
                }
                else if(retval == -1)
                {
                    *stateerror = 1;
                }
                else if(retval == 0)
                {
                    *stateerror = 2;
                }
                placeIndex.clear();
                fromPlacesArcs.clear();
            }
        }
    }
    /// only one step
    else
    {
        foreach(Transition *trans,transitions)
        {
            if(trans->name() == mytransition)
            {
                int i = 0;
                /// find all arcs and get places from and places to transition
                foreach(Arc *arc, trans->myArcs)
                {
                    /// to place = 0
                    if(arc->getDirection() == 0)
                    {
                        ;
                    }
                    /// from place = 1
                    else
                    {
                        bool ok;
                        arc->getLabel().toInt(&ok);
                        if(ok)
                        {
                            trans->appendCondition(" & temp"+QString::number(i)+" == "+arc->getLabel());
                            arc->setLabel("temp"+QString::number(i));
                        }
                        fromPlacesArcs.insert(arc,arc->getPlace());
                    }
                    i++;
                }
                initAnalysis(trans);
                QString result;
                int retval = doAnalysis(&result,trans);
                if(retval == 1)
                {
                    *stateerror = 0;
                    responseMessage.append(result);
                }
                else if(retval == -1)
                {
                    *stateerror = 1;
                }
                else if(retval == 0)
                {
                    *stateerror = 2;
                }
                placeIndex.clear();
                fromPlacesArcs.clear();
                break;
            }
        }
    }
    responseMessage.append("</simulation_step>");
    return responseMessage;
}

/*!
 * Set transition if is only one simulation step.
 */
void Simulation::setTransitionStep(QString tsstep)
{
    mytransition = tsstep;
}

/*!
 * Add place to lists.
 */
void Simulation::addLoadPlace(QString name, QString tokens)
{
    Place *place = new Place();
    place->setText(name,tokens);
    items.append((QGraphicsItem *)place);
    places.append(place);
}

/*!
 * Add arc to lists.
 */
void Simulation::addLoadArc(QString name, QString uniqname, int direction, QString endplace, QString endtransition)
{
    Transition *endt = NULL;
    Place *endp = NULL;
    foreach(QGraphicsItem* item, items)
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

    Arc *arc = new Arc(endt,endp,direction,uniqname);
    arc->setLabel(name);
    items.append((QGraphicsItem *)arc);
    arcs.append(arc);
}

/*!
 * Add transition to lists.
 */
void Simulation::addLoadTransition(QString name, QString condition, QString operation)
{
    Transition *transition = new Transition();
    transition->setText(condition,operation,name);
    items.append((QGraphicsItem *)transition);
    transitions.append(transition);
}

/*!
 * Initialize analysis.
 */
void Simulation::initAnalysis(Transition *transition)
{
    QString condition =  transition->condition().replace(QRegExp(" +& +"),"&");
    conditions = condition.split("&");
    operation = transition->operation();
}

/*!
 * Analyze one transition.
 */
int Simulation::doAnalysis(QString *result,Transition *transition)
{
    /// parse operation
    operation = operation.replace("+"," + ");
    operation = operation.replace("-"," - ");
    operation = operation.replace("="," = ");
    operation = operation.replace(QRegExp(" +")," ");
    QStringList partsOperations;
    partsOperations << operation.split(" ");

    /// loop through conditions
    for(int i = 0; i < conditions.length(); i++)
    {
        /// parse condition
        QString condition = conditions.at(i);
        condition = condition.replace("<"," < ");
        condition = condition.replace("<="," <= ");
        condition = condition.replace(">="," >= ");
        condition = condition.replace(">"," > ");
        condition = condition.replace("=="," == ");
        condition = condition.replace("!="," != ");
        condition = condition.replace(QRegExp(" +")," ");

        QStringList partsConditions;
        partsConditions << condition.split(" ");

        /// doing analysis
        if(partsConditions.length() != 3)
            return -1;

        QString operand1 = partsConditions.at(0);
        QString operator1 = partsConditions.at(1);
        QString operand2 = partsConditions.at(2);

        /// get operand 1
        bool ok1;
        int value1 = operand1.toInt(&ok1);
        if(!ok1) /// if it is not integer
        {
            if(placeIndex.contains(operand1))
                ;
            else
                placeIndex.insert(operand1,0);

            if(searchPlace(operand1,&value1))
                return 0;
        }

        /// get operand 2
        bool ok2;
        int value2 = operand2.toInt(&ok2);
        if(!ok2) /// if it is not integer
        {
            if(placeIndex.contains(operand1))
                ;
            else
                placeIndex.insert(operand1,0);

            if(searchPlace(operand2,&value2))
                return 0;
        }

        /// get result
        int value = 0;
        if(operator1 == "<")
        {value = (value1 < value2);}
        else if(operator1 == ">")
        {value = (value1 > value2);}
        else if(operator1 == "<=")
        {value = (value1 <= value2);}
        else if(operator1 == ">=")
        {value = (value1 >= value2);}
        else if(operator1 == "==")
        {value = (value1 == value2);}
        else if(operator1 == "!=")
        {value = (value1 != value2);}
        else return -1;

        /// increment value in qmap
        if(value == 0)
        {
            if(!ok1)
            {
                placeIndex[operand1] = placeIndex[operand1] + 1;
                i = -1;
            }
            else if(!ok2)
            {
                placeIndex[operand2] = placeIndex[operand2] + 1;
                i = -1;
            }
        }
        partsConditions.clear();
    }

    qDebug() << descriptor << "SIMULATION" << "OPERATION EVALUATION";
    if(!(partsOperations.length() > 2))
        return -1;

    QString var = partsOperations.takeFirst();
    partsOperations.takeFirst();

    /// evaluate operation
    int a = 0; int resultval = 0; int valueop = 0; int lastoperator =0;
    foreach(QString item, partsOperations)
    {
        if(a == 0)
        {
            /// have operand
            a = 1;
            bool ok;
            valueop = item.toInt(&ok);
            if(!ok) ///if it is not integer
            {
                if(searchPlace(item,&valueop))
                    return -1;
            }
            resultval = valueop;
        }
        else if (a == 1)
        {
            /// have operator
            if(item == "+")
                lastoperator = 1;
            else if(item == "-")
                lastoperator = 2;
            else
                return -1;
            a = 2;
        }
        else
        {
            bool ok;
            valueop = item.toInt(&ok);
            if(!ok) /// if it is not integer
            {
                if(searchPlace(item,&valueop))
                    return -1;
            }
            /// get a variable/constant
            if(lastoperator == 1)
                resultval += valueop;
            else if(lastoperator == 2)
                resultval -= valueop;
            a = 1;
        }
    }

    /// remove tokens from places
    QMap<QString, int>::iterator i;
    QMap<Arc *, Place *>::iterator j;
    for (i = placeIndex.begin(); i != placeIndex.end(); ++i)
    {
        for (j = fromPlacesArcs.begin(); j != fromPlacesArcs.end(); ++j)
        {
            Arc *arc = j.key();
            if(arc->getLabel() == i.key())
            {
                Place *temp = j.value();
                temp->myTokens.removeAt(i.value());
                result->append("<place");
                result->append(" name=\"");
                result->append(temp->label());
                result->append("\">");
                result->append(temp->myTokens.join(","));
                result->append("</place>\n");
            }
        }
    }

    bool ok3;
    /// add tokens to places
    foreach(Arc *arc, transition->myArcs)
    {
        /// to place = 0
        if(arc->getDirection() == 0)
        {
            arc->getLabel().toInt(&ok3);

            if(ok3 || (arc->getLabel() == var))
            {
                Place *place = arc->getPlace();
                result->append("<place");
                result->append(" name=\"");
                result->append(place->label());
                result->append("\">");

                if(ok3)
                    place->myTokens.append(arc->getLabel());
                else
                    place->myTokens.append(QString::number(resultval));
                int i = 0;
                foreach(QString token,place->myTokens)
                {
                    if(token == "")
                        place->myTokens.removeAt(i);
                    i++;
                }
                result->append(place->myTokens.join(","));
                result->append("</place>\n");
            }
        }
        /// from place = 1
        else
            ;
    }

    qDebug() << descriptor << "SIMULATION RESULT OF OPERATION" << resultval;
    return 1;
}

/*!
 * Check if every transition has arc to place and from place.
 */
int Simulation::doCheck()
{
    int totrans = 0;
    int toplace = 0;
    foreach(Transition *trans,transitions)
    {
        totrans = 0;
        toplace = 0;
        foreach(Arc *arc, trans->myArcs)
        {
            if(arc->getDirection() == 0)
                totrans = 1;
            else
                toplace = 1;
        }
        if((totrans+toplace) != 2)
            return 1;
    }
    return 0;
}

/*!
 * Search token in place with QMap.
 */
int Simulation::searchPlace(QString arclabel,int *value)
{
    QString token1; bool ok = 0;
    QMap<Arc *, Place *>::iterator i;
    for (i = fromPlacesArcs.begin(); i != fromPlacesArcs.end(); ++i)
    {
        Arc *arc = i.key();
        if(arc->getLabel() == arclabel)
        {
            Place *place = arc->getPlace();
            if(!place->myTokens.isEmpty())
            {
                if(place->myTokens.length() > placeIndex[arclabel])
                {
                    token1 = place->myTokens.at(placeIndex[arclabel]);
                    *value = token1.toInt(&ok,10);
                    if(!ok)
                        return 1;
                }
                else
                    return 1;
            }
            else
                return 1;
            return 0;
        }
    }
    return 1;
}
