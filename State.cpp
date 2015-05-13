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

#include "State.h"
#include "MainWindow.h"

#include <QtCore/QObject>

State::State() : VerilogSignal()
{
    // Nothing to see here.
}

QString State::code() const
{
    return mCode;
}

void State::setCode(const QString& code)
{
    mCode = code;
}

QString State::declaration(int bits, int index) const
{
    return QObject::tr("localparam %1 = %2'd%3;").arg(name()).arg(bits).arg(index);
}

QDomElement State::toXml(QDomDocument doc) const
{
    QDomElement node = doc.createElement("state");
    node.setAttribute("name", name());
    node.appendChild(doc.createCDATASection(mCode));

    return node;
}

State* State::fromXml(const QDomElement& node)
{
    State *state = new State;
    state->setName(node.attribute("name").trimmed());
    if(!MainWindow::validIdentifier(state->name()))
        state->setName("invalid_name");

    state->setCode(node.text());

    return state;
}
