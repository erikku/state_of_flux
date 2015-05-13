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

#include "IOSignal.h"
#include "MainWindow.h"

#include <QtCore/QObject>

IOSignal::IOSignal() : VerilogSignal()
{
    // Nothing to see here.
}

IOSignal::Type IOSignal::type() const
{
    return mType;
}

void IOSignal::setType(Type _type)
{
    mType = _type;
}

IOSignal::Direction IOSignal::direction() const
{
    return mDirection;
}

void IOSignal::setDirection(Direction dir)
{
    mDirection = dir;
}

int IOSignal::size() const
{
    return mSize;
}

void IOSignal::setSize(int sz)
{
    mSize = sz;
}

QString IOSignal::declaration() const
{
    QString dir;

    switch(mDirection)
    {
    case Direction_Output:
        dir = QObject::tr("output");
        break;
    case Direction_Inout:
        dir = QObject::tr("inout");
        break;
    default:
        dir = QObject::tr("input");
        break;
    }

    QString _type;

    switch(mType)
    {
    case Type_Reg:
        _type = QObject::tr("reg");
        break;
    case Type_Logic:
        _type = QObject::tr("logic");
        break;
    default:
        _type = QObject::tr("wire");
        break;
    }

    QString _size;

    if(mSize > 1)
        _size = QObject::tr(" [%1:0]").arg(mSize - 1);

    return QObject::tr("%1 %2%3 %4").arg(dir).arg(_type
        ).arg(_size).arg(name());
}

QDomElement IOSignal::toXml(QDomDocument doc) const
{
    QDomElement node = doc.createElement("io_signal");
    node.setAttribute("name", name());

    switch(mDirection)
    {
    case Direction_Output:
        node.setAttribute("direction", "output");
        break;
    case Direction_Inout:
        node.setAttribute("direction", "inout");
        break;
    default:
        node.setAttribute("direction", "input");
        break;
    }

    switch(mType)
    {
    case Type_Reg:
        node.setAttribute("type", "reg");
        break;
    case Type_Logic:
        node.setAttribute("type", "logic");
        break;
    default:
        node.setAttribute("type", "wire");
        break;
    }

    node.setAttribute("size", mSize);

    return node;
}

IOSignal* IOSignal::fromXml(const QDomElement& node)
{
    IOSignal *sig = new IOSignal;
    sig->setName(node.attribute("name").trimmed());
    if(!MainWindow::validIdentifier(sig->name()))
        sig->setName("invalid_name");

    QString dir = node.attribute("direction").trimmed().toLower();

    if(dir == "output")
        sig->setDirection(IOSignal::Direction_Output);
    else if(dir == "inout")
        sig->setDirection(IOSignal::Direction_Inout);
    else
        sig->setDirection(IOSignal::Direction_Input);

    QString _type = node.attribute("type").trimmed().toLower();

    if(_type == "reg")
        sig->setType(IOSignal::Type_Reg);
    else if(_type == "logic")
        sig->setType(IOSignal::Type_Logic);
    else
        sig->setType(IOSignal::Type_Wire);

    sig->setSize(node.attribute("size").trimmed().toInt());
    if(sig->size() < 1)
        sig->setSize(1);

    return sig;
}
