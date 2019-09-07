#ifndef __PROPDEV
#define __PROPDEV

#include <qdialog.h>
class KLineEdit;

class QWidget;
class QLabel;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QScrollArea;

class CircuitWidget;
class XDevice;
class InverterChooser;
class ValueChooser;
class RAMWidget;

class PropDev : public QDialog
{
    Q_OBJECT

public:
    PropDev(QWidget *, const QString&, CircuitWidget *, Circuit *, XDevice *);

private:
    void done(int);
    void setSize(int);

    static const int COL1 = 25;
    static const int COL12 = 80;
    static const int COL1_WIDTH = 150;
    static const int COL2 = 200;
    static const int COL2_WIDTH = 110;
    static const int COL3 = 325;
    static const int COL3_WIDTH = 165;
    static const int COL4 = 505;
    static const int COL4_WIDTH = 165;

    QPushButton *bEQU;
    QPushButton *bOK;
    QPushButton *bCancel;
    KLineEdit *devname;
    KLineEdit *delay;
    KLineEdit *size;
    QPushButton *bRAMClrSet;
    QCheckBox *cb_display_name;
    QCheckBox *cb_enable_tristate;
    QCheckBox *cb_HugeWidth;

    QScrollArea *inverterChooserScrollArea;
    InverterChooser *inverterChooser;
    ValueChooser *ledColorChooser;
    ValueChooser *undefChooser;
    ValueChooser *muxChooser;
    ValueChooser *inverterDefaultChooser;
    ValueChooser *clkChooser;
    ValueChooser *masterChooser;
    RAMWidget *ramw;

    CircuitWidget *circuitWidget;
    Circuit *dnet;
    XDevice *dev;

    int osize;
    int ram_clr;

private slots:
    void ramClrSet();
    void showDeviceEquations();
    void setSize(const QString &);
    void setClock(int);
    void setMaster(int);
    void setLEDColor(int);
};

#endif
