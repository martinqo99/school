/**
 * @file: tablemodel.cpp
 * @author: Michal Lukac(xlukac05), Michal Muran(xmuran00)
 * @brief: The table for searchdialog.
 */
#include "tablemodel.h"
#include <QDebug>

/*!
 * Constructor, set model.
 */
TableModel::TableModel(QObject *parent, int rows, int columns, QList <QList<QString> *> list)
    :QAbstractTableModel(parent)
{
    this->rows = rows;
    this->columns = columns;
    this->list = list;
}

/*!
 * Return number of rows.
 */
int TableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return this->rows;
}

/*!
 * Return number of columns.
 */
int TableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return this->columns;
}

/*!
 * Set data in every cell.
 */
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
            return QVariant();

    if (role == Qt::DisplayRole)
    {
        return list.at(index.row())->at(index.column());
    }
    return QVariant();
}

/*!
 * Set header columns.
 */
QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Author");
            case 1:
                return QString("Net name");
            case 2:
                return QString("Version");
            case 3:
                return QString("Time Simulation");
            case 4:
                return QString("Login Simulation");
            case 5:
                return QString("Description");

            }
        }
    }
    return QVariant();
}
/*** End of file tablemodel.cpp ***/
