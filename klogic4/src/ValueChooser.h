/*
 * LedWidget.h
 *
 *  Created on: Nov 27, 2011
 *      Author: andi
 */

#ifndef LEDCOLORCHOOSER_H_
#define LEDCOLORCHOOSER_H_

#include <qgroupbox.h>
#include <qbuttongroup.h>

#include <list>
#include <string>
using namespace std;

class ValueChooser : public QGroupBox {
	Q_OBJECT
public:
	ValueChooser(QWidget *parent, string title, list<string> choices, int currentValue);
	ValueChooser(QWidget *parent, string title, list<string> choices, list<int> values, int currentValue);
	~ValueChooser();

	void setCurrentValue(int value);
	int getChosenValue();
	int getInitialValue();
	void setEnabled(bool);
	void setEnabled(int value, bool enabled);

signals:
	void valueChosen(int);

private slots:
	void newValue(int value);

private:
	static const int LENGTH = 100;
	static const int HEIGHT = 20;

	QButtonGroup *buttonGroup;
	int initialValue;
};

#endif /* LEDWIDGET_H_ */
