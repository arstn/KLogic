/*******************************************************/
/* methods for a net-widget                            */
/*                                                     */
/* this contains the current active circuit widget,    */
/* manages various modes, status, drawing and io       */
/*                                                     */
/* Andreas Rostin                                      */
/* 27.10.97                                            */
/*******************************************************/
#include <qpainter.h>
#include <qstring.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <QMenu>
#include <qtimer.h>
#include <qwidget.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdialog.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <QMouseEvent>
#include <qnamespace.h>

#include "klogic4.h"
#include "CircuitWidget.h"
#include "KlogicGlobal.h"
#include "DeviceType.h"

#include "FileIO.h"
#include "circuit.h"
#include "xdevice.h"
#include "selection.h"
#include "propOsz.h"
#include "propDev.h"
#include "propName.h"
#include "propPwr.h"
#include "propText.h"
#include "propSwitch.h"
#include "dlgEqu.h"
#include "xmlImport.h"
#include "grid.h"
#include "Simulator.h"

#include "CircuitWidget.moc"

CircuitWidget::CircuitWidget(QWidget *parent, Circuit *circuit) : QFrame(parent) {
    initialize(circuit);
}

void CircuitWidget::initialize(Circuit *circuit) {

    setBackgroundRole(QPalette::Base);
    //setAutoFillBackground(true);
    setFrameStyle( QFrame::Panel | QFrame::Raised );
    resize(KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X, KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y);                // set default size in pixels

    activeCircuit = circuit;
    activeDevice = (XDevice *)NULL;
    activeWire = (XWire *)NULL;
    m_poActiveSelection = new Selection();

    // popup for the right mouse key (on devices)
    rpop = new QMenu();

    // popup for the right mouse key (on selections)
    rselpop = new QMenu();
    QAction *action;
    action = rselpop->addAction(i18n("Copy"));
    connect(action, SIGNAL(triggered(bool)), SLOT (copy()));
    action = rselpop->addAction(i18n("Paste"));
    connect(action, SIGNAL(triggered(bool)), SLOT (pasteAt()));
    action = rselpop->addAction(i18n("Cut"));
    connect(action, SIGNAL(triggered(bool)), SLOT (cut()));

    setContentChanged(false);
    m_bSelMoveCursor = false;
    m_bRMenuActive = false;
}

CircuitWidget::~CircuitWidget()
{
    delete activeCircuit;
    delete m_poActiveSelection;
}

void CircuitWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    
    int x, y;
    int x1s, y1s, x2s, y2s;
    QPainter p;

    x1s = event->rect().x() / Grid::GRID * Grid::GRID - Grid::GRID;
    x2s = event->rect().width() + event->rect().x();
    y1s = event->rect().y() / Grid::GRID * Grid::GRID - Grid::GRID;
    y2s = event->rect().height() +  event->rect().y();

    p.begin(this);
    // draw the grid
    p.setPen(Qt::black);
    for (x = x1s ; x < x2s ;x += Grid::GRID) {
        for (y = y1s; y < y2s; y += Grid::GRID) {
            p.drawPoint(x, y);
		}
    }
    // draw the circuit
    activeCircuit->draw(this, &p);
    m_poActiveSelection->draw(&p);
    p.end();

	// runtime shortcut occured?
	if (activeCircuit->hasShortcut()) {
		klogic4::getInstance()->circuitHasShortcut();
	}
}

