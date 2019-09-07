/***************************************************************************
 *   Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include <qpainter.h>
#include <qprinter.h>
#include <qscrollarea.h>
#include <qslider.h>
#include <qdebug.h>
#include <qlayout.h>
#include <qtoolbar.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <QKeyEvent>

#include <KConfigDialog>
#include <KStatusBar>

#include <KAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KFileDialog>
#include <KToolBar>

#include <KLocale>
#include <kmenubar.h>

#include "klogic4.h"
#include "KlogicGlobal.h"
#include "settings.h"
#include "DeviceType.h"
#include "CircuitWidget.h"
#include "ClientWidget.h"
#include "OscWidget.h"
#include "dlgLib.h"
#include "circuit.h"
#include "Simulator.h"

klogic4 * klogic4::instance = 0;

klogic4::klogic4()
        : KXmlGuiWindow(),
          clientWidget(0),
          oscWidget(0),
          m_printer(0)
{
	if (instance != 0) {
		qFatal("Instance does already exist.");
	}
    XDevice::m_oBasicImageCache.init();

    Circuit *theMainCircuit = (Circuit *)DeviceFactory::getInstance()->createDevice(NULL, DeviceType::fSUB, QPoint(0,0));
    clientWidget = new ClientWidget(theMainCircuit);

    simulationMode = 0;
	simulationStepTime = 100;
	simulationBurstCount = 0;
	function = 0;
	fINAction = 0;
	fOUTAction = 0;
	activeLib = 0;

	runtimeShortcutWarning = true;
	connect(&shortcutWarningTimer, SIGNAL(timeout()), this, SLOT(enableShortcutWarning()));

	instance = this;

    // accept dnd
    setAcceptDrops(true);

    // tell the KXmlGuiWindow that this is indeed the main widget
    setCentralWidget(clientWidget);

    // then, setup our actions
    setupActions();

    // add a status bar
    statusBar()->show();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI(Default, "klogic4ui.rc");

    adjustToolbarsNotHandledByXml();

    // Do initial KLogic settings
    performInitialSettings();

}

klogic4::~klogic4()
{
}

klogic4 * klogic4::getInstance() {
	return instance;
}

void klogic4::performInitialSettings() {
	setSimStepTime(100);
	setSimBurstCount(1);
	setSimPermanent();
	Simulator::getInstance()->simulationParametersChanged();

	connect(Simulator::getInstance(), SIGNAL(simStepped()), this, SLOT(repaintClientWidgets()));

	connect(clientWidget->getCircuitWidget(), SIGNAL(circuitDeleted(Circuit *)), this, SLOT(circuitDeleted(Circuit *)));
	connect(clientWidget->getCircuitWidget(), SIGNAL(circuitChanged(Circuit *)), this, SLOT(repaintClientWidgets(Circuit *)));

	connect(clientWidget->getCircuitWidget(), SIGNAL(addDeviceToLibrary(XDevice *)), this, SLOT(addDeviceToLibrary(XDevice *)));
	connect(clientWidget->getCircuitWidget(), SIGNAL(ovserveCircuit(Circuit *)), this, SLOT(oscilloscopeAnotherCircuit(Circuit *)));
	connect(clientWidget->getCircuitWidget(), SIGNAL(observedChanged(Circuit *)), this, SLOT(oscilloscopeCircuitChanged(Circuit *)));

	connect(clientWidget->getCircuitWidget(), SIGNAL(openClientWidget(Circuit *)), this, SLOT(openClientWidget(Circuit *)));
	connect(clientWidget, SIGNAL(clientWidgetClosed(ClientWidget *)), this, SLOT(clientWidgetClosed(ClientWidget *)));
	clientWidgets.push_back(clientWidget);
}

void klogic4::adjustToolbarsNotHandledByXml() {
	KToolBar *simTb = toolBar("simulationToolBar");
	simTb->setMovable(true);

    // toolbar: simulation time slider
	simTimeSliderLabel = new QLabel(simTb);
	simTimeSliderLabel->setText(i18n("Simulation time (ms)") + ": ");
	simTb->addWidget(simTimeSliderLabel);
	simTimeSlider = new QSlider(simTb);
	simTimeSlider->setOrientation(Qt::Horizontal);
	simTimeSlider->setRange(KlogicGlobal::SIMSTEPTIME_MIN, KlogicGlobal::SIMSTEPTIME_MAX);
	simTimeSlider->setSliderPosition(getSimStepTime());
	simTimeSlider->setInvertedAppearance(true);
	simTimeSlider->setFixedWidth(140);
	connect(simTimeSlider, SIGNAL(valueChanged(int)), this, SLOT(setSimStepTime(int)));
	simTb->addWidget(simTimeSlider);

	KToolBar *libTb = toolBar("libraryToolBar");
	libTb->setMovable(true);

    // toolbar: library chooser
    libraryChooserLabel = new QLabel(libTb);
    libraryChooserLabel->setText(i18n("Library") + ": ");
    libTb->addWidget(libraryChooserLabel);

    libraryChooser = new QComboBox(libTb);
    libraryChooser->setFixedWidth(140);
    connect(libraryChooser, SIGNAL(activated(int)), SLOT(libraryChanged(int)));
    libTb->addWidget(libraryChooser);

    // toolbar: library device chooser
    libraryDeviceChooserLabel = new QLabel(libTb);
    libraryDeviceChooserLabel->setText("    " + i18n("Device") + ": ");
    libTb->addWidget(libraryDeviceChooserLabel);

    libraryDeviceChooser = new QComboBox(libTb);
    libraryDeviceChooser->setFixedWidth(140);
    connect(libraryDeviceChooser, SIGNAL(activated(int)), SLOT(libraryDeviceSelected()));
    libTb->addWidget(libraryDeviceChooser);

    // move the most important toolbar into a new line
	addToolBarBreak();
	KToolBar *logicTb = toolBar("logicToolBar");
	logicTb->setMovable(true);
	removeToolBar(logicTb);
	addToolBar(logicTb);
	logicTb->setVisible(true);
}

void klogic4::setupActions()
{
    // that appers in the menu <file> by KDE4 magic
    KStandardAction::openNew(this, SLOT(newCircuit()), actionCollection());
    KStandardAction::open(this, SLOT(openFile()), actionCollection());
    KStandardAction::save(this, SLOT(saveFile()), actionCollection());
    KStandardAction::saveAs(this, SLOT(saveFileAs()), actionCollection());
    //KStandardAction::print(this, SLOT(print()), actionCollection());
    KStandardAction::quit(this, SLOT(quit()), actionCollection());

    KAction *action;

    // Iterations are not welcome here somehow.. Always the same stupid code.. And grouping by hand??
    QActionGroup *functionActionGroup = new QActionGroup(this);
    action = new KAction(KIcon("klogic4-fSELECT"), i18n("&Selection Mode"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_S);
    action->setToolTip("Select, copy and paste groups of devices and wires.");
    action->setCheckable(true);
    actionCollection()->addAction("actionSelectionMode", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(selectionMode()));

    action = new KAction(KIcon("klogic4-fCONNECT"), i18n("&Connection Mode"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_C);
    action->setToolTip("Connect devices using wires.");
    action->setCheckable(true);
    actionCollection()->addAction("actionConnectionMode", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(connectionMode()));
    action->setChecked(true);
    connectionMode();
    
    action = new KAction(KIcon("klogic4-fEQUATION"), i18n("&Equation"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_E);
    action->setToolTip("Equation device with boolean algebra formulas.");
    action->setCheckable(true);
    actionCollection()->addAction("actionfEQuation", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fEQUATION()));

    devANDAction = new KAction(KIcon("klogic4-fAND"), i18n("&AND"), this);
    devANDAction->setShortcut(Qt::CTRL + Qt::Key_A);
    devANDAction->setToolTip("AND device. Output becomes true if all inputs are true.");
    devANDAction->setCheckable(true);
    actionCollection()->addAction("actionfAND", devANDAction);
    functionActionGroup->addAction(devANDAction);
    connect(devANDAction, SIGNAL(triggered(bool)), this, SLOT(fAND()));

    action = new KAction(KIcon("klogic4-fNAND"), i18n("&NAND"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_N);
    action->setToolTip("NAND device. Output becomes false if all inputs are true.");
    action->setCheckable(true);
    actionCollection()->addAction("actionfNAND", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fNAND()));

    action = new KAction(KIcon("klogic4-fOR"), i18n("&OR"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_O);
    action->setToolTip("OR device. Output becomes true if one or more inputs are true.");
    action->setCheckable(true);
    actionCollection()->addAction("actionfOR", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fOR()));

    action = new KAction(KIcon("klogic4-fNOR"), i18n("NOR"), this);
    action->setCheckable(true);
    action->setToolTip("NOR device. Output becomes false if one or more inputs are true.");
    actionCollection()->addAction("actionfNOR", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fNOR()));

    action = new KAction(KIcon("klogic4-fXOR"), i18n("&XOR"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_X);
    action->setToolTip("XOR device. Output becomes true if exactly one input is true.");
    action->setCheckable(true);
    actionCollection()->addAction("actionfXOR", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fXOR()));

    action = new KAction(KIcon("klogic4-fNOT"), i18n("&Inverter"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_I);
    action->setToolTip("NOT device. Output becomes true if input is false.");
    action->setCheckable(true);
    actionCollection()->addAction("actionfNOT", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fNOT()));

    action = new KAction(KIcon("klogic4-fRS.png"), i18n("&RS flipflop"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_R);
    action->setCheckable(true);
    actionCollection()->addAction("actionfRS", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fRS()));

    action = new KAction(KIcon("klogic4-fJK.png"), i18n("&JK flipflop"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_J);
    action->setCheckable(true);
    actionCollection()->addAction("actionfJK", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fJK()));

    action = new KAction(KIcon("klogic4-fDFF.png"), i18n("&D flipflop"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_D);
    action->setCheckable(true);
    actionCollection()->addAction("actionfDFF", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fDFF()));

    action = new KAction(KIcon("klogic4-fTFF.png"), i18n("&T flipflop"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_T);
    action->setCheckable(true);
    actionCollection()->addAction("actionfTFF", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fTFF()));

    action = new KAction(KIcon("klogic4-fRAM.png"), i18n("RA&M 32x8"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_M);
    action->setCheckable(true);
    actionCollection()->addAction("actionfRAM", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fRAM()));

    action = new KAction(KIcon("klogic4-fTRI.png"), i18n("Tristate output"), this);
    action->setCheckable(true);
    actionCollection()->addAction("actionfTRI", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fTRI()));

    action = new KAction(KIcon("klogic4-fBUS.png"), i18n("Bus Connector"), this);
    action->setCheckable(true);
    actionCollection()->addAction("actionfBUS", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fBUS()));

    action = new KAction(KIcon("klogic4-fOSC.png"), i18n("Os&cillator"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_C);
    action->setCheckable(true);
    actionCollection()->addAction("actionfOSC", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fOSC()));

    action = new KAction(KIcon("klogic4-fSWI.png"), i18n("&Switch"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_S);
    action->setCheckable(true);
    actionCollection()->addAction("actionfSWI", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fSWI()));

    action = new KAction(KIcon("klogic4-fSS.png"), i18n("&7-Segment"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_7);
    action->setCheckable(true);
    actionCollection()->addAction("actionfSS", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fSS()));

    action = new KAction(KIcon("klogic4-fLED.png"), i18n("&LED"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_L);
    action->setCheckable(true);
    actionCollection()->addAction("actionfLED", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fLED()));

    action = new KAction(KIcon("klogic4-fPWR.png"), i18n("&Voltage Connector"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_V);
    action->setCheckable(true);
    actionCollection()->addAction("actionfPWR", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fPWR()));

    action = new KAction(KIcon("klogic4-fTXT.png"), i18n("&Text"), this);
    action->setCheckable(true);
    actionCollection()->addAction("actionfTXT", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fTXT()));

    action = new KAction(KIcon("klogic4-fSUB.png"), i18n("Sub-Circuit"), this);
    action->setCheckable(true);
    action->setToolTip("Sub circuit consists of another circuit with a defined interface. Double click on the screen to open it.");
    actionCollection()->addAction("actionfSUB", action);
    functionActionGroup->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(fSUB()));

    devFromLibAction = new KAction(KIcon("klogic4-FromLib.png"), i18n("Chosen Device From Library"), this);
    devFromLibAction->setCheckable(true);
    devFromLibAction->setToolTip("Create the device currently chosen from active library.");
    actionCollection()->addAction("actionFromLib", devFromLibAction);
    functionActionGroup->addAction(devFromLibAction);
    connect(devFromLibAction, SIGNAL(triggered(bool)), this, SLOT(fFromLib()));
    devFromLibAction->setVisible(false);

    fINAction = new KAction(KIcon("klogic4-fIN.png"), i18n("&Input"), this);
    fINAction->setCheckable(true);
    functionActionGroup->addAction(fINAction);

    fOUTAction = new KAction(KIcon("klogic4-fOUT.png"), i18n("&Output"), this);
    fOUTAction->setCheckable(true);
    functionActionGroup->addAction(fOUTAction);

    openOscilloscopeAction = new KAction(KIcon("klogic4-Oscilloscope"), i18n("&Oscilloscope"), this);
    openOscilloscopeAction->setShortcut(Qt::CTRL + Qt::Key_O);
    openOscilloscopeAction->setToolTip("Open oscilloscope window.");
    openOscilloscopeAction->setCheckable(true);
    actionCollection()->addAction("actionOscilloscope", openOscilloscopeAction);
    connect(openOscilloscopeAction, SIGNAL(triggered(bool)), this, SLOT(openOscilloscope()));

    action = new KAction(KIcon("klogic4-library-maintenance"), i18n("&Library Maintenance .."), this);
    action->setShortcut(Qt::CTRL + Qt::Key_L);
    actionCollection()->addAction("actionLibraryMaintenance", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(openLibraryMaintenance()));

    simStepByStepAction = new KAction(KIcon("klogic4-SingleStep"), i18n("&Single Simulation Step (Press S-Key)"), this);
    simStepByStepAction->setShortcut(Qt::Key_S);
    simStepByStepAction->setCheckable(true);
    actionCollection()->addAction("actionSingleStep", simStepByStepAction);
    connect(simStepByStepAction, SIGNAL(triggered(bool)), this, SLOT(setSimStepByStep()));

    simAutoAction = new KAction(KIcon("klogic4-AutoStep"), i18n("&Automatic Simulation Stepping (Press A-Key)"), this);
    simAutoAction->setShortcut(Qt::Key_A);
    simAutoAction->setCheckable(true);
    actionCollection()->addAction("actionAutoStep", simAutoAction);
    connect(simAutoAction, SIGNAL(triggered(bool)), this, SLOT(setSimPermanent()));

    connectionMode();
}

void klogic4::openOscilloscope() {
	if (oscWidget) {
		closeOscilloscope();
	} else {
		openOscilloscope(clientWidget->getCircuitWidget()->getActiveCircuit());
	}
}

void klogic4::closeOscilloscope() {
	if (oscWidget) {
		oscWidget->setVisible(false);
		oscWidget->close();
	}
}

void klogic4::openOscilloscope(Circuit *circuit) {
	openOscilloscopeAction->setChecked(true);
	oscWidget = new OscWidget();
	connect(oscWidget, SIGNAL(oscillatorWidgetClosed()), this, SLOT(oscilloscopeClosed()));
	oscWidget->setVisible(true);
	oscWidget->displayCircuit(circuit);

	connect(clientWidget->getCircuitWidget(), SIGNAL(observedChanged(Circuit *)), this, SLOT(oscilloscopeCircuitChanged(Circuit *)));
	connect(clientWidget->getCircuitWidget(), SIGNAL(ovserveCircuit(Circuit *)), this, SLOT(oscilloscopeAnotherCircuit(Circuit *)));
	connect(Simulator::getInstance(), SIGNAL(simStepped()), oscWidget, SLOT(simStep()));
	qDebug() << "Oscillator opened.";
}

void klogic4::oscilloscopeClosed() {
	disconnect(clientWidget->getCircuitWidget(), SIGNAL(observedChanged(Circuit *)), this, SLOT(oscilloscopeCircuitChanged(Circuit *)));
	disconnect(clientWidget->getCircuitWidget(), SIGNAL(ovserveCircuit(Circuit *)), this, SLOT(oscilloscopeAnotherCircuit(Circuit *)));
	disconnect(Simulator::getInstance(), SIGNAL(simStepped()), oscWidget, SLOT(simStep()));
	oscWidget = NULL;
	openOscilloscopeAction->setChecked(false);
	qDebug() << "Oscillator closed.";
}

void klogic4::oscilloscopeAnotherCircuit(Circuit *circuit) {
	if (oscWidget) {
		oscWidget->displayCircuit(circuit);
	}
}

void klogic4::oscilloscopeCircuitChanged(Circuit *circuit) {
	if (oscWidget) {
		oscWidget->circuitChanged(circuit);
	}
}

void klogic4::openClientWidget(Circuit *circuit) {
	ClientWidget *widget = new ClientWidget(circuit);
	widget->setVisible(true);
	clientWidgets.push_back(widget);
	connect(widget->getCircuitWidget(), SIGNAL(circuitDeleted(Circuit *)), this, SLOT(circuitDeleted(Circuit *)));
	connect(widget->getCircuitWidget(), SIGNAL(circuitChanged(Circuit *)), this, SLOT(repaintClientWidgets(Circuit *)));

	connect(widget->getCircuitWidget(), SIGNAL(addDeviceToLibrary(XDevice *)), this, SLOT(addDeviceToLibrary(XDevice *)));

	connect(widget->getCircuitWidget(), SIGNAL(ovserveCircuit(Circuit *)), this, SLOT(oscilloscopeAnotherCircuit(Circuit *)));
	connect(widget->getCircuitWidget(), SIGNAL(observedChanged(Circuit *)), this, SLOT(oscilloscopeCircuitChanged(Circuit *)));

	connect(widget->getCircuitWidget(), SIGNAL(openClientWidget(Circuit *)), this, SLOT(openClientWidget(Circuit *)));
	connect(widget, SIGNAL(clientWidgetClosed(ClientWidget *)), this, SLOT(clientWidgetClosed(ClientWidget *)));
	qDebug() << "client widget opened:" << widget;
}

void klogic4::clientWidgetClosed(ClientWidget *widget) {
	clientWidgets.remove(widget);
	disconnect(widget->getCircuitWidget(), SIGNAL(circuitDeleted(Circuit *)), this, SLOT(circuitDeleted(Circuit *)));
	disconnect(widget->getCircuitWidget(), SIGNAL(circuitChanged(Circuit *)), this, SLOT(repaintClientWidgets(Circuit *)));

	disconnect(widget->getCircuitWidget(), SIGNAL(addDeviceToLibrary(XDevice *)), this, SLOT(addDeviceToLibrary(XDevice *)));

	disconnect(widget->getCircuitWidget(), SIGNAL(ovserveCircuit(Circuit *)), this, SLOT(oscilloscopeAnotherCircuit(Circuit *)));
	disconnect(widget->getCircuitWidget(), SIGNAL(observedChanged(Circuit *)), this, SLOT(oscilloscopeCircuitChanged(Circuit *)));

	disconnect(widget->getCircuitWidget(), SIGNAL(openClientWidget(Circuit *)), this, SLOT(openClientWidget(Circuit *)));
	disconnect(widget, SIGNAL(clientWidgetClosed(ClientWidget *)), this, SLOT(clientWidgetClosed(ClientWidget *)));
	qDebug() << "client widget closed:" << widget;
}

void klogic4::newCircuit() {
    Simulator::getInstance()->setPaused(true);

    if (clientWidget->getCircuitWidget()->isContentChanged()) {
    	if (QMessageBox::Yes == QMessageBox::warning(parentWidget(),
				i18n("New Circuit"),
				i18n("Save changes?"),
				QMessageBox::Yes,
				QMessageBox::No)) {
        	saveFile();
    	}
    }

    // Close all client widgets except the main.
	bool continueClose = true;
	while (continueClose) {
		continueClose = false;
	    for (list<ClientWidget *>::iterator iter = clientWidgets.begin(); iter != clientWidgets.end(); iter++) {
	    	if (clientWidget != *iter)  {
	    		(*iter)->close();
	    		continueClose = true;
	    		break;
	    	}
	    }
	}
   	closeOscilloscope();

    clientWidget->getCircuitWidget()->newCircuit();
    QFileInfo fi(KlogicGlobal::CurrentCircuit::filename);
    KlogicGlobal::CurrentCircuit::filename = fi.dir().path() + "/" + KlogicGlobal::Klogic::emptyFileName;
    setPlainCaption(KlogicGlobal::Klogic::Name + " - " + KlogicGlobal::Klogic::emptyFileName);
    Simulator::getInstance()->setPaused(false);
}

void klogic4::openFile() {
    Simulator::getInstance()->setPaused(true);
	QFileInfo fi(KlogicGlobal::CurrentCircuit::filename);
    QString new_filename = KFileDialog::getOpenFileName(KUrl(fi.dir().path()), "*.circuit");
    if (!new_filename.isEmpty()) {
    	newCircuit();

    	int i = clientWidget->getCircuitWidget()->openCircuit(new_filename);
        if (i > 0) {
			KlogicGlobal::CurrentCircuit::filename = new_filename;
			if (i == 1) {	// not a subcircuit
			fi.setFile(KlogicGlobal::CurrentCircuit::filename);
				setPlainCaption(KlogicGlobal::Klogic::Name + " - " + fi.fileName());
			}
        } else {
			KlogicGlobal::CurrentCircuit::filename = new_filename;
			fi.setFile(KlogicGlobal::CurrentCircuit::filename);
			setPlainCaption(KlogicGlobal::Klogic::Name + " - " + fi.fileName());
        }
    }
    Simulator::getInstance()->setPaused(false);
}

void klogic4::saveFile() {
    // save-command: always save toplevel circuit!
    QFileInfo fi(KlogicGlobal::CurrentCircuit::filename);
    if (fi.exists()) {
    	clientWidget->getCircuitWidget()->saveCircuit(KlogicGlobal::CurrentCircuit::filename, false);
        setPlainCaption(KlogicGlobal::Klogic::Name + " - " + fi.fileName());
    } else {
        saveFileAs();
    }
}

void klogic4::saveFileAs() {
    // save-as-command: ask for saving sub circuit
    bool save_as_sub = clientWidget->getCircuitWidget()->saveAsSubCircuit();
    QFileInfo fi(KlogicGlobal::CurrentCircuit::filename);
    QString new_filename = KFileDialog::getSaveFileName(KUrl(fi.dir().path()), "*.circuit");
    if (!new_filename.isEmpty()) {
    	clientWidget->getCircuitWidget()->saveCircuit(new_filename, save_as_sub);
        KlogicGlobal::CurrentCircuit::filename = new_filename;
        fi.setFile(KlogicGlobal::CurrentCircuit::filename);
        setPlainCaption(KlogicGlobal::Klogic::Name + " - " + fi.fileName());
    }
}

void klogic4::selectionMode() {
    kde4ActionFanCatcher(klogic4::modeSELECT);
}

void klogic4::connectionMode() {
    kde4ActionFanCatcher(klogic4::modeCONNECT);
}

void klogic4::fEQUATION() {
    kde4ActionFanCatcher(DeviceType::fEQU);
}
void klogic4::fAND() {
    kde4ActionFanCatcher(DeviceType::fAND);
}
void klogic4::fNAND() {
    kde4ActionFanCatcher(DeviceType::fNAND);
}
void klogic4::fOR() {
    kde4ActionFanCatcher(DeviceType::fOR);
}
void klogic4::fNOR() {
    kde4ActionFanCatcher(DeviceType::fNOR);
}
void klogic4::fXOR() {
    kde4ActionFanCatcher(DeviceType::fXOR);
}
void klogic4::fNOT() {
    kde4ActionFanCatcher(DeviceType::fINV_INTERNAL);
}
void klogic4::fRS() {
    kde4ActionFanCatcher(DeviceType::fRS);
}
void klogic4::fJK() {
    kde4ActionFanCatcher(DeviceType::fJK);
}
void klogic4::fDFF() {
    kde4ActionFanCatcher(DeviceType::fDFF);
}
void klogic4::fTFF() {
    kde4ActionFanCatcher(DeviceType::fTFF);
}
void klogic4::fRAM() {
    kde4ActionFanCatcher(DeviceType::fRAM);
}
void klogic4::fTRI() {
    kde4ActionFanCatcher(DeviceType::fTRI);
}
void klogic4::fBUS() {
    kde4ActionFanCatcher(DeviceType::fBUS);
}
void klogic4::fOSC() {
    kde4ActionFanCatcher(DeviceType::fOSC);
}
void klogic4::fSWI() {
    kde4ActionFanCatcher(DeviceType::fSWI);
}
void klogic4::fSS() {
    kde4ActionFanCatcher(DeviceType::fSS);
}
void klogic4::fLED() {
    kde4ActionFanCatcher(DeviceType::fLED);
}
void klogic4::fPWR() {
    kde4ActionFanCatcher(DeviceType::fPWR);
}
void klogic4::fTXT() {
    kde4ActionFanCatcher(DeviceType::fTXT);
}
void klogic4::fSUB() {
    kde4ActionFanCatcher(DeviceType::fSUB);
}
void klogic4::fFromLib() {
	libraryDeviceSelected();
}
// Called by ClientWidget::fIN
void klogic4::setFunctionfIN() {
    kde4ActionFanCatcher(DeviceType::fIN);
    fINAction->setChecked(true);
}
// Called by ClientWidget::fOUT
void klogic4::setFunctionfOUT() {
    kde4ActionFanCatcher(DeviceType::fOUT);
    fOUTAction->setChecked(true);
}

void klogic4::kde4ActionFanCatcher(const int newFunction) {
    function = newFunction;
    qDebug() << "function selected:" << function;

    for (list<ClientWidget *>::iterator iter = clientWidgets.begin(); iter != clientWidgets.end(); iter++) {
    	ClientWidget *cw = *iter;
    	cw->applyFunctionToGUI(newFunction);
    }
}

void klogic4::circuitDeleted(Circuit *circuit) {
	list<ClientWidget *>::iterator iter = clientWidgets.begin();
    while (iter != clientWidgets.end()) {
    	ClientWidget *cw = *iter;
    	bool iterMod = false;
    	if (cw == clientWidget) {
    		while (cw->getCircuitWidget()->contained(circuit)) {
    			cw->getCircuitWidget()->toParentCircuit();
    		}
    	} else {
    		qDebug() << "Analyzing " << cw;
    		if (cw->getCircuitWidget()->contained(circuit)) {
    			cw->setVisible(false);
    			cw->close();
    			clientWidgets.remove(cw);
    			iterMod = true;
    		}
    	}
    	if (iterMod) {
    		iter = clientWidgets.begin();
    	} else {
    		iter++;
    	}
    }
}

void klogic4::repaintClientWidgets(Circuit *circuit) {
	list<ClientWidget *>::iterator iter = clientWidgets.begin();
    while (iter != clientWidgets.end()) {
    	ClientWidget *clientWidget = *iter;
    	if (clientWidget->getCircuitWidget()->getActiveCircuit() == circuit) {
    		clientWidget->getCircuitWidget()->repaint();
    	}
    	iter++;
    }
}

void klogic4::repaintClientWidgets() {
	list<ClientWidget *>::iterator iter = clientWidgets.begin();
    while (iter != clientWidgets.end()) {
    	ClientWidget *clientWidget = *iter;
   		clientWidget->getCircuitWidget()->repaint();
    	iter++;
    }
}

void klogic4::setSimPermanent() {
	if (simulationMode != simModePERMANENT) {
		simAutoAction->setChecked(true);
		simTimeSliderLabel->setEnabled(true);
		simTimeSlider->setEnabled(true);
		simulationMode = simModePERMANENT;
		qDebug() << "simulation mode selected:" << simulationMode;
		Simulator::getInstance()->simulationParametersChanged();
	}
}

void klogic4::setSimStepByStep() {
	simStepByStepAction->setChecked(false);
	if (simulationMode != simModeSINGLE_STEP) {
		simTimeSliderLabel->setEnabled(false);
		simTimeSlider->setEnabled(false);
		simAutoAction->setChecked(false);
		simulationMode = simModeSINGLE_STEP;
		qDebug() << "simulation mode selected:" << simulationMode;
		Simulator::getInstance()->simulationParametersChanged();
	} else {
		Simulator::getInstance()->simStep();
	}
}

void klogic4::setSimStepTime(int newTime) {
	setSimPermanent();
    simulationStepTime = newTime;
    qDebug() << "simulation time changed:" << simulationStepTime;
    Simulator::getInstance()->simulationParametersChanged();

    QString tt2;
    tt2.sprintf("Simulation Time (%.03d ms):", newTime);
	simTimeSliderLabel->setText(tt2);
}

void klogic4::setSimBurstCount(int new_burstCount) {
    simulationBurstCount = new_burstCount;
    qDebug() << "simulation burst count changed:" << simulationBurstCount;
    Simulator::getInstance()->simulationParametersChanged();
}

int klogic4::getSimulationMode() {
    return simulationMode;
}

int klogic4::getSimStepTime() {
    return simulationStepTime;
}

int klogic4::getSimBurstCount() {
    return simulationBurstCount;
}

/**
 * [static]
 * returns the function. consider first, in which mode we are. Function is only vaid if in drop mode!
 * @return The function (or a mode if not in drop mode!)
 */
