#ifndef __PROPTEXT
#define __PROPTEXT

class KLineEdit;

class QPushButton;
class Circuit;
class XDevice;

class PropText : public QDialog
{
    Q_OBJECT

public:
    PropText(QWidget *, const QString &, Circuit *, XDevice *);

private:
    void done(int);

    QPushButton *bOK;
    QPushButton *bCancel;
    KLineEdit *devtext;
    Circuit *dnet;
    XDevice *dev;
};

#endif