void CircuitWidget::mousePressEvent( QMouseEvent *e )
{
    if (e->button() == Qt::LeftButton) {
        if (m_bRMenuActive) {
            m_bRMenuActive = false;
            return;
        }

        if (klogic4::getInstance()->getMode() == klogic4::modeCONNECT || klogic4::getInstance()->getMode() == klogic4::modeDROP) {
            // device clicked? always set active device and return
            activeDevice = activeCircuit->checkDeviceCollision(e->pos());
            if (activeDevice) {
                activeDevice->setOrigin();

                // if device is a push button device,  toggle its status
                if (activeDevice->isInteractive()) {
                    activeDevice->toggleStaticInput();
                }
                setContentChanged(true);
                return;
            }
        }

        if (klogic4::getInstance()->getMode() == klogic4::modeCONNECT) {
            // wire clicked? set active wire
            activeWire = activeCircuit->checkWireCollision(e->pos());
            if (activeWire) {
            	activeWire->createNode();
                setContentChanged(true);
                return;
            }

            // nothing clicked? new wire
            activeWire = activeCircuit->newWire(e->pos());
            if (activeWire) {
            	activeWire->createNode();
            	setContentChanged(true);
                return;
            }
        } else if (klogic4::getInstance()->getMode() == klogic4::modeDROP) {
        	if (!activeCircuit->parent() &&
        			(klogic4::getInstance()->getFunction() == DeviceType::fIN ||
        			 klogic4::getInstance()->getFunction() == DeviceType::fOUT)) {
        		// no inputs/outputs in root parent
        		return;
        	}
            // create new device
        	Simulator::getInstance()->setPaused(true);
            activeDevice = activeCircuit->createDevice(klogic4::getInstance()->getFunction(), e->pos());
            if (!activeDevice && (klogic4::getInstance()->getFunction() == DeviceType::fIN)) {
                QMessageBox::warning(parentWidget(), i18n("device failure"), i18n("unable to add more named inputs\nresize this sub circuit"));
                return;
            } else if (!activeDevice && (klogic4::getInstance()->getFunction() == DeviceType::fOUT)) {
                QMessageBox::warning(parentWidget(), i18n("device failure"), i18n("unable to add more named outputs\nresize this sub circuit"));
                return;
            } else if (!activeDevice) {
                qFatal("panic: cannot create device!");
                return;
            }
            if (activeCircuit->checkDeviceCollision(activeDevice)) {
            	// do not ask, just delete it
            	delete activeDevice;
                return;
            }
        	Simulator::getInstance()->setPaused(false);
            // Creation OK
            activeDevice->setOrigin();
            setContentChanged(true);
            observedChanged();
            return;
        } else if (klogic4::getInstance()->getMode() == klogic4::modeSELECT) {
            if (m_poActiveSelection->getStatus() == SEL_EXISTS && m_poActiveSelection->onFrame(e->pos())) {
                // selection move mode
                m_poActiveSelection->beginMove(e->pos());
            } else if (m_poActiveSelection->getStatus() == SEL_EXISTS &&  NULL != (activeDevice = activeCircuit->checkDeviceCollision(e->pos()))) {
                if (m_poActiveSelection->contains(activeDevice)) {
                    // selection move mode
                    m_poActiveSelection->beginMove(e->pos());
                } else {
                    // select device, then selection move mode
                    m_poActiveSelection->addTemp(activeDevice);
                    m_poActiveSelection->beginMove(e->pos());
                }
            } else {
                // new selection?
                m_poActiveSelection->setCircuit(activeCircuit);

                // select/deselect single device
                activeDevice = activeCircuit->checkDeviceCollision(e->pos());
                if (activeDevice) {
                    if (m_poActiveSelection->contains(activeDevice)) {
                    	m_poActiveSelection->remove(activeDevice);
                    } else {
                    	m_poActiveSelection->add(activeDevice);
                    }
                    activeDevice = (XDevice *)NULL;
                    activeWire = (XWire *)NULL;
                    setContentChanged(true);
                    return;
                }

                // select/deselect single wire
                activeWire = activeCircuit->checkWireCollision(e->pos());
                if (activeWire) {
                    if (m_poActiveSelection->contains(activeWire)) {
                    	m_poActiveSelection->remove(activeWire);
                    } else {
                    	m_poActiveSelection->add(activeWire);
                    }
                    activeDevice = (XDevice *)NULL;
                    activeWire = (XWire *)NULL;
                    setContentChanged(true);
                    return;
                }

                // group selections
                m_poActiveSelection->beginFrame(e->pos());
            }
            setContentChanged(true);
            return;
        }
    }

    if (e->button() == Qt::RightButton) {
        // right clicked in selection mode? open popup to choose operation
        if (klogic4::getInstance()->getMode() == klogic4::modeSELECT) {
            m_poActiveSelection->at(e->pos());
            selRightSelMenu(e);
            m_bRMenuActive = true;
            return;
        }

        // device right clicked? open popup to chooce operation
        if (NULL != (activeDevice = activeCircuit->checkDeviceCollision(e->pos()))) {
            selRightMenu(e);
            m_bRMenuActive = true;
            return;
        }

        // right clicked twice?
        if (m_bRMenuActive) {
            m_bRMenuActive = false;
            return;
        }

        // wire right clicked? remove node
        if (activeCircuit->removeNode(e->pos())) {
            activeWire = (XWire *)NULL;
            setContentChanged(true);
        }
    }
}