int klogic4::getFunction() {
    return function;
}

/**
 * [static]
 * Returns the mode. There are three modes currently.
 * When in DROP mode, the function specifies in more detail, which drop mode is active (kind of device).
 * The two other modes are SELECT and CONNECT. Then, the function call becomes invalid somehow.
 * @return the current mode
 */
int klogic4::getMode() {
    if (function != klogic4::modeCONNECT && function != klogic4::modeSELECT) {
        return klogic4::modeDROP;
    }
    return function;
}

void klogic4::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_S) {
		setSimStepByStep();
	} else if (event->key() == Qt::Key_A) {
		setSimPermanent();
	}
}

void klogic4::closeEvent(QCloseEvent *) {
	Simulator::getInstance()->quit();
}

void klogic4::quit() {
	close();
}

void klogic4::circuitHasShortcut() {
	if (runtimeShortcutWarning) {
			Simulator::getInstance()->setPaused(true);
			QMessageBox::warning(parentWidget(),
								 i18n("runtime error"),
								 i18n("shortcut\n"));
			Simulator::getInstance()->setPaused(false);
			runtimeShortcutWarning = false;
			shortcutWarningTimer.setSingleShot(true);
			shortcutWarningTimer.setInterval(10000);
			shortcutWarningTimer.start();		// single shot after 20 sec!
	}
}

