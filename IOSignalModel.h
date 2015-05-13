/*
 * Copyright (C) 2015 John Eric Martin <john.eric.martin@gmail.com>
 *
 * This file is part of State of Flux.
 *
 * State of Flux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * State of Flux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with State of Flux.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IOSIGNALMODEL_H
#define IOSIGNALMODEL_H

#include "IOSignal.h"

#include <QtCore/QHash>
#include <QtCore/QList>

#include <QAbstractListModel>

class IOSignalModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit IOSignalModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    bool contains(const QString& sigName) const;
    void append(IOSignal *sig);

    void up(int index);
    void down(int index);
    void remove(int index);

    void update(int index);
    void clear();

    IOSignal* at(int index) const;

private:
    QList<IOSignal*> mIOSignals;
    QHash<QString, IOSignal*> mIOSignalsByName;
};

#endif // IOSIGNALMODEL_H
