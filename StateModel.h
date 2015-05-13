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

#ifndef STATEMODEL_H
#define STATEMODEL_H

#include "State.h"

#include <QtCore/QHash>
#include <QtCore/QList>

#include <QAbstractListModel>

class StateModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit StateModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    bool contains(const QString& sigName) const;
    void append(State *sig);

    void up(int index);
    void down(int index);
    void remove(int index);

    void update(int index);
    void clear();

    State* at(int index) const;

private:
    QList<State*> mStates;
    QHash<QString, State*> mStatesByName;
};

#endif // STATEMODEL_H
