/*
 * ScrollableCircuitWidget.h
 *
 *  Created on: Nov 21, 2011
 *      Author: andi
 */

#ifndef SCROLLABLECIRCUITWIDGET_H_
#define SCROLLABLECIRCUITWIDGET_H_

#include <qscrollarea.h>
#include "CircuitWidget.h"

class ScrollableCircuitWidget : public QScrollArea {
    Q_OBJECT
public:
	ScrollableCircuitWidget(QWidget *parent, Circuit *);
	virtual ~ScrollableCircuitWidget();

	CircuitWidget *getCircuitWidget();

public slots:
	void autoMove(const QPoint &);

private:
	void initialize();

	CircuitWidget *circuitWidget;

};

#endif /* SCROLLABLECIRCUITWIDGET_H_ */
