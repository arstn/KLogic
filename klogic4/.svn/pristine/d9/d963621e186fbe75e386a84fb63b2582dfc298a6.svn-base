/*
 * LedWidget.cpp
 *
 *  Created on: Nov 27, 2011
 *      Author: andi
 */

#include <qboxlayout.h>
#include <qradiobutton.h>

#include "KlogicGlobal.h"
#include "deviceColored.h"
#include "ValueChooser.h"

ValueChooser::ValueChooser(QWidget *parent, string title, list<string> choices, int currentValue)
	: QGroupBox(i18n(title.c_str()), parent)
{
	buttonGroup = new QButtonGroup();
	QVBoxLayout *layout = new QVBoxLayout(this);

	int h = HEIGHT;
	int pos = 0;
	for (list<string>::iterator it = choices.begin(); it != choices.end(); ++it) {
		QRadioButton *valueButton = new QRadioButton(i18n((*it).c_str()));
		valueButton->setGeometry(30, h, 100, HEIGHT);
		buttonGroup->addButton(valueButton, pos++);
		layout->addWidget(valueButton);
		h += HEIGHT;
	}
	setLayout(layout);

	initialValue = currentValue;
	connect	(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(newValue(int)));
	setCurrentValue(currentValue);
}

ValueChooser::ValueChooser(QWidget *parent, string title, list<string> choices, list<int> values, int currentValue)
	: QGroupBox(i18n(title.c_str()), parent)
{
	buttonGroup = new QButtonGroup();
	QVBoxLayout *layout = new QVBoxLayout(this);

	int h = HEIGHT;
	list<int>::iterator valueIterator = values.begin();
	for (list<string>::iterator it = choices.begin(); it != choices.end(); ++it) {
		QRadioButton *valueButton = new QRadioButton(i18n((*it).c_str()));
		valueButton->setGeometry(30, h, 100, HEIGHT);
		buttonGroup->addButton(valueButton, *valueIterator);
		layout->addWidget(valueButton);
		h += HEIGHT;
		++valueIterator;
	}
	setLayout(layout);

	initialValue = currentValue;
	connect	(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(newValue(int)));
	setCurrentValue(currentValue);
}

ValueChooser::~ValueChooser() {
	delete buttonGroup;
}

void ValueChooser::newValue(int value) {
	emit(valueChosen(value));
}

void ValueChooser::setCurrentValue(int value) {
	QAbstractButton *button = buttonGroup->button(value);
	if (button) {
		button->setChecked(true);
	}
}

int ValueChooser::getChosenValue() {
	return buttonGroup->checkedId();
}

int ValueChooser::getInitialValue() {
	return initialValue;
}

void ValueChooser::setEnabled(bool enabled) {
	QGroupBox::setEnabled(enabled);
}

void ValueChooser::setEnabled(int value, bool enabled) {
	QAbstractButton *button = buttonGroup->button(value);
	if (button) {
		button->setEnabled(enabled);
	}
}
