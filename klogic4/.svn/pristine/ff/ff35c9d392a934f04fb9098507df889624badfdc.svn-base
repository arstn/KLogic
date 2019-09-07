#ifndef __OSCWCLASS
#define __OSCWCLASS

#include <qwidget.h>
class OscContentWidget;
class Circuit;

//************ class *************
class OscWidget : public QWidget
{	Q_OBJECT
public:
	OscWidget();
	virtual ~OscWidget();

	virtual void closeEvent(QCloseEvent *e);
	void displayCircuit(Circuit *);
	void circuitChanged(Circuit *circuit);

signals:
	void oscillatorWidgetClosed();

public slots:
	void simStep();

private:
	OscContentWidget *oscContentWidget;
};
#endif
