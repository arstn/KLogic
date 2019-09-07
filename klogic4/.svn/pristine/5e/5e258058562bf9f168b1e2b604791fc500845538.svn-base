/************************************/
/* methods of InverterWidget             */
/* methods of InverterScrollWidget        */
/*                                  */
/* inverter dialog for devices      */
/* is part of device prop dialog    */
/*                                  */
/* Andreas Rostin                   */
/* 16.12.98                         */
/*                                  */
/* InverterScrollWidget extension:        */
/* Andreas Rostin                   */
/* 04.03.2000                       */
/************************************/
#include <qwidget.h>
#include <qdialog.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <QVBoxLayout>
#include <qcheckbox.h>
#include <qpaintdevice.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qscrollbar.h>
#include <qlabel.h>

#include "InverterChooser.h"
#include "xdevice.h"
#include "xwire.h"
#include "KlogicGlobal.h"
#include "grid.h"

#include "InverterChooser.moc"

const int InverterChooser::Y_START = 10;
const QString InverterChooser::SWITCH_STYLE = "QCheckBox::indicator { width: 14px; height: 14px; }";
const int InverterChooser::SWITCH_HALF_HEIGHT = 6;

InverterChooser::InverterChooser(QWidget *parent, XDevice *_dev)
	: QGroupBox(parent)
{
	dev = _dev;
	
	setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

	for (int i = 0; i < KlogicGlobal::Device::MAXSIZE; i++) {
		iswitch[i] = (QCheckBox *)NULL;
		oswitch[i] = (QCheckBox *)NULL;
	}

	// set switches for each input
	bg_inputs = NULL;
	setInputButtons();
	// set switches for each output
	bg_outputs = NULL;
	setOutputButtons();
}

InverterChooser::~InverterChooser() {
	for (int i = 0; i < KlogicGlobal::Device::MAXSIZE;i++) {
		iwire[i].Destroy();
	}
	for (int i = 0; i < KlogicGlobal::Device::MAXSIZE;i++) {
		owire[i].Destroy();
	}
}

void InverterChooser::paintEvent(QPaintEvent *e) {
	QWidget::paintEvent(e);

	const Image *poImage = dev->getImage();
	devImagePos = QPoint((rect().width() - poImage->width()) / 2, Y_START);

	QPainter p;
	p.begin(this);
	p.drawPixmap(devImagePos.x(), devImagePos.y() , *poImage);
	p.end();
}

// slot for input-buttons-group
void InverterChooser::inputButton(int no) {
	KlogicList<XWire> *lw = iwire[no].First();
	while(lw) {
		lw->Get()->invertInput(dev, iswitch[no]->isChecked());
		dev->setImage();
		lw = lw->Next();
	}
	repaint();
}
// slot for input-buttons-group
void InverterChooser::outputButton(int no) {
	KlogicList<XWire> *lw = owire[no].First();
	while(lw) {
		lw->Get()->invertOutput(dev, oswitch[no]->isChecked());
		dev->setImage();
		lw = lw->Next();
	}
	repaint();
}

void InverterChooser::setInputButtons() {
	if (bg_inputs) {
		disconnect(bg_inputs, SIGNAL(buttonClicked(int)), this, SLOT(inputButton(int)));
		delete bg_inputs;
		for (int i = 0; i < KlogicGlobal::Device::MAXSIZE;i++) {
			iwire[i].Destroy();
		}
	}

	bg_inputs = new QButtonGroup(this);
	bg_inputs->setExclusive(false);

	int idx = 0;
	KlogicList<XWire> *lw = dev->getIRef()->First();
	while (lw && lw->Get()) {
		XWire *w = lw->Get();

		int devIdx = w->getInputIndex(dev);
		QPoint pt = w->getInputPosition(devIdx);
		int y =  pt.y() - dev->getPos().y() + Y_START - SWITCH_HALF_HEIGHT;

		// each y only once!
		for(int j=0;((j < idx) && y);j++) {
			if (iswitch[j]->geometry().y() == y) {
				y = 0;
				iwire[j].Append(w);
			}
		}

		if (y) {
			iwire[idx].Append(w);
			iswitch[idx] = new QCheckBox(this);
			iswitch[idx]->setStyleSheet(SWITCH_STYLE);
			iswitch[idx]->move(5, y);
			iswitch[idx]->setChecked(w->inputIsInverted(dev));
			bg_inputs->addButton(iswitch[idx], idx);
			idx++;
		}

		lw = lw->Next();
	}
	connect(bg_inputs, SIGNAL(buttonClicked(int)), this, SLOT(inputButton(int)));
}

void InverterChooser::setOutputButtons()
{
	if (bg_outputs) {
		disconnect(bg_outputs, SIGNAL(buttonClicked(int)), this, SLOT(outputButton(int)));
		delete bg_outputs;
		for (int i = 0; i < KlogicGlobal::Device::MAXSIZE;i++) {
			owire[i].Destroy();
		}
	}

	bg_outputs = new QButtonGroup(this);
	bg_outputs->setExclusive(false);

	int idx = 0;
	KlogicList<XWire> *lw = dev->getORef()->First();
	while (lw && lw->Get()) {
		XWire *w = lw->Get();
		int devIdx = w->getOutputIndex(dev);
		QPoint pt = w->getOutputPosition(devIdx);
		int y =  pt.y() - dev->getPos().y() + Y_START - SWITCH_HALF_HEIGHT;
		// each y only once!
		for(int j = 0; (j < idx) && y; j++) {
			if (oswitch[j]->geometry().y() == y) {
				y = 0;
				owire[j].Append(w);
			}
		}
		if (y) {
			owire[idx].Append(w);
			oswitch[idx] = new QCheckBox(this);
			oswitch[idx]->setStyleSheet(SWITCH_STYLE);
			oswitch[idx]->move(85, y);
			oswitch[idx]->setChecked(w->outputIsInverted(dev));
			bg_outputs->addButton(oswitch[idx], idx);
			idx++;
		}
		lw = lw->Next();
	}
	connect(bg_outputs, SIGNAL(buttonClicked(int)), this, SLOT(outputButton(int)));
}

