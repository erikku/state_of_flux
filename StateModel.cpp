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

#include "StateModel.h"

StateModel::StateModel(QObject *parent) :
    QAbstractListModel(parent)
{
    // Nothing to see here.
}

int StateModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return mStates.count();
}

QVariant StateModel::data(const QModelIndex& index, int role) const
{
    if(index.parent().isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= mStates.count())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    State *sig = mStates.at(index.row());
    if(!sig)
        return QVariant();

    return sig->name();
}

bool StateModel::contains(const QString &sigName) const
{
    return mStatesByName.contains(sigName);
}

void StateModel::append(State *sig)
{
    if(contains(sig->name()))
        return;

    int row = mStates.count();

    beginInsertRows(QModelIndex(), row, row);
    mStates.append(sig);
    mStatesByName[sig->name()] = sig;
    endInsertRows();
}

void StateModel::up(int index)
{
    if(index < 1 || index >= mStates.count())
        return;

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
    State *sig = mStates.at(index);
    mStates[index] = mStates.at(index - 1);
    mStates[index - 1] = sig;
    endMoveRows();
}

void StateModel::down(int index)
{
    if(index < 0 || index >= (mStates.count() - 1))
        return;

    beginMoveRows(QModelIndex(), index + 1, index + 1, QModelIndex(), index);
    State *sig = mStates.at(index);
    mStates[index] = mStates.at(index + 1);
    mStates[index + 1] = sig;
    endMoveRows();
}

void StateModel::remove(int index)
{
    if(index < 0 || index >= mStates.count())
        return;

    State *sig = mStates.at(index);
    if(!sig)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    mStates.removeAt(index);
    mStatesByName.remove(sig->name());
    endRemoveRows();
}

State* StateModel::at(int index) const
{
    if(index < 0 || index >= mStates.count())
        return 0;

    return mStates.at(index);
}

void StateModel::update(int index)
{
    if(index < 0 || index >= mStates.count())
        return;

    State *sig = mStates.at(index);
    if(!sig)
        return;

    QString name = mStatesByName.key(sig);
    if(name != sig->name())
    {
        mStatesByName.remove(name);
        mStatesByName[sig->name()] = sig;
    }

    emit dataChanged(this->index(index), this->index(index),
        QVector<int>() << Qt::DisplayRole);
}

void StateModel::clear()
{
    beginResetModel();

    foreach(State *sig, mStates)
        delete sig;

    mStates.clear();
    mStatesByName.clear();

    endResetModel();
}
