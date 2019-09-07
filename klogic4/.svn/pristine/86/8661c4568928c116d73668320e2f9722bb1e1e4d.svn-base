#ifndef __PROPNAME
#define __PROPNAME


#include <qdialog.h>
class QLabel;
class QPushButton;
class KLineEdit;

class CircuitWidget;
class Circuit;
class XDevice;

class PropName : public QDialog
{
    Q_OBJECT

public:
    PropName(QWidget *, const QString &, CircuitWidget *, Circuit *, XDevice *);

private:
    void done(int);

    QPushButton *bOK;
    QPushButton *bCancel;
    KLineEdit *devname;
    QLabel *label0;
    XDevice *dev;
    Circuit *circuit;
    CircuitWidget *circuitWidget;
};

#endif