void CircuitWidget::mouseReleaseEvent( QMouseEvent *e)
{
    if (e->button() == Qt::RightButton) {
    	return;
    }

    if (klogic4::getInstance()->getMode() == klogic4::modeSELECT) {
        if (m_poActiveSelection->getStatus() == SEL_START) {	// making frames
            m_poActiveSelection->endFrame();
            return;
        }
        if (m_poActiveSelection->getStatus() == SEL_MOVE) {	// moving frames or select devices
            m_poActiveSelection->endMove();

            if (!m_poActiveSelection->hasMoved()) {
                // select/deselect single device when group was not moved
                activeDevice = activeCircuit->checkDeviceCollision(e->pos());
                if (activeDevice) {
                    if (m_poActiveSelection->isTemp(activeDevice)) {
                    	m_poActiveSelection->fixTemp(activeDevice);
                    } else {
                    	m_poActiveSelection->remove(activeDevice);
                    }
                    activeDevice = (XDevice *)NULL;
                    activeWire = (XWire *)NULL;
                    activeDevice = (XDevice *)NULL;
                    setContentChanged(true);
                    return;
                }
            }
            activeDevice = activeCircuit->checkDeviceCollision(e->pos());
            if (activeDevice) {
                // look if some wires want to drop nodes..
                activeDevice->garbageCollection();
                activeDevice = (XDevice *)NULL;
                setContentChanged(true);
                return;
            }
        }
        return;
    }

    // check for collision with another device
    if (activeDevice) {
        //look for devices lying in the device
        if (activeCircuit->checkDeviceCollision(activeDevice)) {
            activeDevice->toOrigin();
            setContentChanged(true);
        }

        // if device was not moved and it is interactive, toggle its status
        if (activeDevice->isInteractive() && activeDevice->isOrigin(e->pos())) {
        	if (activeDevice->isToggle()) {
        		activeDevice->toggleStaticInput();
                setContentChanged(true);
        	}
        }

        // look if some wires want to drop nodes..
        activeDevice->garbageCollection();
        activeDevice = (XDevice *)NULL;
    }
    
    if (activeWire) {
        // check for connection to device or another wire
        activeCircuit->checkConnection(activeWire);
        activeCircuit->releaseWire(activeWire);
        activeWire = (XWire *)NULL;
        setContentChanged(true);
        observedChanged();
    }
}

void CircuitWidget::activateSelectionMode(bool activated) {
    setMouseTracking(activated);
	if (!activated) {
		removeSelection();
        activeDevice = (XDevice *)NULL;
        activeWire = (XWire *)NULL;
        m_bSelMoveCursor = false;
        setCursor(Qt::ArrowCursor);
	}
}

