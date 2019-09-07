#ifndef __RAMW_CLASS
#define __RAMW_CLASS

#include<QWidget>
#include "imageCache.h"
class QPaintEvent;
class QCheckBox;
class QGroupBox;
class QButtonGroup;
class XDevice;
class XWire;
class RAMWidget;

class RAMWidget : public QGroupBox
{	Q_OBJECT
public:
	RAMWidget(QWidget *, const QString &, XDevice *);
	~RAMWidget();
	void paintEvent(QPaintEvent *);
	void removeBitButtons();
	char *getNewRAMContent();
	void setRAM(char);

private slots:
	void bitModified(int);

private:
	void setBitButtons();

	static const QString SWITCH_STYLE;
	static const int BIT_BUTTON_WIDTH_HEIGTH = 10;

	int byte_cnt;
	QCheckBox *bitswitch[256][8];
	QButtonGroup * buttonGroup;
	Image *pix_inputval;
	char *ram;
};

#endif
