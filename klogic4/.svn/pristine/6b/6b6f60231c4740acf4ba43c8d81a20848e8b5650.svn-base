#ifndef __PROPGLOB
#define __PROPGLOB

#include <qdialog.h>

class KLineEdit;

class QGroupBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QComboBox;

class MainWidget;
class CircuitWidget;

class PropGlob : public QDialog
{
    Q_OBJECT

public:
    PropGlob(QWidget *, const QString &name, CircuitWidget *);

private slots:
    void undefChange(int);
    void invertedChange(int);
    void clockChange(int);
    void theFontChange(int);
    void apply();

private:
    void done(int);

    CircuitWidget *netw;

    QGroupBox *gb_dev;
    QLabel *lsize;
    KLineEdit *defsize;
    QLabel *ldelay;
    KLineEdit *defdelay;
    QCheckBox *cb_display_name;
    QCheckBox *cb_ext_width;
    QGroupBox *bg_undef;
    QRadioButton *undef[2];
    QGroupBox *bg_invert;
    QRadioButton *invert[2];
    QComboBox *cb_font_type;
    QComboBox *cb_font_size;

    QGroupBox *bg_clock;
    QRadioButton *clock[7];

    QGroupBox *gb_sim;
    QLabel *lstepTime;
    KLineEdit *stepTime;
    QLabel *lburst;
    KLineEdit *burst;

    QGroupBox *bg_ftype;
    QRadioButton *xml_ftype;
    QRadioButton *klogic_ftype;

    QPushButton *bApply;
    QPushButton *bCancel;
    QPushButton *bOK;

    KlogicGlobal::Device::CLOCKTYPE curr_clk;
    int curr_undef;
    int curr_invert;
};

#endif