void CircuitWidget::mouseMoveEvent( QMouseEvent *e )
{
    if (klogic4::getInstance()->getMode() == klogic4::modeSELECT) {
        if (m_poActiveSelection->getStatus() == SEL_EXISTS && m_poActiveSelection->onFrame(e->pos())) {
            if (!m_bSelMoveCursor) {
                m_bSelMoveCursor = true;
                setCursor(Qt::SizeAllCursor);
            }
        } else {
            if (m_bSelMoveCursor) {					// switch selecion cursor
                m_bSelMoveCursor = false;
                setCursor(Qt::ArrowCursor);
            } else if (m_poActiveSelection->getStatus() == SEL_START) {	// make selections
                emit(autoMove(e->pos()));	// move circuit when nessecary
                m_poActiveSelection->changeFrame(e->pos());
				setContentChanged(true);
            } else if (m_poActiveSelection->getStatus() == SEL_MOVE) {		// move selections
                emit(autoMove(e->pos()));	// move circuit when nessecary
                m_poActiveSelection->moveFrame(e->pos());
                setContentChanged(true);
            }
        }
    }

    if (m_bRMenuActive) {
    	return;
    }

    // move wire/device
    if (activeDevice) {
        emit(autoMove(e->pos()));	// move circuit when nessecary

        if (activeDevice->moveTo(e->pos())) {
            setContentChanged(true);
        }
    }
    if (activeWire) {
        emit(autoMove(e->pos()));	// move if nessecary
        activeWire->updateNode(e->pos());
        setContentChanged(true);
    }
}

void CircuitWidget::mouseDoubleClickEvent( QMouseEvent *e )
{
    // device clicked?
    activeDevice = activeCircuit->checkDeviceCollision(e->pos());
    if (activeDevice) {
        if (activeDevice->type() == DeviceType::fSWI) {
            activeDevice = (XDevice *)NULL;
            return;
        }

        // if sub circuit is double clicked, open it
        if (toSubCircuit()) {
            return;
        }

        // all other devices: open properties dialogue
        openActiveDeviceProperties();
    }
}

// right mouse button popup entries
// menu depends on device type
void CircuitWidget::selRightMenu(QMouseEvent *e)
{
    if (!activeDevice) return;

    rpop->clear();
    QAction *action;
    action = rpop->addAction(i18n("properties"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(openActiveDeviceProperties()));

    if (activeDevice->type() == DeviceType::fSUB) {
        action = rpop->addAction(i18n("open"));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(toSubCircuit()));
        action = rpop->addAction(i18n("open in window"));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(toSubCircuitW()));
        action = rpop->addAction(i18n("display in oscilloscope"));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(showCircuitInOscilloscope()));
    }

    if (activeDevice->type() == DeviceType::fEQU) {
        action = rpop->addAction(i18n("equations"));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(showDeviceEquations()));
    }

    if (activeDevice->isObservable()) {
        if (activeDevice->isObserved()) {
            action = rpop->addAction(i18n("hide in oscilloscope"));
        } else {
            action = rpop->addAction(i18n("show in oscilloscope"));
        }
        connect(action, SIGNAL(triggered(bool)), this, SLOT(addActiveDeviceToOscilloscope()));
    }

    if (activeDevice->type() == DeviceType::fEQU || activeDevice->type() == DeviceType::fSUB) {
        action = rpop->addAction(i18n("add to lib"));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(addActiveDeviceToLib()));
    }

    rpop->addSeparator();
    action = rpop->addAction(i18n("remove"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(deleteActiveDevice(bool)));

    rpop->popup(mapToGlobal(e->pos()));
}

void CircuitWidget::addActiveDeviceToLib() {
   	emit addDeviceToLibrary(activeDevice);
    m_bRMenuActive = false;
}

void CircuitWidget::addActiveDeviceToOscilloscope() {
	// toggle graph enabled flag in device
	activeDevice->setObserved(!activeDevice->isObserved());
	emit observedChanged(activeCircuit);
    m_bRMenuActive = false;
}

void CircuitWidget::showCircuitInOscilloscope() {
	if (activeDevice->devIsCircuit()) {
		emit ovserveCircuit(activeDevice->devIsCircuit());
	}
    m_bRMenuActive = false;
}

