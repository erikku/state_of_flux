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

#include "IOSignalModel.h"

IOSignalModel::IOSignalModel(QObject *parent) :
    QAbstractListModel(parent)
{
    // Nothing to see here.
}

int IOSignalModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return mIOSignals.count();
}

QVariant IOSignalModel::data(const QModelIndex& index, int role) const
{
    if(index.parent().isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= mIOSignals.count())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    IOSignal *sig = mIOSignals.at(index.row());
    if(!sig)
        return QVariant();

    return sig->declaration();
}

bool IOSignalModel::contains(const QString &sigName) const
{
    return mIOSignalsByName.contains(sigName);
}

void IOSignalModel::append(IOSignal *sig)
{
    if(contains(sig->name()))
        return;

    int row = mIOSignals.count();

    beginInsertRows(QModelIndex(), row, row);
    mIOSignals.append(sig);
    mIOSignalsByName[sig->name()] = sig;
    endInsertRows();
}

void IOSignalModel::up(int index)
{
    if(index < 1 || index >= mIOSignals.count())
        return;

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
    IOSignal *sig = mIOSignals.at(index);
    mIOSignals[index] = mIOSignals.at(index - 1);
    mIOSignals[index - 1] = sig;
    endMoveRows();
}

void IOSignalModel::down(int index)
{
    if(index < 0 || index >= (mIOSignals.count() - 1))
        return;

    beginMoveRows(QModelIndex(), index + 1, index + 1, QModelIndex(), index);
    IOSignal *sig = mIOSignals.at(index);
    mIOSignals[index] = mIOSignals.at(index + 1);
    mIOSignals[index + 1] = sig;
    endMoveRows();
}

void IOSignalModel::remove(int index)
{
    if(index < 0 || index >= mIOSignals.count())
        return;

    IOSignal *sig = mIOSignals.at(index);
    if(!sig)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    mIOSignals.removeAt(index);
    mIOSignalsByName.remove(sig->name());
    endRemoveRows();
}

IOSignal* IOSignalModel::at(int index) const
{
    if(index < 0 || index >= mIOSignals.count())
        return 0;

    return mIOSignals.at(index);
}

void IOSignalModel::update(int index)
{
    if(index < 0 || index >= mIOSignals.count())
        return;

    IOSignal *sig = mIOSignals.at(index);
    if(!sig)
        return;

    QString name = mIOSignalsByName.key(sig);
    if(name != sig->name())
    {
        mIOSignalsByName.remove(name);
        mIOSignalsByName[sig->name()] = sig;
    }

    emit dataChanged(this->index(index), this->index(index),
        QVector<int>() << Qt::DisplayRole);
}

void IOSignalModel::clear()
{
    beginResetModel();

    foreach(IOSignal *sig, mIOSignals)
        delete sig;

    mIOSignals.clear();
    mIOSignalsByName.clear();

    endResetModel();
}
