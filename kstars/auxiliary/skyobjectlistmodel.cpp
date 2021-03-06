/***************************************************************************
                          skyobjectlistmodel.cpp  -  K Desktop Planetarium
                             -------------------
    begin                : Wed Jul 29 2016
    copyright            : (C) 2016 by Artem Fedoskin
    email                : afedoskin3@gmail.com
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "skyobjectlistmodel.h"
#include "skyobject.h"

SkyObjectListModel::SkyObjectListModel(QObject *parent) : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> SkyObjectListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "name";
    roles[SkyObjectRole]   = "skyobject";
    return roles;
}

int SkyObjectListModel::indexOf(QString objectName) const
{
    for (int i = 0; i < skyObjects.size(); ++i)
    {
        if (skyObjects[i].first == objectName)
        {
            return i;
        }
    }
    return -1;
}

QVariant SkyObjectListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (role == Qt::DisplayRole)
    {
        return QVariant(skyObjects[index.row()].first);
    }
    else if (role == SkyObjectRole)
    {
        return qVariantFromValue((void *)skyObjects[index.row()].second);
    }
    return QVariant();
}

QStringList SkyObjectListModel::filter(QRegExp regEx)
{
    QStringList filteredList;
    for (int i = 0; i < skyObjects.size(); ++i)
    {
        if (regEx.exactMatch(skyObjects[i].first))
        {
            filteredList.append(skyObjects[i].first);
        }
    }
    return filteredList;
}

void SkyObjectListModel::setSkyObjectsList(QVector<QPair<QString, const SkyObject *>> sObjects)
{
    emit beginResetModel();
    skyObjects = sObjects;

    emit endResetModel();
}
