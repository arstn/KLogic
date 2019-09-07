/************************************/
/* methods of RAMWidget             */
/* methods of RAMScrollWidget        */
/*                                  */
/* inverter dialog for devices      */
/* is part of device prop dialog    */
/*                                  */
/* Andreas Rostin                   */
/* 16.12.98                         */
/*                                  */
/* RAMScrollWidget extension:        */
/* Andreas Rostin                   */
/* 04.03.2000                       */
/************************************/
#include <qwidget.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qpaintdevice.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "ramw.h"
#include "xdevice.h"
#include "KlogicGlobal.h"
#include "tool.h"
#include "grid.h"

#include "ramw.moc"

const QString RAMWidget::SWITCH_STYLE = "QCheckBox::indicator { width: 10px; height: 10px; }";

RAMWidget::RAMWidget(QWidget *parent, const QString &name, XDevice *_dev)
	: QGroupBox(parent)
{
	setWindowTitle(name);
	setBackgroundRole(QPalette::Base);
	
	ram = _dev->getRAMStorageCopy();
	byte_cnt = _dev->RAMSize();
	pix_inputval = new Image();
	pix_inputval->fill();

	for (int i = 0; i < byte_cnt; i++) {
		for (int j = 0; j < 8; j++) {
			bitswitch[i][j] = (QCheckBox *)NULL;
		}
	}
	buttonGroup = NULL;
	setBitButtons();
}

RAMWidget::~RAMWidget()
{
	if (ram) delete [] ram;
	delete pix_inputval;
	removeBitButtons();
}

char * RAMWidget::getNewRAMContent()
{
	return ram;
}

void RAMWidget::paintEvent(QPaintEvent *pev)
{
	QWidget::paintEvent(pev);
	QPainter p;
	p.begin(this);
	p.drawPixmap(0, 0, *pix_inputval);
	p.end();
}

// slot for setting/resetting a bit
void RAMWidget::bitModified(int no)
{
	int i = no / 8;
	int j = no % 8;
	if (bitswitch[i][j]->isChecked()) {
		ram[i] = ram[i] | Tool::pow2(j);
	} else {
		int bitval = Tool::pow2(j);
		if (ram[i] & bitval) ram[i] -= bitval;
	}
}

// init bit buttons
void RAMWidget::setBitButtons()
{
	removeBitButtons();
	setGeometry(0, 0, 8 * BIT_BUTTON_WIDTH_HEIGTH + 80, byte_cnt * BIT_BUTTON_WIDTH_HEIGTH);

	buttonGroup = new QButtonGroup();
	buttonGroup->setExclusive(false);

	delete pix_inputval;
	pix_inputval = new Image(40, BIT_BUTTON_WIDTH_HEIGTH * byte_cnt);
	pix_inputval->fill();

	// display input value in front
	QPainter p;
	p.begin(pix_inputval);
	p.setPen(Qt::black);
	p.setBrush(Qt::black);
	p.setFont(QFont( "helvetica", 10, QFont::Bold ) );

	for(int i = 0; i < byte_cnt; i++) {
		p.drawText(5, (i + 1) * BIT_BUTTON_WIDTH_HEIGTH - 1, Tool::bitString((char)i, 5));

		// each bit a button: output value
		for(int j = 0; j < 8; j++) {
			bitswitch[i][j] = new QCheckBox(this);
			bitswitch[i][j]->setGeometry(40 + (7 - j) * BIT_BUTTON_WIDTH_HEIGTH, i * BIT_BUTTON_WIDTH_HEIGTH, BIT_BUTTON_WIDTH_HEIGTH, BIT_BUTTON_WIDTH_HEIGTH);
			bitswitch[i][j]->setStyleSheet(SWITCH_STYLE);
			bitswitch[i][j]->setChecked(ram[i] & Tool::pow2(j));
			bitswitch[i][j]->show();
			buttonGroup->addButton(bitswitch[i][j], i * 8 + j);
		}
	}
	p.end();
	connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(bitModified(int)));
}

void RAMWidget::removeBitButtons()
{
	if (buttonGroup) {
		disconnect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(bitModified(int)));
		for(int i = 0; i < byte_cnt; i++) {
			for(int j = 0; j < 8; j++) {
				if (bitswitch[i][j]) {
					delete bitswitch[i][j];
				}
			}
		}
		delete buttonGroup;
	}
}

// set each byte of the ram to the given value
void RAMWidget::setRAM(char bytecontent)
{
	for(int i = 0; i < byte_cnt; i++) {
		ram[i] = bytecontent;
		for(int j = 0; j < 8; j++) {
			bitswitch[i][j]->setChecked(ram[i] & Tool::pow2(j));
		}
	}
}
