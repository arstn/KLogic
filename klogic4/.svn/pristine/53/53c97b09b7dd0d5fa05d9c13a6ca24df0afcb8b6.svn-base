/*************************************************/
/* methods for class PropOsz                     */
/*                                               */
/* oszillator properties dialog                  */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <klineedit.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qvalidator.h>

#include <KlogicGlobal.h>
#include <deviceOsc.h>
#include <xdevice.h>
#include <circuit.h>

#include <propOsz.h>
#include "propOsz.moc"

PropOsz::PropOsz(QWidget *parent, const QString &name, Circuit *_dnet, XDevice *_dev)
	: QDialog(parent, Qt::Dialog)
{
	setModal(true);
	setWindowTitle(name);
	dev = _dev;
	dnet = _dnet;

	label0 = new QLabel(this);
	label0->setGeometry(65, 10, 40, 20);
	label0->setText(i18n("Name"));
	devname = new KLineEdit(this);
	devname->setGeometry(105, 10, 180, 20);

	label1 = new QLabel(this);
	label1->setGeometry(30, 40, 75, 20);
	label1->setText(i18n("high period"));
	onCnt = new KLineEdit(this);
	QIntValidator *vOnCnt = new QIntValidator(onCnt);
	onCnt->setValidator(vOnCnt);
	onCnt->setGeometry(105, 40, 90, 20);

	label2 = new QLabel(this);
	label2->setGeometry(34, 70, 75, 20);
	label2->setText(i18n("low period"));
	offCnt = new KLineEdit(this);
	QIntValidator *vOffCnt = new QIntValidator(offCnt);
	offCnt->setValidator(vOffCnt);
	offCnt->setGeometry(105, 70, 90, 20);

	cb_type = new QCheckBox(i18n("Monoflop"), this);
	cb_type->setGeometry(105, 100, 90, 15);
	if (dev->getOscType() == OscillatorDevice::TYPE_MONO) {
		cb_type->setChecked(TRUE);
		offCnt->setEnabled(FALSE);
	} else {
		cb_type->setChecked(FALSE);
		offCnt->setEnabled(TRUE);
	}

	//******************************************************
	bCancel = new QPushButton(this);
	bCancel->setGeometry(180, 130, 70, 30);
	bCancel->setText(i18n("Cancel"));

	bOK = new QPushButton(this);
	bOK->setGeometry(70, 130, 70, 30);
	bOK->setDefault(true);
	bOK->setText(i18n("OK"));

	devname->setMaxLength(80);
	onCnt->setMaxLength(5);
	offCnt->setMaxLength(5);
	setFixedSize(310,180);

	devname->setText(dev->getName());
	QString value;
	onCnt->setText(value.setNum(dev->getOscOn()));
	offCnt->setText(value.setNum(dev->getOscOff()));

	connect(bOK, SIGNAL(clicked()), SLOT(accept()));
	connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
	connect(cb_type, SIGNAL(clicked()), SLOT(mono()));
}

void PropOsz::done(int r)
{	int ivalue;
	QString value;

	if (r == Accepted) {
		// set new name
		dnet->setDeviceName(dev->getID(), devname->text());

		value = onCnt->text();
		ivalue = value.toInt();
		if (ivalue > OscillatorDevice::MAXOSCCNT)
			ivalue = OscillatorDevice::MAXOSCCNT;
		if (ivalue < 1) ivalue = 1;
		dev->setOscOn(ivalue);

		value = offCnt->text();
		ivalue = value.toInt();
		if (ivalue > OscillatorDevice::MAXOSCCNT)
			ivalue = OscillatorDevice::MAXOSCCNT;
		if (ivalue < 1) ivalue = 1;
		dev->setOscOff(ivalue);
		if (dev->getOscType() != OscillatorDevice::TYPE_MULTI && !cb_type->isChecked()) {
			dev->setOscType(OscillatorDevice::TYPE_MULTI);
		} else if (dev->getOscType() != OscillatorDevice::TYPE_MONO && cb_type->isChecked()) {
			dev->setOscType(OscillatorDevice::TYPE_MONO);
		}
	}
	QDialog::done(r);
}

void PropOsz::mono()
{
	if (cb_type->isChecked()) offCnt->setEnabled(FALSE);
	else offCnt->setEnabled(TRUE);

}
