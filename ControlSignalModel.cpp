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

#include "ControlSignalModel.h"

ControlSignalModel::ControlSignalModel(QObject *parent) :
    QAbstractListModel(parent)
{
    // Nothing to see here.
}

int ControlSignalModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return mControlSignals.count();
}

QVariant ControlSignalModel::data(const QModelIndex& index, int role) const
{
    if(index.parent().isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= mControlSignals.count())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    ControlSignal *sig = mControlSignals.at(index.row());
    if(!sig)
        return QVariant();

    return sig->name();
}

bool ControlSignalModel::contains(const QString &sigName) const
{
    return mControlSignalsByName.contains(sigName);
}

void ControlSignalModel::append(ControlSignal *sig)
{
    if(contains(sig->name()))
        return;

    int row = mControlSignals.count();

    beginInsertRows(QModelIndex(), row, row);
    mControlSignals.append(sig);
    mControlSignalsByName[sig->name()] = sig;
    endInsertRows();
}

void ControlSignalModel::up(int index)
{
    if(index < 1 || index >= mControlSignals.count())
        return;

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
    ControlSignal *sig = mControlSignals.at(index);
    mControlSignals[index] = mControlSignals.at(index - 1);
    mControlSignals[index - 1] = sig;
    endMoveRows();
}

void ControlSignalModel::down(int index)
{
    if(index < 0 || index >= (mControlSignals.count() - 1))
        return;

    beginMoveRows(QModelIndex(), index + 1, index + 1, QModelIndex(), index);
    ControlSignal *sig = mControlSignals.at(index);
    mControlSignals[index] = mControlSignals.at(index + 1);
    mControlSignals[index + 1] = sig;
    endMoveRows();
}

void ControlSignalModel::remove(int index)
{
    if(index < 0 || index >= mControlSignals.count())
        return;

    ControlSignal *sig = mControlSignals.at(index);
    if(!sig)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    mControlSignals.removeAt(index);
    mControlSignalsByName.remove(sig->name());
    endRemoveRows();
}

ControlSignal* ControlSignalModel::at(int index) const
{
    if(index < 0 || index >= mControlSignals.count())
        return 0;

    return mControlSignals.at(index);
}

void ControlSignalModel::update(int index)
{
    if(index < 0 || index >= mControlSignals.count())
        return;

    ControlSignal *sig = mControlSignals.at(index);
    if(!sig)
        return;

    QString name = mControlSignalsByName.key(sig);
    if(name != sig->name())
    {
        mControlSignalsByName.remove(name);
        mControlSignalsByName[sig->name()] = sig;
    }

    emit dataChanged(this->index(index), this->index(index),
        QVector<int>() << Qt::DisplayRole);
}

void ControlSignalModel::clear()
{
    beginResetModel();

    foreach(ControlSignal *sig, mControlSignals)
        delete sig;

    mControlSignals.clear();
    mControlSignalsByName.clear();

    endResetModel();
}
