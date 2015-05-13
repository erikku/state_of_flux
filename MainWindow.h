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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "IOSignalModel.h"
#include "IOSignalModelInputs.h"
#include "ControlSignalModel.h"
#include "StateModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static bool validIdentifier(const QString& ident);

    bool signalExists(const QString& sigName) const;

    QString SaveXml();
    void LoadXml(const QString& xml);

    QString GenerateVerilog() const;
    QString GenerateVHDL() const;

    QString indent(int level = 1) const;
    QString indentLine(const QString& line, int level = 1) const;
    QString indentLines(const QString& src, int level = 1) const;
    QString padLine(const QString& line, int length = 1) const;
    QString trimBlankLines(const QString& line) const;

protected slots:
    void SaveTemplate();
    void Save();
    void Load();
    void New();
    void About();
    void Generate();

    void ioListAdd();
    void ioListUp();
    void ioListDown();
    void ioListDelete();
    void ioListChanged();
    void ioListUpdate();

    void controlListAdd();
    void controlListUp();
    void controlListDown();
    void controlListDelete();
    void controlListRename();
    void controlListChanged();

    void stateListAdd();
    void stateListUp();
    void stateListDown();
    void stateListDelete();
    void stateListChanged();
    void stateListUpdate();

    void instUpdate();

protected:
    void closeEvent(QCloseEvent *evt);

private:
    QString mCleanXml;
    QString mDefaultTemplate;
    QString mProjectPath;

    IOSignalModel *mIOSignalModel;
    IOSignalModelInputs *mIOSignalModelShort;
    ControlSignalModel *mControlSignalModel;
    StateModel *mStateModel;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
