/************************************/
/* widget methods for PropDev       */
/*                                  */
/* device propperties dialog        */
/*                                  */
/* Andreas Rostin                   */
/* 13.11.98                         */
/************************************/
#include <klineedit.h>

#include <qwidget.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <QVBoxLayout>
#include <qvalidator.h>
#include <qscrollarea.h>

#include "KlogicGlobal.h"
#include "circuit.h"
#include "xdevice.h"
#include "device.h"
#include "InverterChooser.h"
#include "ValueChooser.h"
#include "ramw.h"
#include "dlgEqu.h"
#include "deviceColored.h"
#include "DeviceType.h"
#include "CircuitWidget.h"

#include "propDev.h"
#include "propDev.moc"

PropDev::PropDev(QWidget *parent, const QString& name,
		CircuitWidget *_circuitWidget, Circuit *_dnet, XDevice *_dev) :
		QDialog(parent, Qt::Dialog) {
	setModal(true);
	setWindowTitle(name);
	QLabel *label;
	QString value;

	// object to modify in this dialog:
	dev = _dev;
	circuitWidget = _circuitWidget;
	dnet = _dnet;

	osize = dev->getSize();

	label = new QLabel(this);
	label->setGeometry(COL1 + 5, 10, 43, 20);
	label->setText(i18n("Name"));
	devname = new KLineEdit(this);
	devname->setEnabled(TRUE);

	label = new QLabel(this);
	label->setText(i18n("Size"));
	size = new KLineEdit(this);
	QIntValidator *vSize = new QIntValidator(size);
	size->setValidator(vSize);
	if (!dev->sizeChangeable()) {
		size->setEnabled(FALSE);
	}
	label->setGeometry(COL1 + 5, 40, 32, 20);
	size->setGeometry(COL12, 40, 40, 20);
	connect(size, SIGNAL(textChanged(const QString &)), this, SLOT(setSize(const QString &)));

	delay = NULL;
	if (dev->type() != DeviceType::fBUS) {
		label = new QLabel(this);
		label->setGeometry(30, 70, 43, 20);
		label->setText(i18n("Delay"));
		delay = new KLineEdit(this);
		QIntValidator *vDelay = new QIntValidator(delay);
		delay->setValidator(vDelay);
		delay->setGeometry(COL12, 70, 40, 20);
		delay->setMaxLength(4);
		delay->setText(value.setNum(dev->getDelay()));
	}

	cb_display_name = NULL;
	if (dev->type() != DeviceType::fBUS) {
		cb_display_name = new QCheckBox(i18n("display name"), this);
		cb_display_name->setGeometry(COL2, 40, 100, 20);
		if (dev->nameDisplayed()) {
			cb_display_name->setChecked(TRUE);
		} else {
			cb_display_name->setChecked(FALSE);
		}
	}

	cb_HugeWidth = NULL;
	if (dev->type() == DeviceType::fSUB || dev->type() == DeviceType::fEQU) {
		cb_HugeWidth = new QCheckBox(i18n("Large Width"), this);
		cb_HugeWidth->setGeometry(COL2, 70, 100, 20);
		if (dev->width() == 3) {
			cb_HugeWidth->setChecked(TRUE);
		} else {
			cb_HugeWidth->setChecked(FALSE);
		}
	}

	inverterChooser = NULL;
	if (dev->type() != DeviceType::fBUS) {
		label = new QLabel(this);
		label->setGeometry(COL2, 95, 110, 20);
		label->setText(i18n("Inverter Setting"));

		inverterChooser = new InverterChooser(this, dev);
		inverterChooserScrollArea = new QScrollArea(this);
		inverterChooserScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		inverterChooserScrollArea->setWidget(inverterChooser);
		// geometry of inverterChooserScrollArea and inverterChooser is set by setSize method
	}

	undefChooser = NULL;
	muxChooser = NULL;
	if (dev->type() != DeviceType::fBUS) {
		list<string> nameList;
		nameList.push_back("Low");
		nameList.push_back("High");
		undefChooser = new ValueChooser(this, "Undefined value", nameList, dev->getUndefinedValue());
		undefChooser->setGeometry(COL1, 95, 150, 80);
	} else {
		list<string> nameList;
		nameList.push_back("Fan in");
		nameList.push_back("Fan out");
		muxChooser = new ValueChooser(this, "Bus Type", nameList, (int)dev->isMux());
		muxChooser->setGeometry(COL1, 95, 150, 80);
		connect	(muxChooser, SIGNAL(valueChosen(int)), this, SLOT(setMux(int)));

		if (dev->type() == DeviceType::fONE || dev->getMaxOutputCount() == 0) {
			muxChooser->setEnabled(FALSE);
		}
	}

	ledColorChooser = NULL;
	inverterDefaultChooser = NULL;
	if (dev->type() == DeviceType::fLED) {
		list<string> nameList;
		nameList.push_back("green");
		nameList.push_back("red");
		nameList.push_back("yellow");
		nameList.push_back("blue");
		ledColorChooser = new ValueChooser(this, "LED Color", nameList, dev->getColor());
		ledColorChooser->setGeometry(COL1, 180, 150, 120);
		connect	(ledColorChooser, SIGNAL(valueChosen(int)), this, SLOT(setLEDColor(int)));
	} else if (dev->type() != DeviceType::fBUS) {
		list<string> nameList;
		nameList.push_back("Not inverted");
		nameList.push_back("Inverted");
		inverterDefaultChooser = new ValueChooser(this, "Output always inverted", nameList, (int)dev->isInverted());
		inverterDefaultChooser->setGeometry(COL1, 180, 150, 80);
		if (dev->type() == DeviceType::fONE || dev->getMaxOutputCount() == 0) {
			inverterDefaultChooser->setEnabled(FALSE);
		}
	}

	cb_enable_tristate = NULL;
	if (dev->type() == DeviceType::fEQU || dev->type() == DeviceType::fRAM) {
		cb_enable_tristate = new QCheckBox(i18n("Tristate outputs"), this);
		cb_enable_tristate->setGeometry(COL1, 265, 150, 25);
		if (dev->isDeviceTristateEnabled())
			cb_enable_tristate->setChecked(TRUE);
		else
			cb_enable_tristate->setChecked(FALSE);
	}

	if (dev->type() == DeviceType::fEQU) {
		bEQU = new QPushButton(this);
		bEQU->setText(i18n("Equation Editor"));
		bEQU->setGeometry(COL1, 295, 150, 25);
		connect(bEQU, SIGNAL(clicked()), this, SLOT(showDeviceEquations()));
	}

	clkChooser = NULL;
	masterChooser = NULL;
	if (dev->hasClock()) {
		list<string> nameList;
		list<int> valueList;
		if (dev->type() == DeviceType::fJK) {
			nameList.push_back("Rising edge");
			valueList.push_back((int)KlogicGlobal::Device::CLK_RISING_1EDGE);
			nameList.push_back("Falling edge");
			valueList.push_back((int)KlogicGlobal::Device::CLK_FALLING_1EDGE);
			nameList.push_back("Rising 2 edge");
			valueList.push_back((int)KlogicGlobal::Device::CLK_RISING_2EDGE);
			nameList.push_back("Falling 2 edge");
			valueList.push_back((int)KlogicGlobal::Device::CLK_FALLING_2EDGE);
			nameList.push_back("High value");
			valueList.push_back((int)KlogicGlobal::Device::CLK_HIGH_VALUE);
			nameList.push_back("Low value");
			valueList.push_back((int)KlogicGlobal::Device::CLK_LOW_VALUE);
			nameList.push_back("Disabled");
			valueList.push_back((int)KlogicGlobal::Device::CLK_NONE);
		} else {
			nameList.push_back("Rising edge");
			valueList.push_back((int)KlogicGlobal::Device::CLK_RISING_1EDGE);
			nameList.push_back("Falling edge");
			valueList.push_back((int)KlogicGlobal::Device::CLK_FALLING_1EDGE);
			nameList.push_back("High value");
			valueList.push_back((int)KlogicGlobal::Device::CLK_HIGH_VALUE);
			nameList.push_back("Low value");
			valueList.push_back((int)KlogicGlobal::Device::CLK_LOW_VALUE);
			nameList.push_back("Disabled");
			valueList.push_back((int)KlogicGlobal::Device::CLK_NONE);
		}
		clkChooser = new ValueChooser(this, "Clock Behaviour", nameList, valueList, dev->clock());
		clkChooser->setGeometry(COL3, 95, COL3_WIDTH, 180);

		if (dev->type() != DeviceType::fRS || dev->hasMaster()) {
			clkChooser->setEnabled(KlogicGlobal::Device::CLK_NONE, false);
		}
		if (!dev->hasMaster()) {
			clkChooser->setEnabled(KlogicGlobal::Device::CLK_RISING_1EDGE, false);
			clkChooser->setEnabled(KlogicGlobal::Device::CLK_FALLING_1EDGE, false);
			clkChooser->setEnabled(KlogicGlobal::Device::CLK_RISING_2EDGE, false);
			clkChooser->setEnabled(KlogicGlobal::Device::CLK_FALLING_2EDGE, false);
		}
		connect	(clkChooser, SIGNAL(valueChosen(int)), this, SLOT(setClock(int)));

		list<string> masterNameList;
		masterNameList.push_back("Disabled");
		masterNameList.push_back("Enabled");
		masterChooser = new ValueChooser(this, "Master FF", masterNameList, (int)dev->hasMaster());
		masterChooser->setGeometry(COL4, 95, COL4_WIDTH, 80);
		if (dev->type() == DeviceType::fJK) {
			masterChooser->setEnabled(false);
		}
		connect(masterChooser, SIGNAL(valueChosen(int)), this, SLOT(setMaster(int)));
	}

	ramw = NULL;
	if (dev->type() == DeviceType::fRAM) {
		ramw = new RAMWidget(this, name, dev);

		QScrollArea *ramScrollArea = new QScrollArea(this);
		ramScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		ramScrollArea->setGeometry(COL3, 112, COL3_WIDTH, 195);
		ramScrollArea->setWidget(ramw);

		label = new QLabel(this);
		label->setGeometry(COL3, 92, COL3_WIDTH, 20);
		label->setText(i18n("RAM Content"));

		bRAMClrSet = new QPushButton(this);
		bRAMClrSet->setGeometry(405, 92, 50, 20);
		bRAMClrSet->setText(i18n("clear"));
		connect(bRAMClrSet, SIGNAL(clicked()), SLOT(ramClrSet()));
		ram_clr = 1;
	}

	bOK = new QPushButton(this);
	bOK->setDefault(true);
	bOK->setText(i18n("OK"));

	bCancel = new QPushButton(this);
	bCancel->setText(i18n("Cancel"));

	size->setMaxLength(2);
	devname->setMaxLength(80);

	if (dev->hasClock()) {
		bOK->setGeometry(80, 330, 70, 25);
		bCancel->setGeometry(170, 330, 70, 25);
		devname->setGeometry(80, 10, 385, 20);
		setFixedSize(COL4 + COL4_WIDTH + 25, 400);
	} else if (dev->type() == DeviceType::fRAM) {
		bOK->setGeometry(80, 330, 70, 25);
		bCancel->setGeometry(170, 330, 70, 25);
		devname->setGeometry(80, 10, 385, 20);
		setFixedSize(COL3 + COL3_WIDTH + 25, 400);
	} else {
		bOK->setGeometry(80, 330, 70, 25);
		bCancel->setGeometry(170, 330, 70, 25);
		devname->setGeometry(80, 10, 215, 20);
		setFixedSize(COL2 + COL2_WIDTH + 25, 370);
	}

	devname->setText(dev->getName());
	size->setText(value.setNum(dev->getSize()));
	setSize(dev->getSize());

	connect(bOK, SIGNAL(clicked()), SLOT(accept()));
	connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
}

