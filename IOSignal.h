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

#ifndef IOSIGNAL_H
#define IOSIGNAL_H

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "VerilogSignal.h"

class IOSignal : public VerilogSignal
{
public:
    IOSignal();

    typedef enum _Direction
    {
        Direction_Input = 0,
        Direction_Output,
        Direction_Inout
    }Direction;

    typedef enum _Type
    {
        Type_Wire = 0,
        Type_Reg,
        Type_Logic
    }Type;

    Type type() const;
    void setType(Type type);

    Direction direction() const;
    void setDirection(Direction dir);

    int size() const;
    void setSize(int size);

    QString declaration() const;

    QDomElement toXml(QDomDocument doc) const;
    static IOSignal* fromXml(const QDomElement& node);

private:
    Type mType;
    Direction mDirection;
    int mSize;
};

#endif // IOSIGNAL_H
