/*************************************************/
/* methods for class ClientW                     */
/*                                               */
/* window containing a scrollable logic circuit  */
/*                                               */
/* Andreas Rostin                                */
/* 12.03.99                                      */
/*************************************************/

#include <kicon.h>

#include <qtoolbar.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qaction.h>
#include <qscrollarea.h>

#include "OscWidget.h"
#include "KlogicGlobal.h"
#include "OscContentWidget.h"
#include "circuit.h"

#include "OscWidget.moc"

OscWidget::OscWidget()
	: QWidget(0, Qt::Window)
{
	setBackgroundRole(QPalette::Base);
    QLayout *widgetLayout = new QGridLayout();

    oscContentWidget = new OscContentWidget(this);
    oscContentWidget->setGeometry(0, 0, OscContentWidget::WIDTH, 600);
    oscContentWidget->setMaximumWidth(OscContentWidget::WIDTH);
    oscContentWidget->setMinimumWidth(OscContentWidget::WIDTH);

    QScrollArea *qScollArea = new QScrollArea(this);
	qScollArea->setBackgroundRole(QPalette::Base);
	qScollArea->setWidget(oscContentWidget);

	widgetLayout->addWidget(qScollArea);
	setLayout(widgetLayout);

	setMaximumWidth(OscContentWidget::WIDTH + 28);
	setGeometry(100, 100, 800, OscContentWidget::WIDTH + 28);
	setWindowTitle("KLogic - Ocilloscope");
	setVisible(true);
}

OscWidget::~OscWidget() {
}

void OscWidget::closeEvent(QCloseEvent *) {
	emit oscillatorWidgetClosed();
}

void OscWidget::displayCircuit(Circuit *circuit) {
	oscContentWidget->displayCircuit(circuit);
}

void OscWidget::circuitChanged(Circuit *circuit) {
	oscContentWidget->circuitChanged(circuit);
}

void OscWidget::simStep() {
	oscContentWidget->simStep();
}
