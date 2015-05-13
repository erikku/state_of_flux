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

#include "IOSignalModelInputs.h"

IOSignalModelInputs::IOSignalModelInputs(IOSignalModel *model,
    QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(model);
}

QVariant IOSignalModelInputs::data(const QModelIndex& index, int role) const
{
    if(index.parent().isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= qobject_cast<IOSignalModel*>(
        sourceModel())->rowCount())
    {
        return QVariant();
    }

    if(role != Qt::DisplayRole)
        return QVariant();

    IOSignal *sig = qobject_cast<IOSignalModel*>(
        sourceModel())->at(index.row());
    if(!sig)
        return QVariant();

    return sig->name();
}

bool IOSignalModelInputs::filterAcceptsRow(int source_row,
    const QModelIndex& source_parent) const
{
    if(source_parent.isValid())
        return false;

    if(source_row < 0 ||source_row >= qobject_cast<IOSignalModel*>(
        sourceModel())->rowCount())
    {
        return false;
    }

    IOSignal *sig = qobject_cast<IOSignalModel*>(
        sourceModel())->at(source_row);
    if(!sig)
        return false;

    return sig->direction() == IOSignal::Direction_Input && (
        sig->type() == IOSignal::Type_Wire ||
        sig->type() == IOSignal::Type_Logic) &&
        sig->size() == 1;
}
