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

#ifndef SIGNAL_H
#define SIGNAL_H

#include <QtCore/QString>

class VerilogSignal
{
public:
    VerilogSignal();

    QString name() const;
    void setName(const QString& name);

private:
    QString mName;
};

#endif // SIGNAL_H
