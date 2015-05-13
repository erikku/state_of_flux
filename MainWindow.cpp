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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "VerilogHighlighter.h"

#include <cmath>

#include <QtCore/QRegExp>
#include <QtCore/QSettings>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QFile templateFile(":/template.xml");
    templateFile.open(QIODevice::ReadOnly);
    mDefaultTemplate = QString::fromUtf8(templateFile.readAll());
    templateFile.close();

    mIOSignalModel = new IOSignalModel();
    mIOSignalModelShort = new IOSignalModelInputs(mIOSignalModel);
    mControlSignalModel = new ControlSignalModel();
    mStateModel = new StateModel();

    ui->setupUi(this);
    ui->ioList->setModel(mIOSignalModel);
    ui->controlList->setModel(mControlSignalModel);
    ui->clockSignal->setModel(mIOSignalModelShort);
    ui->resetSignal->setModel(mIOSignalModelShort);
    ui->stateList->setModel(mStateModel);
    ui->stateReset->setModel(mStateModel);

    /// @todo Use these?
    ui->mainToolBar->hide();
    ui->statusBar->hide();

    /// @todo Add VHDL support.
    ui->outputLang->removeItem(1);

    ui->dataCode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->taskCode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->headerCode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->stateCode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->instCode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->stateDefaultsCode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    QPalette codePalette = ui->dataCode->palette();
    codePalette.setColor(QPalette::Foreground, QColor("#F8F8F2"));
    codePalette.setColor(QPalette::Background, QColor("#272822"));
    codePalette.setColor(QPalette::Text, QColor("#F8F8F2"));
    codePalette.setColor(QPalette::Base, QColor("#272822"));
    codePalette.setColor(QPalette::HighlightedText, QColor("#F8F8F2"));
    codePalette.setColor(QPalette::Highlight, QColor("#49483E"));

    ui->dataCode->setPalette(codePalette);
    ui->taskCode->setPalette(codePalette);
    ui->headerCode->setPalette(codePalette);
    ui->stateCode->setPalette(codePalette);
    ui->instCode->setPalette(codePalette);
    ui->stateDefaultsCode->setPalette(codePalette);

    new VerilogHighlighter(ui->dataCode->document());
    new VerilogHighlighter(ui->taskCode->document());
    new VerilogHighlighter(ui->headerCode->document());
    new VerilogHighlighter(ui->stateCode->document());
    new VerilogHighlighter(ui->instCode->document());
    new VerilogHighlighter(ui->stateDefaultsCode->document());

    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(Save()));
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(Load()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(New()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));
    connect(ui->generate, SIGNAL(clicked()), this, SLOT(Generate()));
    connect(ui->actionSaveTemplate, SIGNAL(triggered()),
        this, SLOT(SaveTemplate()));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui->ioSignal, SIGNAL(textChanged(QString)),
        this, SLOT(ioListUpdate()));
    connect(ui->ioDirection, SIGNAL(currentIndexChanged(int)),
        this, SLOT(ioListUpdate()));
    connect(ui->ioType, SIGNAL(currentIndexChanged(int)),
        this, SLOT(ioListUpdate()));
    connect(ui->ioSize, SIGNAL(valueChanged(int)),
        this, SLOT(ioListUpdate()));

    connect(ui->ioListAdd, SIGNAL(clicked()),
        this, SLOT(ioListAdd()));
    connect(ui->ioListUp, SIGNAL(clicked()),
        this, SLOT(ioListUp()));
    connect(ui->ioListDown, SIGNAL(clicked()),
        this, SLOT(ioListDown()));
    connect(ui->ioListDelete, SIGNAL(clicked()),
        this, SLOT(ioListDelete()));
    connect(ui->ioList->selectionModel(), SIGNAL(selectionChanged(
        QItemSelection,QItemSelection)), this, SLOT(ioListChanged()));

    connect(ui->controlListAdd, SIGNAL(clicked()),
        this, SLOT(controlListAdd()));
    connect(ui->controlListUp, SIGNAL(clicked()),
        this, SLOT(controlListUp()));
    connect(ui->controlListDown, SIGNAL(clicked()),
        this, SLOT(controlListDown()));
    connect(ui->controlListDelete, SIGNAL(clicked()),
        this, SLOT(controlListDelete()));
    connect(ui->controlListRename, SIGNAL(clicked()),
        this, SLOT(controlListRename()));
    connect(ui->controlList->selectionModel(), SIGNAL(selectionChanged(
        QItemSelection,QItemSelection)), this, SLOT(controlListChanged()));

    connect(ui->stateName, SIGNAL(textChanged(QString)),
        this, SLOT(stateListUpdate()));
    connect(ui->stateCode, SIGNAL(textChanged()),
            this, SLOT(stateListUpdate()));

    connect(ui->stateListAdd, SIGNAL(clicked()),
        this, SLOT(stateListAdd()));
    connect(ui->stateListUp, SIGNAL(clicked()),
        this, SLOT(stateListUp()));
    connect(ui->stateListDown, SIGNAL(clicked()),
        this, SLOT(stateListDown()));
    connect(ui->stateListDelete, SIGNAL(clicked()),
        this, SLOT(stateListDelete()));
    connect(ui->stateList->selectionModel(), SIGNAL(selectionChanged(
        QItemSelection,QItemSelection)), this, SLOT(stateListChanged()));

    // Let it load the template without a dialog box.
    mCleanXml = SaveXml();

    New();

    QSettings settings;
    restoreGeometry(settings.value("geom").toByteArray());
    restoreState(settings.value("state").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SaveTemplate()
{
    QSettings settings;
    settings.setValue("template", SaveXml());
}

void MainWindow::Save()
{
    if(mProjectPath.isEmpty())
    {
        QString path = QFileDialog::getSaveFileName(this, tr("Save Project"),
            QString(), tr("State of Flux FSM (*.fsm)"));
        if(path.isEmpty())
            return;

        mProjectPath = path;
    }

    QFile project(mProjectPath);
    if(!project.open(QIODevice::WriteOnly) || !project.write(SaveXml().toUtf8()))
    {
        QMessageBox::critical(this, tr("Save Failed"), tr("Failed to save the project."));
        return;
    }

    mCleanXml = SaveXml();

    project.close();

    setWindowTitle(tr("State of Flux - HDL FSM Made Easy [%1]").arg(QFileInfo(mProjectPath).baseName()));
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
    // Check for saving.
    if(mCleanXml != SaveXml())
    {
        int button = QMessageBox::question(this, tr("Save Project?"),
            tr("The project has unsaved changes. Save first?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(button == QMessageBox::Cancel)
        {
            evt->ignore();
            return;
        }

        if(button == QMessageBox::Yes)
        {
            if(mProjectPath.isEmpty())
            {
                QString path = QFileDialog::getSaveFileName(this, tr("Save Project"),
                    QString(), tr("State of Flux FSM (*.fsm)"));
                if(path.isEmpty())
                {
                    evt->ignore();
                    return;
                }

                mProjectPath = path;
            }

            QFile project(mProjectPath);
            if(!project.open(QIODevice::WriteOnly) || !project.write(SaveXml().toUtf8()))
            {
                QMessageBox::critical(this, tr("Save Failed"), tr("Failed to save the project."));
                evt->ignore();
                return;
            }

            mCleanXml = SaveXml();

            project.close();
        }
    }

    QSettings settings;
    settings.setValue("geom", saveGeometry());
    settings.setValue("state", saveState());

    evt->accept();
}

void MainWindow::New()
{
    // Check for saving.
    if(mCleanXml != SaveXml())
    {
        int button = QMessageBox::question(this, tr("Save Project?"),
            tr("The project has unsaved changes. Save first?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(button == QMessageBox::Cancel)
            return;

        if(button == QMessageBox::Yes)
        {
            if(mProjectPath.isEmpty())
            {
                QString path = QFileDialog::getSaveFileName(this, tr("Save Project"),
                    QString(), tr("State of Flux FSM (*.fsm)"));
                if(path.isEmpty())
                    return;

                mProjectPath = path;
            }

            QFile project(mProjectPath);
            if(!project.open(QIODevice::WriteOnly) || !project.write(SaveXml().toUtf8()))
            {
                QMessageBox::critical(this, tr("Save Failed"), tr("Failed to save the project."));
                return;
            }

            mCleanXml = SaveXml();

            project.close();
        }
    }

    mProjectPath.clear();

    setWindowTitle(tr("State of Flux - HDL FSM Made Easy [%1]").arg("Untitled"));

    QSettings settings;

    // Load the template.
    LoadXml(settings.value("template", mDefaultTemplate).toString());
}

void MainWindow::Load()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Load Project"),
        QString(), tr("State of Flux FSM (*.fsm)"));
    if(path.isEmpty())
        return;

    mProjectPath = path;

    QString xml;
    QFile project(mProjectPath);
    if(!project.open(QIODevice::ReadOnly) || (xml = QString::fromUtf8(project.readAll())).isEmpty())
    {
        QMessageBox::critical(this, tr("Load Failed"), tr("Failed to load the project."));
        return;
    }

    project.close();

    LoadXml(xml);

    setWindowTitle(tr("State of Flux - HDL FSM Made Easy [%1]").arg(QFileInfo(mProjectPath).baseName()));
}

QString MainWindow::SaveXml()
{
    QDomDocument doc;

    // Create the root element.
    QDomElement root = doc.createElement("state_of_flux");
    doc.appendChild(root);

    for(int i = 0; i < mIOSignalModel->rowCount(); i++)
        root.appendChild(mIOSignalModel->at(i)->toXml(doc));

    for(int i = 0; i < mControlSignalModel->rowCount(); i++)
        root.appendChild(mControlSignalModel->at(i)->toXml(doc));

    for(int i = 0; i < mStateModel->rowCount(); i++)
        root.appendChild(mStateModel->at(i)->toXml(doc));

    QDomElement clockSignal = doc.createElement("clock");
    clockSignal.setAttribute("signal", ui->clockSignal->currentText());
    root.appendChild(clockSignal);

    QDomElement resetSignal = doc.createElement("reset");
    resetSignal.setAttribute("signal", ui->resetSignal->currentText());
    root.appendChild(resetSignal);

    switch(ui->resetType->currentIndex())
    {
    case 0:
        resetSignal.setAttribute("type", "async");
        break;
    default:
        resetSignal.setAttribute("type", "sync");
        break;
    }

    QDomElement options = doc.createElement("options");
    options.setAttribute("ascii_states", ui->asciiStates->isChecked());
    options.setAttribute("default_nettype", ui->defaultNettype->isChecked());
    options.setAttribute("language", ui->outputLang->currentText().toLower());
    options.setAttribute("indent_type", ui->indentType->currentText().toLower());
    options.setAttribute("indent_size", ui->indentValue->value());
    root.appendChild(options);

    QDomElement datapath = doc.createElement("datapath");
    datapath.appendChild(doc.createCDATASection(ui->dataCode->toPlainText()));
    root.appendChild(datapath);

    QDomElement header = doc.createElement("header");
    header.appendChild(doc.createCDATASection(ui->headerCode->toPlainText()));
    root.appendChild(header);

    QDomElement tasks = doc.createElement("tasks");
    tasks.appendChild(doc.createCDATASection(ui->taskCode->toPlainText()));
    root.appendChild(tasks);

    QDomElement stateDefaults = doc.createElement("state_defaults");
    stateDefaults.appendChild(doc.createCDATASection(ui->stateDefaultsCode->toPlainText()));
    root.appendChild(stateDefaults);

    QDomElement resetState = doc.createElement("reset_state");
    resetState.setAttribute("state", ui->stateReset->currentText());
    root.appendChild(resetState);

    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
        doc.toString();

    // std::cout << xml.toUtf8().constData() << std::endl;

    return xml;
}

void MainWindow::LoadXml(const QString& xml)
{
    // std::cout << xml.toUtf8().constData() << std::endl;

    // Clear the models first.
    mIOSignalModel->clear();
    mControlSignalModel->clear();
    mStateModel->clear();

    QDomDocument doc;
    doc.setContent(xml);

    QDomElement root = doc.documentElement();

    QDomNodeList nodes = root.elementsByTagName("io_signal");

    for(int i = 0; i < nodes.count(); i++)
        mIOSignalModel->append(IOSignal::fromXml(nodes.at(i).toElement()));

    ioListChanged();

    nodes = root.elementsByTagName("control_signal");

    for(int i = 0; i < nodes.count(); i++)
        mControlSignalModel->append(ControlSignal::fromXml(nodes.at(i).toElement()));

    controlListChanged();

    nodes = root.elementsByTagName("state");

    for(int i = 0; i < nodes.count(); i++)
        mStateModel->append(State::fromXml(nodes.at(i).toElement()));

    stateListChanged();

    nodes = root.elementsByTagName("clock");
    if(nodes.count())
    {
        QDomElement clockSignal = nodes.at(0).toElement();
        ui->clockSignal->setCurrentText(clockSignal.attribute("signal"));
    }
    else
    {
        ui->clockSignal->setCurrentIndex(0);
    }

    nodes = root.elementsByTagName("reset");
    if(nodes.count())
    {
        QDomElement resetSignal = nodes.at(0).toElement();
        ui->resetSignal->setCurrentText(resetSignal.attribute("signal"));

        if(resetSignal.attribute("type").toLower().trimmed() == "sync")
            ui->resetType->setCurrentIndex(1);
        else
            ui->resetType->setCurrentIndex(0);
    }
    else
    {
        ui->resetSignal->setCurrentIndex(0);
        ui->resetType->setCurrentIndex(0);
    }

    nodes = root.elementsByTagName("options");
    if(nodes.count())
    {
        QDomElement options = nodes.at(0).toElement();
        ui->asciiStates->setChecked(options.attribute("ascii_states").toInt());
        ui->defaultNettype->setChecked(options.attribute("default_nettype").toInt());
        ui->outputLang->setCurrentIndex(options.attribute("language").toLower().trimmed() == "vhdl" ? 1 : 0);
        ui->indentType->setCurrentIndex(options.attribute("indent_type").toLower().trimmed() == "tabs" ? 1 : 0);
        ui->indentValue->setValue(options.attribute("indent_size").toInt());
    }
    else
    {
        ui->asciiStates->setChecked(true);
        ui->defaultNettype->setChecked(true);
        ui->outputLang->setCurrentIndex(0);
        ui->indentType->setCurrentIndex(0);
        ui->indentValue->setValue(2);
    }

    nodes = root.elementsByTagName("datapath");
    if(nodes.count())
    {
        QDomElement datapath = nodes.at(0).toElement();
        ui->dataCode->setPlainText(datapath.text());
    }
    else
    {
        ui->dataCode->clear();
    }

    nodes = root.elementsByTagName("header");
    if(nodes.count())
    {
        QDomElement header = nodes.at(0).toElement();
        ui->headerCode->setPlainText(header.text());
    }
    else
    {
        ui->headerCode->clear();
    }

    nodes = root.elementsByTagName("tasks");
    if(nodes.count())
    {
        QDomElement tasks = nodes.at(0).toElement();
        ui->taskCode->setPlainText(tasks.text());
    }
    else
    {
        ui->taskCode->clear();
    }

    nodes = root.elementsByTagName("state_defaults");
    if(nodes.count())
    {
        QDomElement stateDefaults = nodes.at(0).toElement();
        ui->stateDefaultsCode->setPlainText(stateDefaults.text());
    }
    else
    {
        ui->stateDefaultsCode->clear();
    }

    nodes = root.elementsByTagName("reset_state");
    if(nodes.count())
    {
        QDomElement resetState = nodes.at(0).toElement();
        ui->stateReset->setCurrentText(resetState.attribute("state"));
    }
    else
    {
        ui->stateReset->clear();
    }

    mCleanXml = SaveXml();
}

bool MainWindow::signalExists(const QString& sigName) const
{
    return sigName == "current_state" || sigName == "next_state" ||
        mIOSignalModel->contains(sigName) ||
        mControlSignalModel->contains(sigName) ||
        mStateModel->contains(sigName);
}

bool MainWindow::validIdentifier(const QString& ident)
{
    // Use the SystemVerilog keywords list as there are more.
    static QRegExp identRegExp("([a-zA-Z_][a-zA-Z0-9_$]*|\\\\[^ \t\f\r]+)");
    static QStringList keywords(QStringList()
        << QString("accept_on")
        << QString("alias")
        << QString("always")
        << QString("always_comb")
        << QString("always_ff")
        << QString("always_latch")
        << QString("and")
        << QString("assert")
        << QString("assign")
        << QString("assume")
        << QString("automatic")
        << QString("before")
        << QString("begin")
        << QString("bind")
        << QString("bins")
        << QString("binsof")
        << QString("bit")
        << QString("break")
        << QString("buf")
        << QString("bufif0")
        << QString("bufif1")
        << QString("byte")
        << QString("case")
        << QString("casex")
        << QString("casez")
        << QString("cell")
        << QString("chandle")
        << QString("checker")
        << QString("class")
        << QString("clocking")
        << QString("cmos")
        << QString("config")
        << QString("const")
        << QString("constraint")
        << QString("context")
        << QString("continue")
        << QString("cover")
        << QString("covergroup")
        << QString("coverpoint")
        << QString("cross")
        << QString("deassign")
        << QString("default")
        << QString("defparam")
        << QString("design")
        << QString("disable")
        << QString("dist")
        << QString("do")
        << QString("edge")
        << QString("else")
        << QString("end")
        << QString("endcase")
        << QString("endchecker")
        << QString("endclass")
        << QString("endclocking")
        << QString("endconfig")
        << QString("endfunction")
        << QString("endgenerate")
        << QString("endgroup")
        << QString("endinterface")
        << QString("endmodule")
        << QString("endpackage")
        << QString("endprimitive")
        << QString("endprogram")
        << QString("endproperty")
        << QString("endspecify")
        << QString("endsequence")
        << QString("endtable")
        << QString("endtask")
        << QString("enum")
        << QString("event")
        << QString("eventually")
        << QString("expect")
        << QString("export")
        << QString("extends")
        << QString("extern")
        << QString("final")
        << QString("first_match")
        << QString("for")
        << QString("force")
        << QString("foreach")
        << QString("forever")
        << QString("fork")
        << QString("forkjoin")
        << QString("function")
        << QString("generate")
        << QString("genvar")
        << QString("global")
        << QString("highz0")
        << QString("highz1")
        << QString("if")
        << QString("iff")
        << QString("ifnone")
        << QString("ignore_bins")
        << QString("illegal_bins")
        << QString("implements")
        << QString("implies")
        << QString("import")
        << QString("incdir")
        << QString("include")
        << QString("initial")
        << QString("inout")
        << QString("input")
        << QString("inside")
        << QString("instance")
        << QString("int")
        << QString("integer")
        << QString("interconnect")
        << QString("interface")
        << QString("intersect")
        << QString("join")
        << QString("join_any")
        << QString("join_none")
        << QString("large")
        << QString("let")
        << QString("liblist")
        << QString("library")
        << QString("local")
        << QString("localparam")
        << QString("logic")
        << QString("longint")
        << QString("macromodule")
        << QString("matches")
        << QString("medium")
        << QString("modport")
        << QString("module")
        << QString("nand")
        << QString("negedge")
        << QString("nettype")
        << QString("new")
        << QString("nexttime")
        << QString("nmos")
        << QString("nor")
        << QString("noshowcancelled")
        << QString("not")
        << QString("notif0")
        << QString("notif1")
        << QString("null")
        << QString("or")
        << QString("output")
        << QString("package")
        << QString("packed")
        << QString("parameter")
        << QString("pmos")
        << QString("posedge")
        << QString("primitive")
        << QString("priority")
        << QString("program")
        << QString("property")
        << QString("protected")
        << QString("pull0")
        << QString("pull1")
        << QString("pulldown")
        << QString("pullup")
        << QString("pulsestyle_ondetect")
        << QString("pulsestyle_onevent")
        << QString("pure")
        << QString("rand")
        << QString("randc")
        << QString("randcase")
        << QString("randsequence")
        << QString("rcmos")
        << QString("real")
        << QString("realtime")
        << QString("ref")
        << QString("reg")
        << QString("reject_on")
        << QString("release")
        << QString("repeat")
        << QString("restrict")
        << QString("return")
        << QString("rnmos")
        << QString("rpmos")
        << QString("rtran")
        << QString("rtranif0")
        << QString("rtranif1")
        << QString("s_always")
        << QString("s_eventually")
        << QString("s_nexttime")
        << QString("s_until")
        << QString("s_until_with")
        << QString("scalared")
        << QString("sequence")
        << QString("shortint")
        << QString("shortreal")
        << QString("showcancelled")
        << QString("signed")
        << QString("small")
        << QString("soft")
        << QString("solve")
        << QString("specify")
        << QString("specparam")
        << QString("static")
        << QString("string")
        << QString("strong")
        << QString("strong0")
        << QString("strong1")
        << QString("struct")
        << QString("super")
        << QString("supply0")
        << QString("supply1")
        << QString("sync_accept_on")
        << QString("sync_reject_on")
        << QString("table")
        << QString("tagged")
        << QString("task")
        << QString("this")
        << QString("throughout")
        << QString("time")
        << QString("timeprecision")
        << QString("timeunit")
        << QString("tran")
        << QString("tranif0")
        << QString("tranif1")
        << QString("tri")
        << QString("tri0")
        << QString("tri1")
        << QString("triand")
        << QString("trior")
        << QString("trireg")
        << QString("type")
        << QString("typedef")
        << QString("union")
        << QString("unique")
        << QString("unique0")
        << QString("unsigned")
        << QString("until")
        << QString("until_with")
        << QString("untyped")
        << QString("use")
        << QString("uwire")
        << QString("var")
        << QString("vectored")
        << QString("virtual")
        << QString("void")
        << QString("wait")
        << QString("wait_order")
        << QString("wand")
        << QString("weak")
        << QString("weak0")
        << QString("weak1")
        << QString("while")
        << QString("wildcard")
        << QString("wire")
        << QString("with")
        << QString("within")
        << QString("wor")
        << QString("xnor")
        << QString("xor"));

    return !keywords.contains(ident) && identRegExp.exactMatch(ident);
}

void MainWindow::ioListAdd()
{
    QString sigName;

    do
    {
        // Check if the signal was a duplicate.
        if(!sigName.isEmpty())
        {
            QMessageBox::critical(this, tr("Duplicate Signal Name"),
                tr("A signal named \"%1\" already exists. "
                   "Please choose a different signal name.").arg(sigName));
        }

        // Check for a cancel.
        bool ok = false;

        // Get a new signal name.
        sigName = QInputDialog::getText(this, tr("Signal Name"),
            tr("Please specify a unique signal name:"),
            QLineEdit::Normal, sigName, &ok);

        // If the dialog was canceled, bail.
        if(!ok)
            return;

        // See if the signal name is valid.
        if(!validIdentifier(sigName))
        {
            QMessageBox::critical(this, tr("Invalid Signal Name"),
                tr("A signal named \"%1\" is not valid. "
                   "Please choose a different signal name.").arg(sigName));
            sigName.clear();
            continue;
        }
    }while(sigName.isEmpty() || signalExists(sigName));

    IOSignal *sigObj = new IOSignal;
    sigObj->setName(sigName);
    sigObj->setDirection(IOSignal::Direction_Input);
    sigObj->setType(IOSignal::Type_Wire);
    sigObj->setSize(1);

    mIOSignalModel->append(sigObj);
    ioListChanged();
}

void MainWindow::ioListUp()
{
    QModelIndexList rows = ui->ioList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mIOSignalModel->up(rows.first().row());
    ioListChanged();
}

void MainWindow::ioListDown()
{
    QModelIndexList rows = ui->ioList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mIOSignalModel->down(rows.first().row());
    ioListChanged();
}

void MainWindow::ioListDelete()
{
    QModelIndexList rows = ui->ioList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mIOSignalModel->remove(rows.first().row());
    ioListChanged();
}

void MainWindow::ioListChanged()
{
    QModelIndexList rows = ui->ioList->selectionModel()->selectedRows();

    if(rows.isEmpty())
    {
        ui->ioListUp->setEnabled(false);
        ui->ioListDown->setEnabled(false);
        ui->ioListDelete->setEnabled(false);
        ui->ioSignal->setEnabled(false);
        ui->ioDirection->setEnabled(false);
        ui->ioType->setEnabled(false);
        ui->ioSize->setEnabled(false);

        bool sigNameBlocked = ui->ioSignal->blockSignals(true);
        bool sigDirectionBlocked = ui->ioDirection->blockSignals(true);
        bool sigTypeBlocked = ui->ioType->blockSignals(true);
        bool sigSizeBlocked = ui->ioSize->blockSignals(true);

        ui->ioSignal->clear();
        ui->ioDirection->setCurrentIndex(0);
        ui->ioType->setCurrentIndex(0);
        ui->ioSize->setValue(1);

        ui->ioSignal->blockSignals(sigNameBlocked);
        ui->ioDirection->blockSignals(sigDirectionBlocked);
        ui->ioType->blockSignals(sigTypeBlocked);
        ui->ioSize->blockSignals(sigSizeBlocked);
    }
    else
    {
        int row = rows.first().row();

        IOSignal *sig = mIOSignalModel->at(row);

        ui->ioListUp->setEnabled(row > 0);
        ui->ioListDown->setEnabled(row < (mIOSignalModel->rowCount() - 1));
        ui->ioListDelete->setEnabled(true);
        ui->ioSignal->setEnabled(true);
        ui->ioDirection->setEnabled(true);
        ui->ioType->setEnabled(true);
        ui->ioSize->setEnabled(true);

        if(sig)
        {
            bool sigNameBlocked = ui->ioSignal->blockSignals(true);
            bool sigDirectionBlocked = ui->ioDirection->blockSignals(true);
            bool sigTypeBlocked = ui->ioType->blockSignals(true);
            bool sigSizeBlocked = ui->ioSize->blockSignals(true);

            ui->ioSignal->setText(sig->name());

            switch(sig->direction())
            {
            case IOSignal::Direction_Output:
                ui->ioDirection->setCurrentIndex(1);
                break;
            case IOSignal::Direction_Inout:
                ui->ioDirection->setCurrentIndex(2);
                break;
            default:
                ui->ioDirection->setCurrentIndex(0);
                break;
            }

            switch(sig->type())
            {
            case IOSignal::Type_Reg:
                ui->ioType->setCurrentIndex(1);
                break;
            case IOSignal::Type_Logic:
                ui->ioType->setCurrentIndex(2);
                break;
            default:
                ui->ioType->setCurrentIndex(0);
                break;
            }

            ui->ioSize->setValue(sig->size());

            ui->ioSignal->blockSignals(sigNameBlocked);
            ui->ioDirection->blockSignals(sigDirectionBlocked);
            ui->ioType->blockSignals(sigTypeBlocked);
            ui->ioSize->blockSignals(sigSizeBlocked);
        }
    }

    instUpdate();
}

void MainWindow::ioListUpdate()
{
    QModelIndexList rows = ui->ioList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    int row = rows.first().row();

    IOSignal *sig = mIOSignalModel->at(row);
    if(!sig)
        return;

    sig->setName(ui->ioSignal->text());

    switch(ui->ioDirection->currentIndex())
    {
    case 1:
        sig->setDirection(IOSignal::Direction_Output);
        break;
    case 2:
        sig->setDirection(IOSignal::Direction_Inout);
        break;
    default:
        sig->setDirection(IOSignal::Direction_Input);
        break;
    }

    switch(ui->ioType->currentIndex())
    {
    case 1:
        sig->setType(IOSignal::Type_Reg);
        break;
    case 2:
        sig->setType(IOSignal::Type_Logic);
        break;
    default:
        sig->setType(IOSignal::Type_Wire);
        break;
    }

    sig->setSize(ui->ioSize->value());
    mIOSignalModel->update(row);
    instUpdate();
}

void MainWindow::instUpdate()
{
    QStringList decl;

    for(int i = 0; i < mIOSignalModel->rowCount(); i++)
    {
        IOSignal *sig = mIOSignalModel->at(i);

        QString size;

        if(sig->size() > 1)
            size = QObject::tr(" [%1:0]").arg(sig->size() - 1);

        decl.append(tr("logic%1 %2;").arg(size).arg(sig->name()));
    }

    QStringList ports;
    /// @todo Change this to the setting.
    QString indent = "  ";

    for(int i = 0; i < mIOSignalModel->rowCount(); i++)
    {
        IOSignal *sig = mIOSignalModel->at(i);
        ports.append(tr("%1.%2(%3)").arg(indent).arg(sig->name()).arg(sig->name()));
    }

    ui->instCode->setPlainText(tr("%1\n\n%2 inst_name_here(\n%3\n);").arg(decl.join("\n")).arg("test").arg(ports.join(",\n")));
}

void MainWindow::controlListAdd()
{
    QString sigName;

    do
    {
        // Check if the signal was a duplicate.
        if(!sigName.isEmpty())
        {
            QMessageBox::critical(this, tr("Duplicate Signal Name"),
                tr("A signal named \"%1\" already exists. "
                   "Please choose a different signal name.").arg(sigName));
        }

        // Check for a cancel.
        bool ok = false;

        // Get a new signal name.
        sigName = QInputDialog::getText(this, tr("Signal Name"),
            tr("Please specify a unique signal name:"),
            QLineEdit::Normal, sigName, &ok);

        // If the dialog was canceled, bail.
        if(!ok)
            return;

        // See if the signal name is valid.
        if(!validIdentifier(sigName))
        {
            QMessageBox::critical(this, tr("Invalid Signal Name"),
                tr("A signal named \"%1\" is not valid. "
                   "Please choose a different signal name.").arg(sigName));
            sigName.clear();
            continue;
        }
    }while(sigName.isEmpty() || signalExists(sigName));

    ControlSignal *sigObj = new ControlSignal;
    sigObj->setName(sigName);

    mControlSignalModel->append(sigObj);
    controlListChanged();
}

void MainWindow::controlListUp()
{
    QModelIndexList rows = ui->controlList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mControlSignalModel->up(rows.first().row());
    controlListChanged();
}

void MainWindow::controlListDown()
{
    QModelIndexList rows = ui->controlList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mControlSignalModel->down(rows.first().row());
    controlListChanged();
}

void MainWindow::controlListDelete()
{
    QModelIndexList rows = ui->controlList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mControlSignalModel->remove(rows.first().row());
    controlListChanged();
}

void MainWindow::controlListChanged()
{
    QModelIndexList rows = ui->controlList->selectionModel()->selectedRows();

    if(rows.isEmpty())
    {
        ui->controlListUp->setEnabled(false);
        ui->controlListDown->setEnabled(false);
        ui->controlListDelete->setEnabled(false);
        ui->controlListRename->setEnabled(false);
    }
    else
    {
        int row = rows.first().row();

        ui->controlListUp->setEnabled(row > 0);
        ui->controlListDown->setEnabled(row < (mControlSignalModel->rowCount() - 1));
        ui->controlListDelete->setEnabled(true);
        ui->controlListRename->setEnabled(true);
    }
}

void MainWindow::controlListRename()
{
    QModelIndexList rows = ui->controlList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    int row = rows.first().row();

    ControlSignal *sig = mControlSignalModel->at(row);
    if(!sig)
        return;

    QString sigName;

    do
    {
        // Check if the signal was a duplicate.
        if(!sigName.isEmpty())
        {
            QMessageBox::critical(this, tr("Duplicate Signal Name"),
                tr("A signal named \"%1\" already exists. "
                   "Please choose a different signal name.").arg(sigName));
        }

        // Use the previous name.
        if(sigName.isEmpty())
            sigName = sig->name();

        // Check for a cancel.
        bool ok = false;

        // Get a new signal name.
        sigName = QInputDialog::getText(this, tr("Signal Name"),
            tr("Please specify a unique signal name:"),
            QLineEdit::Normal, sigName, &ok);

        // If the dialog was canceled, bail.
        if(!ok)
            return;

        // See if the signal name is valid.
        if(!validIdentifier(sigName))
        {
            QMessageBox::critical(this, tr("Invalid Signal Name"),
                tr("A signal named \"%1\" is not valid. "
                   "Please choose a different signal name.").arg(sigName));
            sigName.clear();
            continue;
        }
    }while(sigName.isEmpty() || signalExists(sigName));

    sig->setName(sigName);

    mControlSignalModel->update(row);
}

void MainWindow::stateListAdd()
{
    QString sigName;

    do
    {
        // Check if the signal was a duplicate.
        if(!sigName.isEmpty())
        {
            QMessageBox::critical(this, tr("Duplicate State Name"),
                tr("A state or signal named \"%1\" already exists. "
                   "Please choose a different state name.").arg(sigName));
        }

        // Check for a cancel.
        bool ok = false;

        // Get a new signal name.
        sigName = QInputDialog::getText(this, tr("State Name"),
            tr("Please specify a unique state name:"),
            QLineEdit::Normal, sigName, &ok);

        // If the dialog was canceled, bail.
        if(!ok)
            return;

        // See if the signal name is valid.
        if(!validIdentifier(sigName))
        {
            QMessageBox::critical(this, tr("Invalid State Name"),
                tr("A state named \"%1\" is not valid. "
                   "Please choose a different state name.").arg(sigName));
            sigName.clear();
            continue;
        }
    }while(sigName.isEmpty() || signalExists(sigName));

    State *sigObj = new State;
    sigObj->setName(sigName);

    mStateModel->append(sigObj);
    stateListChanged();
}

void MainWindow::stateListUp()
{
    QModelIndexList rows = ui->stateList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mStateModel->up(rows.first().row());
    stateListChanged();
}

void MainWindow::stateListDown()
{
    QModelIndexList rows = ui->stateList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mStateModel->down(rows.first().row());
    stateListChanged();
}

void MainWindow::stateListDelete()
{
    QModelIndexList rows = ui->stateList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    mStateModel->remove(rows.first().row());
    stateListChanged();
}

void MainWindow::stateListChanged()
{
    QModelIndexList rows = ui->stateList->selectionModel()->selectedRows();

    ui->stateReset->setEnabled(mStateModel->rowCount());

    if(rows.isEmpty())
    {
        ui->stateListUp->setEnabled(false);
        ui->stateListDown->setEnabled(false);
        ui->stateListDelete->setEnabled(false);
        ui->stateName->setEnabled(false);
        ui->stateCode->setEnabled(false);

        bool stateNameBlocked = ui->stateName->blockSignals(true);
        bool stateCodeBlocked = ui->stateCode->blockSignals(true);

        ui->stateName->clear();
        ui->stateCode->clear();

        ui->stateName->blockSignals(stateNameBlocked);
        ui->stateCode->blockSignals(stateCodeBlocked);
    }
    else
    {
        int row = rows.first().row();

        State *sig = mStateModel->at(row);

        ui->stateListUp->setEnabled(row > 0);
        ui->stateListDown->setEnabled(row < (mStateModel->rowCount() - 1));
        ui->stateListDelete->setEnabled(true);
        ui->stateName->setEnabled(true);
        ui->stateCode->setEnabled(true);

        if(sig)
        {
            bool stateNameBlocked = ui->stateName->blockSignals(true);
            bool stateCodeBlocked = ui->stateCode->blockSignals(true);

            ui->stateName->setText(sig->name());
            ui->stateCode->setPlainText(sig->code());

            ui->stateName->blockSignals(stateNameBlocked);
            ui->stateCode->blockSignals(stateCodeBlocked);
        }
    }
}

void MainWindow::stateListUpdate()
{
    QModelIndexList rows = ui->stateList->selectionModel()->selectedRows();
    if(rows.isEmpty())
        return;

    int row = rows.first().row();

    State *sig = mStateModel->at(row);
    if(!sig)
        return;

    sig->setName(ui->stateName->text());
    sig->setCode(ui->stateCode->toPlainText());

    mStateModel->update(row);
}

void MainWindow::About()
{
    QMessageBox::about(this, tr("State of Flux v1.0"), tr(
        "State of Flux\n"
        "Version 1.0 build 1\n"
        "Copyright (C) 2015 John Eric Martin\n"
        "http://www.logicfault.com"));
}

void MainWindow::Generate()
{
    // Check if the file needs to be saved first.
    // Check for saving.
    if(mCleanXml != SaveXml() || mProjectPath.isEmpty())
    {
        int button = QMessageBox::question(this, tr("Save Project?"),
            tr("The project has unsaved changes. You must save it first. Save?"),
            QMessageBox::Yes, QMessageBox::No);
        if(button == QMessageBox::No)
            return;

        if(mProjectPath.isEmpty())
        {
            QString path = QFileDialog::getSaveFileName(this, tr("Save Project"),
                QString(), tr("State of Flux FSM (*.fsm)"));
            if(path.isEmpty())
                return;

            mProjectPath = path;
        }

        QFile project(mProjectPath);
        if(!project.open(QIODevice::WriteOnly) || !project.write(SaveXml().toUtf8()))
        {
            QMessageBox::critical(this, tr("Save Failed"), tr("Failed to save the project."));
            return;
        }

        mCleanXml = SaveXml();

        project.close();
    }

    QString code;
    QString ext;

    switch(ui->outputLang->currentIndex())
    {
    case 1:
        code = GenerateVHDL();
        ext = "vhd";
        break;
    default:
        code = GenerateVerilog();
        ext = "v";
        break;
    }

    QString path = QString("%1/%2.%3").arg(
        QFileInfo(mProjectPath).absolutePath()).arg(
        QFileInfo(mProjectPath).baseName()).arg(ext);

    if(QFileInfo(path).exists())
    {
        int button = QMessageBox::question(this, tr("Overwrite Generated Module"),
            tr("The generated module already exists. Overwrite?"),
            QMessageBox::Yes, QMessageBox::No);
        if(button == QMessageBox::No)
            return;
    }

    // Save the generated file.
    QFile genFile(path);
    if(!genFile.open(QIODevice::WriteOnly) || !genFile.write(code.toUtf8()))
    {
        QMessageBox::critical(this, tr("Generate Failed"), tr("Failed to save the generated module."));
    }

    genFile.close();
}

QString MainWindow::GenerateVerilog() const
{
    QString module = QFileInfo(mProjectPath).baseName();

    QString src;

    QString header = trimBlankLines(ui->headerCode->toPlainText());
    if(!header.isEmpty())
    {
        src += header;
        src += "\n";
    }

    src += "`timescale 1ns / 1ps\n";

    if(ui->defaultNettype->isChecked())
        src += "`default_nettype none\n";

    src += "\n";
    src += QString("module %1(\n").arg(module);

    QStringList io;

    for(int i = 0; i < mIOSignalModel->rowCount(); i++)
        io.append(indentLine(mIOSignalModel->at(i)->declaration()));

    src += io.join(",\n");
    src += "\n";
    src += QString("%1);\n").arg(indent());

    int stateCount = mStateModel->rowCount();
    int stateBits = ceil(log2(stateCount));
    if(stateCount <= 1)
        stateBits = 1;

    src += "\n";
    src += indent() + "// States\n";

    QStringList states;

    for(int i = 0; i < mStateModel->rowCount(); i++)
        states.append(indentLine(mStateModel->at(i)->declaration(stateBits, i)));

    src += states.join("\n");
    src += "\n\n";
    src += indent() + "// Control Signals\n";

    QStringList controlSignals;

    for(int i = 0; i < mControlSignalModel->rowCount(); i++)
        controlSignals.append(indentLine(mControlSignalModel->at(i)->declaration()));

    src += controlSignals.join("\n");
    src += "\n\n";
    src += indent() + QString("reg [%1:0] current_state;\n").arg(stateBits - 1);
    src += indent() + QString("reg [%1:0] next_state;\n").arg(stateBits - 1);

    src += "\n";
    if(ui->resetType->currentText().toLower() == "async")
    {
        src += indent() + QString("always @ (posedge %1, posedge %2)\n").arg(
            ui->clockSignal->currentText()).arg(ui->resetSignal->currentText());
    }
    else
    {
        src += indent() + QString("always @ (posedge %1)\n").arg(
            ui->clockSignal->currentText());
    }

    src += indent() + "begin\n";
    src += indent(2) + QString("if(%1) begin\n").arg(
        ui->resetSignal->currentText());
    src += indent(3) + QString("current_state <= %1;\n").arg(ui->stateReset->currentText());
    src += indent(2) + "end else begin\n";
    src += indent(3) + "current_state <= next_state;\n";
    src += indent(2) + "end\n";
    src += indent() + "end\n";
    src += "\n";

    if(ui->asciiStates->isChecked())
    {
        int maxStateLen = 0;

        for(int i = 0; i < mStateModel->rowCount(); i++)
        {
            int stateLen = mStateModel->at(i)->name().length();
            if(stateLen > maxStateLen)
                maxStateLen = stateLen;
        }

        src += indent() + "// synopsys translate_off\n";
        src += indent() + QString("reg [%1:0] ascii_current_state;\n").arg(maxStateLen * 8 - 1);
        src += indent() + QString("reg [%1:0] ascii_next_state;\n").arg(maxStateLen * 8 - 1);
        src += "\n";
        src += indent() + "always @(current_state)\n";
        src += indent() + "begin\n";
        src += indent(2) + "case(current_state)\n";

        for(int i = 0; i < mStateModel->rowCount(); i++)
        {
            QString state = mStateModel->at(i)->name();
            src += indent(3) + QString("%1 ascii_current_state = \"%2\";\n").arg(
                padLine(state + ":", maxStateLen + 1)).arg(padLine(state, maxStateLen));
        }

        src += indent(2) + "endcase\n";
        src += indent() + "end\n";
        src == "\n";
        src += indent() + "always @(next_state)\n";
        src += indent() + "begin\n";
        src += indent(2) + "case(next_state)\n";

        for(int i = 0; i < mStateModel->rowCount(); i++)
        {
            QString state = mStateModel->at(i)->name();
            src += indent(3) + QString("%1 ascii_next_state = \"%2\";\n").arg(
                padLine(state + ":", maxStateLen + 1)).arg(padLine(state, maxStateLen));
        }

        src += indent(2) + "endcase\n";
        src += indent() + "end\n";

        src += indent() + "// synopsys translate_on\n";
    }

    QString datapath = trimBlankLines(ui->dataCode->toPlainText());

    if(!datapath.isEmpty())
    {
        src += "\n";
        src += indentLines(datapath) + "\n";
    }

    QString tasks = trimBlankLines(ui->taskCode->toPlainText());

    if(!tasks.isEmpty())
    {
        src += "\n";
        src += indentLines(tasks) + "\n";
    }

    src += "\n";
    src += indent() + "always @ (*)\n";
    src += indent() + "begin\n";
    src += indent(2) + "// Default values\n";
    src += indent(2) + "next_state = current_state;\n";

    QString stateDefaults = trimBlankLines(ui->stateDefaultsCode->toPlainText());

    if(!stateDefaults.isEmpty())
    {
        src += indentLines(stateDefaults, 2) + "\n";
    }

    for(int i = 0; i < mControlSignalModel->rowCount(); i++)
        src += indent(2) + QString("%1 = 0;\n").arg(mControlSignalModel->at(i)->name());

    src += "\n";
    src += indent(2) + "case(current_state)\n";

    for(int i = 0; i < mStateModel->rowCount(); i++)
    {
        State *s = mStateModel->at(i);
        src += indent(3) + QString("%1: begin\n").arg(s->name());

        QString code = trimBlankLines(s->code());

        src += indentLines(code, 4) + "\n";
        src += indent(3) + QString("end // %1\n").arg(s->name());
    }

    src += indent(2) + "endcase\n";
    src += indent() + "end\n";

    src += "endmodule\n";

    if(ui->defaultNettype->isChecked())
        src += "\n`default_nettype wire\n";

    // std::cout << src.toUtf8().constData() << std::endl;

    return src;
}

QString MainWindow::GenerateVHDL() const
{
    /// @todo Implement VHDL
    return QString();
}

QString MainWindow::indent(int level) const
{
    QString str;
    int c = ui->indentValue->value();

    if(ui->indentType->currentIndex() == 0) // Spaces
    {
        for(int i = 0; i < level; i++)
        {
            for(int j = 0; j < c; j++)
                str += " ";
        }
    }
    else // Tabs
    {
        for(int i = 0; i < level; i++)
        {
            for(int j = 0; j < c; j++)
                str += "\t";
        }
    }

    return str;
}

QString MainWindow::indentLine(const QString& line, int level) const
{
    return QString("%1%2").arg(indent(level)).arg(line);
}

QString MainWindow::indentLines(const QString& src, int level) const
{
    QStringList lines = src.split("\n");
    if(lines.isEmpty())
        return QString();

    QStringList newLines;

    foreach(QString line, lines)
        newLines.append(indentLine(line, level));

    return newLines.join("\n");
}

QString MainWindow::padLine(const QString& line, int length) const
{
    QString str = line;

    length -= str.length();

    while(length-- > 0)
        str += " ";

    return str;
}

QString MainWindow::trimBlankLines(const QString& line) const
{
    if(line.isEmpty())
        return QString();

    QString str = line;
    while(str.at(str.length() - 1).isSpace())
        str.truncate(str.length() - 1);

    return str;
}
