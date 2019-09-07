/***************************************************************************
                          propSwitch.h  -  description
                             -------------------
    begin                : Sun Aug 19 2001
    copyright            : (C) 2001 by rostin
    email                : andreas@a-rostin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROPSWITCH_H
#define PROPSWITCH_H

#include <qwidget.h>
#include <qdialog.h>

class KLineEdit;

class QLabel;
class QGroupBox;
class QRadioButton;

class XDevice;
class Circuit;

class PropSwitch : public QDialog  {
   Q_OBJECT
public: 
	PropSwitch(QWidget *parent, const QString &name, Circuit *, XDevice *);
	~PropSwitch();

private slots:
	void typeChanged(bool);

private:
	void done(int);

	QPushButton *bOK;
	QPushButton *bCancel;
	QLabel *label0;
	KLineEdit *devname;

	QGroupBox *bgType;
	QRadioButton *rbSwitch;
	QRadioButton *rbPushButton;

	QGroupBox *bgStartVal;
	QRadioButton *rbHigh;
	QRadioButton *rbLow;

	Circuit *dnet;
	XDevice *dev;

};

#endif