// right mouse button popup entries on selections
void CircuitWidget::selRightSelMenu(QMouseEvent *e)
{	//(entries are static)
    rselpop->popup(mapToGlobal(e->pos()));
    m_bRMenuActive = false;
}

void CircuitWidget::cut() {
    m_poActiveSelection->cut(activeCircuit);
    setContentChanged(true);
    m_bRMenuActive = false;
}

void CircuitWidget::copy() {
    m_poActiveSelection->copy(activeCircuit);
    m_bRMenuActive = false;
}

void CircuitWidget::paste() {
    m_poActiveSelection->at(QPoint(40, 10));
    pasteAt();
    m_bRMenuActive = false;
}

void CircuitWidget::pasteAt() {
    m_poActiveSelection->paste(activeCircuit);
    setContentChanged(true);
    m_bRMenuActive = false;
}

// remove old selections
void CircuitWidget::removeSelection() {
    m_poActiveSelection->remove(true);
    m_bRMenuActive = false;
}

// remove device
void CircuitWidget::deleteActiveDevice(bool ask) {
    if (activeDevice->type() == DeviceType::fSUB) {
        if (ask) {
            if (1 == QMessageBox::warning(parentWidget(), i18n("remove device"),
                                          i18n("remove entire sub-circuit?"), i18n("OK"), i18n("Cancel")))
            {
                activeDevice = (XDevice *)NULL;
                return;
            }
        }
        qDebug() << this << " emiting circuitDeleted";
        emit circuitDeleted(activeDevice->devIsCircuit());
    }
	Simulator::getInstance()->setPaused(true);
    activeCircuit->deleteDevice(activeDevice);
    activeDevice = (XDevice *)NULL;
	Simulator::getInstance()->setPaused(false);

    setContentChanged(true);
    observedChanged();
    m_bRMenuActive = false;
}

// open device properties?
void CircuitWidget::openActiveDeviceProperties()
{
    // open properties for oszillators
    if (activeDevice->type() == DeviceType::fOSC) {
        PropOsz *dlg = new PropOsz(parentWidget(), i18n("oscillator properties"), activeCircuit, activeDevice);
        showPropertiesDialog(dlg);
    } else if (activeDevice->type() == DeviceType::fTXT) {
        // open properties for text devices
        PropText *dlg = new PropText(parentWidget(), i18n("text label"), activeCircuit, activeDevice);
        showPropertiesDialog(dlg);
    } else if (activeDevice->type() == DeviceType::fPWR) {
        // open properties for power sources
        PropPwr *dlg = new PropPwr(parentWidget(), i18n("power device properties"), activeCircuit, activeDevice);
        showPropertiesDialog(dlg);
    } else if (activeDevice->type() == DeviceType::fSWI) {
        // open properties for switch devices
        PropSwitch *dlg = new PropSwitch(parentWidget(), i18n("switch device properties"), activeCircuit, activeDevice);
        showPropertiesDialog(dlg);
    } else if ((activeDevice->type() == DeviceType::fIN) ||
            (activeDevice->type() == DeviceType::fOUT)) {
        // open properties for inputs and ouputs
        PropName *dlg = new PropName(parentWidget(), i18n("interface device name"), this, activeCircuit, activeDevice);
        showPropertiesDialog(dlg);
    } else {
		// open dialog with common device properties
		PropDev *dlg = new PropDev(parentWidget(), i18n("device properties"),this ,activeCircuit, activeDevice);
		showPropertiesDialog(dlg);
    }
}

void CircuitWidget::showPropertiesDialog(QDialog *dlg) {
	Simulator::getInstance()->setPaused(true);
    dlg->exec();
    delete dlg;
    activeDevice = (XDevice *)NULL;
    setContentChanged(true);
    observedChanged();
	Simulator::getInstance()->setPaused(false);
}

