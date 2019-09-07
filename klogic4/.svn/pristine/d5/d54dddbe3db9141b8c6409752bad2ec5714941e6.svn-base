#ifndef __DEVW_CLASS
#define __DEVW_CLASS

class QWidget;
class QPaintEvent;
class QCheckBox;
class QGroupBox;
class QButtonGroup;
class XWire;
class InverterChooser;

#include <xdevice.h>
#include <klogicList.h>

class InverterChooser : public QGroupBox {
	Q_OBJECT
public:
	InverterChooser(QWidget *, XDevice *);
	~InverterChooser();
	void paintEvent(QPaintEvent *);
	void setInputButtons();
	void setOutputButtons();

private slots:
	void inputButton(int);
	void outputButton(int);

private:
	static const int Y_START;
	static const QString SWITCH_STYLE;
	static const int SWITCH_HALF_HEIGHT;

	XDevice *dev;
	QPoint devImagePos;

	QCheckBox *iswitch[KlogicGlobal::Device::MAXSIZE];
	KlogicList<XWire> iwire[KlogicGlobal::Device::MAXSIZE];
	QButtonGroup *bg_inputs;
	QCheckBox *oswitch[KlogicGlobal::Device::MAXSIZE];
	KlogicList<XWire> owire[KlogicGlobal::Device::MAXSIZE];
	QButtonGroup *bg_outputs;
};

#endif
