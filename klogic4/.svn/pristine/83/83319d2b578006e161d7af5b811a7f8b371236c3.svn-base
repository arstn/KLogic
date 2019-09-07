/***************************************************************************
                          propSwitch.cpp  -  description
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

#include <qlabel.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <klineedit.h>

#include <KlogicGlobal.h>
#include <xdevice.h>
#include <circuit.h>

#include <propSwitch.h>
#include <QGroupBox>
#include "propSwitch.moc"

PropSwitch::PropSwitch(QWidget *parent, const QString &name, Circuit *_dnet, XDevice *_dev)
        : QDialog(parent, Qt::Dialog)
{
    setModal(true);
    setWindowTitle(name);
    dnet = _dnet;
    dev = _dev;

    label0 = new QLabel(this);
    label0->setGeometry(30, 10, 43, 20);
    label0->setText(i18n("Name"));
    devname = new KLineEdit(this);
    devname->setGeometry(80, 10, 110, 20);
    devname->setEnabled(FALSE);
    devname->setText(dev->getName());

    bgType = new QGroupBox(i18n("select type"), this);
    bgType->setGeometry(20, 40, 120, 60);
    rbSwitch = new QRadioButton(i18n("switch"), bgType);
    rbSwitch->setGeometry(10, 20, 100, 15);
    rbPushButton = new QRadioButton(i18n("push button"), bgType);
    rbPushButton->setGeometry(10, 40, 100, 15);

    bgStartVal = new QGroupBox(i18n("start with value"), this);
    bgStartVal->setGeometry(150, 40, 120, 60);
    rbHigh = new QRadioButton(i18n("High"), bgStartVal);
    rbHigh->setGeometry(10, 20, 100, 15);
    rbLow = new QRadioButton(i18n("Low"), bgStartVal);
    rbLow->setGeometry(10, 40, 100, 15);
    if (dev->output()) rbHigh->setChecked(true);
    else rbLow->setChecked(true);

    if (dev->isToggle()) {
        rbPushButton->setChecked(TRUE);
    } else {
        rbSwitch->setChecked(TRUE);
        bgStartVal->setEnabled(false);
    }

    bOK = new QPushButton(this);
    bOK->setGeometry(50, 120, 60, 20);
    bOK->setDefault(TRUE);
    bOK->setText(i18n("OK"));

    bCancel = new QPushButton(this);
    bCancel->setGeometry(170, 120, 60, 20);
    bCancel->setText(i18n("Cancel"));

    setFixedSize(280, 150);

    connect(bOK, SIGNAL(clicked()), SLOT(accept()));
    connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(rbPushButton, SIGNAL(toggled(bool)), SLOT(typeChanged(bool)));
}

PropSwitch::~PropSwitch()
{
}

void PropSwitch::typeChanged(bool is_push_button)
{
    if (is_push_button) {
        bgStartVal->setEnabled(true);
    } else {
        bgStartVal->setEnabled(false);
    }
    if (dev->output()) rbHigh->setChecked(true);
    else rbLow->setChecked(true);
}

void PropSwitch::done(int r)
{
    QString value;

    if (r == Accepted) {
        // check/set new name
        QString s = dev->getName();
        if (0 != devname->text().compare(s)) {
            dnet->setDeviceName(dev->getID(), devname->text());
            dev->setImage();
            dnet->setImage();
        }

        if (rbSwitch->isChecked()) {
            dev->setToggle(false);
        } else {
            dev->setToggle(true);
            if (rbHigh->isChecked()) dev->setInputValue(1);
            else dev->setInputValue(0);
        }
    }

    QDialog::done(r);
}

