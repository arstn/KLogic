#ifndef __OSCCWCLASS
#define __OSCCWCLASS

#include <qwidget.h>

//laurent
//#include <kstatusbar.h>

#include "OscChannel.h"
#include "circuit.h"

#include <list>
using namespace std;

class OscContentWidget : public QWidget
{	Q_OBJECT
public:
	OscContentWidget(QWidget *);
	virtual ~OscContentWidget();

	void displayCircuit(Circuit *);
	void circuitChanged(Circuit *circuit);
	void simStep();

	static const int WIDTH = 800;
	static const unsigned int CHANNEL_START = 50;
	static const unsigned int CHANNEL_SIZE = (unsigned int)WIDTH - 60;
	static const int ROW_HEIGHT = 24;
	static const int CHANNEL_HEIGHT = 22;
	static const int CHANNEL_HEIGHT_HALF = 11;
	static const QColor LT_GRAY_COLOR;
	static const QBrush LT_GRAY_BRUSH;

signals:
	void resized();
public slots:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent *);
	void paintEvent(QPaintEvent *);

private:
	Circuit *circuit;
	list<OscChannel *> channels;
	int currentMouseXPos;
};

#endif