// switch to sub circuit in a new window?
bool CircuitWidget::toSubCircuitW()
{
    Circuit *newActiveCircuit = activeDevice->devIsCircuit();
    if (newActiveCircuit) {
        // remove the current selection before switching to a new circuit
        removeSelection();
        newActiveCircuit->forceOutputChange();
        emit openClientWidget(newActiveCircuit);
        return true;
    }
    return false;
}

// switch to sub circuit?
bool CircuitWidget::toSubCircuit()
{
    Circuit *newActiveCircuit = activeDevice->devIsCircuit();
    if (newActiveCircuit) {
        // remove the current selection before switching to a new circuit
        removeSelection();

        activeCircuit = newActiveCircuit;
        activeDevice = (XDevice *)NULL;
        activeWire = (XWire *)NULL;
        // signal mainw info about parent or not (changes toolbar)
        emit(visibleCircuitChanged(activeCircuit->parent()));
        activeCircuit->forceOutputChange();
        return true;
    }
    return false;
}

// switch to parent of active net
void CircuitWidget::toParentCircuit()
{
    // remove the current selection before switching to a new circuit
    removeSelection();

    Circuit *parentCircuit = activeCircuit->parent();
    if (!parentCircuit) {
        qFatal("no parent net!?");
        return;
    }
    activeCircuit = parentCircuit;
    activeWire = (XWire *)NULL;
    activeDevice = (XDevice *)NULL;
    emit(visibleCircuitChanged(activeCircuit->parent()));
    activeCircuit->forceOutputChange();
}

bool CircuitWidget::saveAsSubCircuit()
{
    Circuit *circuit = activeCircuit->rootParent();

    circuit = activeCircuit->rootParent();
    if (circuit != activeCircuit) {
        int ret = QMessageBox::warning(parentWidget(),
                                       i18n("save circuit"),
                                       i18n("the current circuit is a sub circuit.\nsave it as a.."),
                                       i18n("main circuit"),
                                       i18n("sub circuit"));
        if (ret) return true;
    }
    return false;
}

bool CircuitWidget::saveCircuit(QString filename, bool asSubCircuit)
{
    Circuit *circuit;
    if (asSubCircuit)
        circuit = activeCircuit;
    else
        circuit = activeCircuit->rootParent();

    FileIO fileIO(filename, circuit);
    if (asSubCircuit) fileIO.setSubFilename();

    int ret = fileIO.write(asSubCircuit, false, 0, 0);
    if (ret <= 0) {
        QMessageBox::warning(parentWidget(),
                             i18n("save error"),
                             i18n("unable to write file"));
        return false;
    }

    setContentChanged(false);
    return true;
}

// load file
// init: refresh client widget
// return values:
// 0: error occured
// 1: ok, normal circuit
// 2: ok, subcircuit
int CircuitWidget::openCircuit(QString filename)
{
    bool create_sub = false;

    // -----------------------------------------
    // check file format
    // -----------------------------------------
    FileIO fileIO(filename);
    int file_type = fileIO.checkFileFormat();

    Circuit *circuit;
    if (file_type == FileIO::XML_SUB || file_type == FileIO::KLOGIC_SUB) {
        qDebug() << "SUB import";
        // -----------------------------------------
        // is a sub circuit
        // -----------------------------------------
        create_sub = true;
        circuit = activeCircuit;
    } else if (file_type == FileIO::XML_MAIN || file_type == FileIO::KLOGIC_MAIN) {
        qDebug() << "MAIN import";
        circuit = activeCircuit->rootParent();
        activeCircuit = circuit;
        activeDevice = (XDevice *)NULL;
        activeWire = (XWire *)NULL;

        activeCircuit->deleteAll();
        uniqueID::reset();
        setContentChanged(false);
    } else {
        QMessageBox::warning(parentWidget(),
                             i18n("read error"),
                             i18n("unknown file format"));
        return 0;
    }

	Simulator::getInstance()->setPaused(true);
    fileIO.setCircuit(circuit);

    // -----------------------------------------
    // read the file and create the circuit now
    // -----------------------------------------
    bool result = false;
    switch (file_type) {
    case FileIO::XML_SUB:
    case FileIO::XML_MAIN:
        result = fileIO.readXML();
        break;
    case FileIO::KLOGIC_SUB:
    case FileIO::KLOGIC_MAIN:
        result = fileIO.readCircuit(create_sub);
        break;
    }

	Simulator::getInstance()->setPaused(false);

    // -----------------------------------------
    // analyze the result
    // -----------------------------------------
    if (!result) {
        if (XMLImportHandler::errorsOccured()) {
            QMessageBox::warning(parentWidget(),
                                 i18n("read error"),
                                 XMLImportHandler::getErrors());
        } else {
            QMessageBox::warning(parentWidget(),
                                 i18n("read error"),
                                 i18n("unable to read file"));
        }

        repaint();
        return 0;
    }

    // -----------------------------------------
    // final steps
    // -----------------------------------------
    // signal mainw info about having a parent or not (changes toolbar)
    emit(visibleCircuitChanged(activeCircuit->parent()));
    
    // remove useless nodes/wires
    activeCircuit->garbageCollection();
    
    setContentChanged(false);

    // redraw everything
    repaint();

    if (create_sub) {
    	return 2;
    }
    return 1;
}

