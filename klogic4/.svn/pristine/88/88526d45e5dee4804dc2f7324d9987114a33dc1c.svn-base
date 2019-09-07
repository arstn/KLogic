/*************************************************/
/* methods for class PropName                    */
/*                                               */
/* name property dialog                          */
/*                                               */
/* Andreas Rostin                                */
/* 15.01.99                                      */
/*************************************************/
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <klineedit.h>

#include "KlogicGlobal.h"
#include "CircuitWidget.h"
#include "circuit.h"
#include "xdevice.h"
#include "DeviceType.h"

#include "propName.h"
#include "propName.moc"

PropName::PropName(QWidget *parent, const QString &name, CircuitWidget *_circuitWidget, Circuit *_circuit, XDevice *_dev)
	: QDialog(parent, Qt::Dialog)
{
    // main variable for this dialog:
    setModal(true);
    setWindowTitle(name);
    dev = _dev;
    circuit = _circuit;
    circuitWidget = _circuitWidget;

    label0 = new QLabel((QWidget *)this);
    label0->setGeometry(30, 10, 43, 20);
    label0->setText(i18n("Name"));
    devname = new KLineEdit((QWidget *)this);
    devname->setGeometry(80, 10, 80, 20);
    if (dev->type() == DeviceType::fIN || dev->type() == DeviceType::fOUT)
	    devname->setMaxLength(4);

    bCancel = new QPushButton(this);
    bCancel->setGeometry(108, 50, 60, 25);
    bCancel->setText(i18n("Cancel"));

    bOK = new QPushButton(this);
    bOK->setGeometry(25, 50, 60, 25);
    bOK->setDefault(true);
    bOK->setText(i18n("OK"));

    setFixedSize(195, 95);
    devname->setText(dev->getName());

    connect(bOK, SIGNAL(clicked()), SLOT(accept()));
    connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
}

void PropName::done(int r)
{
	if (r == Accepted) {
                // check/set new name
		QString s = dev->getName();
		if (0 != devname->text().compare(s)) {
			circuit->setDeviceName(dev->getID(), devname->text());
			dev->setImage();
			circuitWidget->repaint();
		}
	}
	QDialog::done(r);
}