void PropDev::done(int r) {
	QString value;
	int ivalue;

	if (r == Accepted) {
		// set new name
		dnet->setDeviceName(dev->getID(), devname->text());

		// delay
		if (delay) {
			value = delay->text();
			ivalue = value.toInt();
			dev->setDelay(ivalue);
		}

		// size set by signal

		// switches..
		if (inverterDefaultChooser != NULL) {
			dev->setInverted(inverterDefaultChooser->getChosenValue());
		}
		if (undefChooser != NULL) {
			dev->setUndefinedValue(undefChooser->getChosenValue());
		}
		if (clkChooser != NULL) {
			setClock(clkChooser->getChosenValue());
		}
		if (cb_display_name) {
			// display the device name
			if (cb_display_name->isChecked())
				dev->displayName(true);
			else
				dev->displayName(false);
		}

		// extend width
		if (cb_HugeWidth) {
			if (cb_HugeWidth->isChecked()) {
				if (dev->width() != 3)
					dev->setWidth(3);
			} else {
				if (dev->width() != 2)
					dev->setWidth(2);
			}
		}

		if (ramw) {
			dev->setRAMStorage(ramw->getNewRAMContent());
			delete ramw;
		}
		if (cb_enable_tristate) {
			dev->enableTristateInput(cb_enable_tristate->isChecked());
		}
		if (muxChooser != NULL) {
			dev->setMux(muxChooser->getChosenValue());
		}
		if (ledColorChooser != NULL) {
			dev->setColor(ledColorChooser->getChosenValue());
		}

		circuitWidget->repaint();
	} else {
		if (dev->getSize() != osize) {
			setSize(osize);
		}
		if (masterChooser != NULL) {
			setMaster(masterChooser->getInitialValue());
		}
		if (clkChooser != NULL) {
			setClock(clkChooser->getInitialValue());
		}
		if (ledColorChooser != NULL) {
			setLEDColor(ledColorChooser->getInitialValue());
		}
		circuitWidget->repaint();
	}
	QDialog::done(r);
}

