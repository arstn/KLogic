#ifndef __PROPPWR
#define __PROPPWR

#include<QDialog>
class QLabel;
class QPushButton;
class KLineEdit;
class QRadioButton;
class QGroupBox;

class PropPwr : public QDialog
{
    Q_OBJECT

public:
    PropPwr(QWidget *, const QString &, Circuit *, XDevice *);

private:
    void done(int);

    QPushButton *bOK;
    QPushButton *bCancel;
    QLabel *label0;
    KLineEdit *devname;
    QGroupBox *bgPwr;
    QRadioButton *rbPlus;
    QRadioButton *rbMinus;
    Circuit *dnet;
    XDevice *dev;
};

#endif
