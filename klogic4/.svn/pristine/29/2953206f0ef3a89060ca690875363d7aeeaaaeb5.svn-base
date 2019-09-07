/*************************************************/
/* methods for class ClientW                     */
/*                                               */
/* window containing a scrollable logic circuit  */
/*                                               */
/* Andreas Rostin                                */
/* 12.03.99                                      */
/*************************************************/
#include <qtoolbar.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qaction.h>

#include <kicon.h>

#include "klogic4.h"
#include "circuit.h"
#include "ScrollableCircuitWidget.h"
#include "ClientWidget.h"
#include "ClientWidget.moc"

ClientWidget::ClientWidget(Circuit *circuit)
	: QWidget(0, Qt::Window)
{
    scollableCircuitWidget = new ScrollableCircuitWidget(this, circuit);
    initialize();
}

ClientWidget::~ClientWidget() {
}

void ClientWidget::closeEvent(QCloseEvent *) {
	emit clientWidgetClosed(this);
    disconnect(getCircuitWidget(), SIGNAL(visibleCircuitChanged(bool)), this, SLOT(visibleCircuitChanged(bool)));
}

void ClientWidget::initialize() {
    QLayout *widgetLayout = new QGridLayout();

	tb = new QToolBar(this);
    widgetLayout->addWidget(tb);
    widgetLayout->addWidget(scollableCircuitWidget);
	setLayout(widgetLayout);

	QAction *action = new QAction(KIcon("klogic4-SubUp.png"), i18n("Go to Parent Circuit"), tb);
    action->setCheckable(false);
    tb->addAction(action);
    connect(action, SIGNAL(triggered(bool)), getCircuitWidget(), SLOT(toParentCircuit()));

    QActionGroup *functionActionGroup = new QActionGroup(this);
    fINAction = new QAction(KIcon("klogic4-fIN.png"), i18n("&Input"), this);
    fINAction->setCheckable(true);
    functionActionGroup->addAction(fINAction);
    tb->addAction(fINAction);
    connect(fINAction, SIGNAL(triggered(bool)), this, SLOT(fIN()));

    fOUTAction = new QAction(KIcon("klogic4-fOUT.png"), i18n("&Output"), this);
    fOUTAction->setCheckable(true);
    functionActionGroup->addAction(fOUTAction);
    tb->addAction(fOUTAction);
    connect(fOUTAction, SIGNAL(triggered(bool)), this, SLOT(fOUT()));

    tb->setVisible(getCircuitWidget()->getActiveCircuit()->parent());
    connect(getCircuitWidget(), SIGNAL(visibleCircuitChanged(bool)), SLOT(visibleCircuitChanged(bool)));

	setVisible(true);
}

CircuitWidget * ClientWidget::getCircuitWidget() {
	return scollableCircuitWidget->getCircuitWidget();
}

void ClientWidget::visibleCircuitChanged(bool hasParent) {
	tb->setVisible(hasParent);
}

void ClientWidget::fIN() {
    klogic4::getInstance()->setFunctionfIN();
}

void ClientWidget::fOUT() {
	klogic4::getInstance()->setFunctionfOUT();
}

void ClientWidget::applyFunctionToGUI(int function) {
	fOUTAction->setChecked(function == DeviceType::fOUT);
	fINAction->setChecked(function == DeviceType::fIN);
	scollableCircuitWidget->getCircuitWidget()->activateSelectionMode(function == klogic4::modeSELECT);
}