void PropDev::showDeviceEquations() {
	DlgEqu * equation_dialog = new DlgEqu(this, i18n("Device Equation Editor"),
			dev);
	equation_dialog->exec();
	delete equation_dialog;
}

void PropDev::setLEDColor(int color) {
	dev->setColor(color);
	inverterChooser->repaint();
}

void PropDev::setClock(int iIntClock) {
	dev->setClock((KlogicGlobal::Device::CLOCKTYPE)iIntClock);
	inverterChooser->setInputButtons();
	inverterChooser->repaint();
}

void PropDev::setMaster(int m) {
	dev->setMaster(m);
	if (!dev->hasMaster()) {
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_RISING_1EDGE, false);
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_FALLING_1EDGE, false);
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_RISING_2EDGE, false);
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_FALLING_2EDGE, false);
		if (clkChooser->getChosenValue() == KlogicGlobal::Device::CLK_RISING_1EDGE ||
				clkChooser->getChosenValue() == KlogicGlobal::Device::CLK_RISING_2EDGE) {
			clkChooser->setCurrentValue((int)KlogicGlobal::Device::CLK_HIGH_VALUE);
			inverterChooser->setInputButtons();
		}
		if (clkChooser->getChosenValue() == KlogicGlobal::Device::CLK_FALLING_1EDGE ||
				clkChooser->getChosenValue() == KlogicGlobal::Device::CLK_FALLING_2EDGE) {
			clkChooser->setCurrentValue((int)KlogicGlobal::Device::CLK_LOW_VALUE);
			inverterChooser->setInputButtons();
		}
	} else {
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_RISING_1EDGE, true);
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_FALLING_1EDGE, true);
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_RISING_2EDGE, true);
		clkChooser->setEnabled(KlogicGlobal::Device::CLK_FALLING_2EDGE, true);
		if (clkChooser->getChosenValue() == KlogicGlobal::Device::CLK_NONE) {
			clkChooser->setCurrentValue((int)KlogicGlobal::Device::CLK_RISING_1EDGE);
			inverterChooser->setInputButtons();
		}
	}
	dev->setImage();
	inverterChooser->repaint();
}

void PropDev::setSize(const QString& value) {
	setSize(value.toInt());
}

void PropDev::setSize(int isize) {
	if (dev->getSize() != isize) {
		dev->setSize(isize);
		dev->setImage();
	}
	if (inverterChooser) {
		int height = isize * Grid::GRID + 20;
		if (height < 208 - 6) {
			height = 208 - 6;
			inverterChooserScrollArea->setGeometry(190, 112, 110, 208);
		} else {
			inverterChooserScrollArea->setGeometry(190, 112, 125, 208);
		}
		inverterChooser->setGeometry(0, 0, 110, height);
		inverterChooser->repaint();
	}
}

// private slot
void PropDev::ramClrSet() {
	if (ram_clr) {
		ram_clr = 0;
		bRAMClrSet->setText(i18n("set"));
		ramw->setRAM(0);
	} else {
		ram_clr = 1;
		bRAMClrSet->setText(i18n("clear"));
		ramw->setRAM(255);
	}
}

