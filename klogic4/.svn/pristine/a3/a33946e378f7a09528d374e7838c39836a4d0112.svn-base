/*
 * ScrollableCircuitWidget.cpp
 *
 *  Created on: Nov 21, 2011
 *      Author: andi
 */

#include <qdebug.h>

#include "ScrollableCircuitWidget.h"

ScrollableCircuitWidget::ScrollableCircuitWidget(QWidget *parent, Circuit *circuit)
	:QScrollArea(parent),
	circuitWidget(new CircuitWidget(parent, circuit))
{
	initialize();
}

ScrollableCircuitWidget::~ScrollableCircuitWidget() {
    disconnect(circuitWidget, SIGNAL(autoMove(const QPoint &)), this, SLOT(autoMove(const QPoint &)));
}

CircuitWidget *ScrollableCircuitWidget::getCircuitWidget() {
	return circuitWidget;
}

void ScrollableCircuitWidget::initialize() {
    setBackgroundRole(QPalette::Dark);
    setWidget(circuitWidget);
    connect(circuitWidget, SIGNAL(autoMove(const QPoint &)), this, SLOT(autoMove(const QPoint &)));
}

void ScrollableCircuitWidget::autoMove(const QPoint &mousePos) {
	ensureVisible(mousePos.x(), mousePos.y());
}
