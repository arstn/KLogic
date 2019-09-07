#ifndef __NETWCLASS
#define __NETWCLASS

#include <qframe.h>
#include <QTimer>
class QPainter;
class QRect;
class QMouseEvent;
class QPaintEvent;
class QMenu;

class Circuit;
class XDevice;
class XWire;
class Selection;

#define RPOP_DEL	100
#define RPOP_PROP	101
#define RPOP_SUB	102
#define RPOP_SUBW	103
#define RPOP_GRAPH	104
#define RPOP_SHOWGRAPH	105
#define RPOP_EQU	106
#define RPOP_ADD_LIB	107

class CircuitWidget : public QFrame
{
    Q_OBJECT
public:
    CircuitWidget(QWidget *);
    CircuitWidget(QWidget *, Circuit *);
    ~CircuitWidget();

    Circuit *getActiveCircuit();
    bool contained(Circuit *);

    void activateSelectionMode(bool);
    void removeSelection();

    bool saveAsSubCircuit();
    bool saveCircuit(QString, bool);
    int openCircuit(QString);
    void newCircuit();
    void showCircuitEquations();
    void showDeviceEquations();
    void applyDefaults();
    
    void setContentChanged(bool);
    bool isContentChanged();
    void observedChanged();

protected:
    void paintEvent(QPaintEvent *);
    
signals:
    void visibleCircuitChanged(bool hasParent);

    void openClientWidget(Circuit *);
    void circuitDeleted(Circuit *);

    void circuitChanged(Circuit *);
    void circuitUnchanged();

    void observedChanged(Circuit *);
    void ovserveCircuit(Circuit *);

    void addDeviceToLibrary(XDevice *);
    void autoMove(const QPoint&);

public slots:
    void cut();
    void copy();
    void paste();
    void pasteAt();

    void toParentCircuit();

private slots:
	bool toSubCircuitW();
	bool toSubCircuit();
	void deleteActiveDevice(bool);
    void openActiveDeviceProperties();
    void addActiveDeviceToLib();
    void showCircuitInOscilloscope();
    void addActiveDeviceToOscilloscope();

private:
    void initialize(Circuit *net);

    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void mouseDoubleClickEvent( QMouseEvent * );
    void selRightMenu(QMouseEvent *);
    void selRightSelMenu(QMouseEvent *);

    void showPropertiesDialog(QDialog *dlg);

    Circuit *activeCircuit;
    XDevice *activeDevice;
    XWire *activeWire;
    Selection *m_poActiveSelection;

    QMenu *rpop;
    QMenu *rselpop;

    bool changed;
    int m_bSelMoveCursor;
    int m_bRMenuActive;
};

#endif