void CircuitWidget::newCircuit() {
	while (activeCircuit != activeCircuit->rootParent()) {
		toParentCircuit();
	}
	activeCircuit->deleteAll();
	uniqueID::reset();
	setContentChanged(true);
	setContentChanged(false);
}

// display dialog with the equations of the current circuit
void CircuitWidget::showCircuitEquations()
{
    if (!activeCircuit->checkCircuitNames()) {
        if (QMessageBox::No == (QMessageBox::warning(this,
                                i18n("Parse circuit equations"),
                                i18n("There are some non unique device names \nwithin your circuit ... \n\nrename them automaticly?"),
                                QMessageBox::Yes,
                                QMessageBox::No)))
            return;
        activeCircuit->unifyCircuitNames();
    }
    if (activeCircuit->hasImpossibleEquations()) {
        QMessageBox::information(this,
                                 i18n("Parse circuit equations"),
                                 i18n("Unable to display equations for circuits containing \nRAM or Tristate devices!"));
        return;
    }
    DlgEqu *equation_dialog = new DlgEqu(this, i18n("Circuit Equation Editor"), activeCircuit);
    showPropertiesDialog(equation_dialog);
}

// display dialog with the equations of the active device
void CircuitWidget::showDeviceEquations()
{
    if (!activeDevice) return;
    DlgEqu * equation_dialog = new DlgEqu(this, i18n("Device Equation Editor"), activeDevice);
    showPropertiesDialog(equation_dialog);
}

// retrieve the active Circuit
Circuit *CircuitWidget::getActiveCircuit()
{
    return activeCircuit;
}

// check if given circuit is this or a parent of this
bool CircuitWidget::contained(Circuit *circuitToTest)
{
    Circuit *_circuitToTest = activeCircuit;

    qDebug() << this << " check circuit contained";

    if (_circuitToTest == circuitToTest) return true;
    while (NULL != (_circuitToTest = _circuitToTest->parent())) {
        if (_circuitToTest == circuitToTest) return true;
    }
    return false;
}

// apply device defaults to all existing devices
void CircuitWidget::applyDefaults()
{
    Circuit *toplevel = activeCircuit->rootParent();
    toplevel->applyDefaults();
}

bool CircuitWidget::isContentChanged()
{
    return changed;
}

void CircuitWidget::setContentChanged(bool changed)
{
    if (changed) {
		emit circuitChanged(activeCircuit);
    } else {
        emit circuitUnchanged();
    }
    this->changed = changed;
}

void CircuitWidget::observedChanged()
{
	if (activeCircuit->isObserved() || (activeDevice && activeDevice->isObserved())) {
		emit observedChanged(activeCircuit);
	}
    this->changed = changed;
}
