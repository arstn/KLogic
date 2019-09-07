#ifndef __CLIENTWCLASS
#define __CLIENTWCLASS

#include<qwidget.h>

//************ class *************
class ClientWidget : public QWidget
{	Q_OBJECT
public:
	ClientWidget(Circuit *circuit);
	virtual ~ClientWidget();

	CircuitWidget * getCircuitWidget();
	void applyFunctionToGUI(int function);
	virtual void closeEvent(QCloseEvent *e);

signals:
	void clientWidgetClosed(ClientWidget *);

private slots:
	void visibleCircuitChanged(bool);
    void fIN();
    void fOUT();

private:
	void initialize();

	QToolBar *tb;
	ScrollableCircuitWidget *scollableCircuitWidget;
    QAction *fINAction;
    QAction *fOUTAction;
};
#endif
