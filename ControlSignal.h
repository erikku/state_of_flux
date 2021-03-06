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

#ifndef CONTROLSIGNAL_H
#define CONTROLSIGNAL_H

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "VerilogSignal.h"

class ControlSignal : public VerilogSignal
{
public:
    ControlSignal();

    QString declaration() const;

    QDomElement toXml(QDomDocument doc) const;
    static ControlSignal* fromXml(const QDomElement& node);
};

#endif // CONTROLSIGNAL_H
