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

#include "ControlSignal.h"
#include "MainWindow.h"

#include <QtCore/QObject>

ControlSignal::ControlSignal() : VerilogSignal()
{
    // Nothing to see here.
}

QString ControlSignal::declaration() const
{
    return QObject::tr("reg %1;").arg(name());
}

QDomElement ControlSignal::toXml(QDomDocument doc) const
{
    QDomElement node = doc.createElement("control_signal");
    node.setAttribute("name", name());

    return node;
}

ControlSignal* ControlSignal::fromXml(const QDomElement& node)
{
    ControlSignal *sig = new ControlSignal;
    sig->setName(node.attribute("name").trimmed());
    if(!MainWindow::validIdentifier(sig->name()))
        sig->setName("invalid_name");

    return sig;
}
