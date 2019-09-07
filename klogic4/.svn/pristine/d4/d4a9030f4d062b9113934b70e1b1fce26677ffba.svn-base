/*************************************************/
/* methods for class PropPwr                     */
/*                                               */
/* power source properties dialog                */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <klineedit.h>

#include <qdialog.h>
#include <qlabel.h>
#include <QGroupBox>
#include <qpushbutton.h>
#include <qradiobutton.h>

#include <KlogicGlobal.h>
#include <xdevice.h>
#include <circuit.h>
#include <propPwr.h>

#include "propPwr.moc"

PropPwr::PropPwr(QWidget *parent, const QString &name, Circuit *_dnet, XDevice *_dev)
	: QDialog(parent, Qt::Dialog)
{
	setModal(true);
	setWindowTitle(name);
	dev = _dev;
	dnet = _dnet;

	label0 = new QLabel(this);
	label0->setGeometry(30, 10, 43, 20);
	label0->setText(i18n("Name"));
	devname = new KLineEdit(this);
	devname->setGeometry(80, 10, 110, 20);
	devname->setEnabled(FALSE);
	devname->setText(dev->getName());

	bgPwr = new QGroupBox(i18n("output value"), this);
	bgPwr->setGeometry(40, 40, 130, 60);
	rbPlus = new QRadioButton(i18n("High"), bgPwr);
	rbPlus->setGeometry(10,20, 100, 15);
	rbMinus = new QRadioButton(i18n("Low"), bgPwr);
	rbMinus->setGeometry(10,40, 100, 15);
	if (dev->output()) rbPlus->setChecked(TRUE);
	else rbMinus->setChecked(TRUE);

	bCancel = new QPushButton(this);
	bCancel->setGeometry(130, 120, 60, 30);
	bCancel->setText(i18n("Cancel"));

	bOK = new QPushButton(this);
	bOK->setGeometry(20, 120, 60, 30);
	bOK->setDefault(TRUE);
	bOK->setText(i18n("OK"));

	setFixedSize(210, 170);

	connect(bOK, SIGNAL(clicked()), SLOT(accept()));
	connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
}

void PropPwr::done(int r)
{
	if (r == Accepted) {
		dnet->setDeviceName(dev->getID(), devname->text());
		if (rbPlus->isChecked()) {
			dev->setInputValue(1);
			dev->setImage();
		}
		else {
			dev->setInputValue(0);
			dev->setImage();
		}
	}
	QDialog::done(r);
}