void klogic4::enableShortcutWarning() {
    runtimeShortcutWarning = true;
}


void klogic4::openLibraryMaintenance(XDevice *new_dev) {
    DlgLib *dlg = new DlgLib(this, i18n("library maintenance"), &libList, activeLib, new_dev);

    dlg->move(pos().x() + 50, pos().y() + 50);
    dlg->exec();

    // library list changed?
    libraryChooser->clear();
    KlogicList<DevLibrary> *ll = libList.First();
    while(ll) {
    	libraryChooser->addItem(ll->getText());
		ll = ll->Next();
    }

    // list empty?
    if (!libraryChooser->count()) {
		libraryChanged(-1);
	    delete dlg;
		return;
    }
    libraryChooser->setEnabled(true);

    // active library changed by dialog?
    activeLib = dlg->getActive();
    delete dlg;

    // actualize lib device list
    if (activeLib) {
		// set active lib in combobox
		for(int i = 0; i < libraryChooser->count(); i++) {
			if (libraryChooser->itemText(i).compare(activeLib->getFileName()) == 0) {
				libraryChooser->setCurrentIndex(i);
				libraryChanged(i);
				return;
			}
		}
    } else {
    	libraryChooser->setCurrentIndex(0);
		libraryChanged(0);
    }
}

// slot coming from toolbar
void klogic4::libraryChanged(int libidx) {
	if (libidx < 0) {
		libraryChooser->setEnabled(false);
		libraryDeviceChooser->clear();
		libraryDeviceChooser->setEnabled(false);
		activeLib = (DevLibrary *)NULL;
	    devANDAction->setChecked(true);
	    devFromLibAction->setVisible(false);
	    fAND();
		return;
	}
	QString libname = libraryChooser->itemText(libidx);
	KlogicList<DevLibrary> *ll = libList.With(libname);
	if (ll) {
		activeLib = ll->Get();
	} else {
		// some error occured: disable everything(?)
		libraryChooser->clear();
		libraryChooser->setEnabled(false);
		libraryDeviceChooser->clear();
		libraryDeviceChooser->setEnabled(false);
		activeLib = (DevLibrary *)NULL;
	    devANDAction->setChecked(true);
	    devFromLibAction->setVisible(false);
	    fAND();
		return;
	}
	QStringList names;
	activeLib->deviceList(names);
	libraryDeviceChooser->clear();
	libraryDeviceChooser->addItems(names);
	if (names.count()) {
		libraryDeviceChooser->setEnabled(true);
		libraryDeviceChooser->setCurrentIndex(0);
		libraryDeviceSelected();
	} else {
		libraryDeviceChooser->setEnabled(false);
	    devFromLibAction->setVisible(false);
	    devANDAction->setChecked(true);
	    fAND();
	}
}

// slot coming from toolbar
// change the currently active library device
void klogic4::libraryDeviceSelected() {
	if (!activeLib) return;
	QString devname = libraryDeviceChooser->currentText();
	int device_id = activeLib->getDevID(devname);
    devFromLibAction->setVisible(true);
    devFromLibAction->setChecked(true);
	kde4ActionFanCatcher(device_id);
}

// slot coming from CircuitWidget
void klogic4::addDeviceToLibrary(XDevice *device) {
	openLibraryMaintenance(device);
}

DevLibrary *klogic4::getActiveLibrary() {
	return activeLib;
}

#include "klogic4.moc"
