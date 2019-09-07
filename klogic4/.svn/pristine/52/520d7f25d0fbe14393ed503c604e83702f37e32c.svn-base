#ifndef __DLGEQU
#define __DLGEQU

#include <klogicList.h>
#include <QDialog>

class KLineEdit;

class QWidget;
class QGroupBox;
class QPushButton;
class QRadioButton;
class QLabel;
class QString;
class QListWidget;

class Circuit;
class XDevice;
class OutputInfo;

class DlgEqu : public QDialog
{
    Q_OBJECT

public:
    DlgEqu(QWidget *, QString, Circuit *);
    DlgEqu(QWidget *, QString, XDevice *);
    ~DlgEqu();

private slots:
    void editEquation(int);
    void editEquationOK();
    void newEquation();
    void deleteEquation();
    void parseCircuit();
    int substituteEquations();
    void normalizeEquation();
    void normalizeEquations();
    void collectEquations();
    void replaceString();
    void extendDialog();
    void showKarnaugh();

private:
    void createDialogContent(QString);
    void freemem();
    void done(int);
    void editEquationCancel();
    void clearSelection();
    void fillList();
    int changeOutputName(QString, QString);

    static const QString TYPE_INTERN;
    static const QString TYPE_FINAL;
    static const QString TYPE_EQUALS;
    static const QString TYPE_NONE;

    QPushButton *bRestore;
    QPushButton *bSubst;
    QPushButton *bEdit;
    QPushButton *bCollect;
    QPushButton *bNew;
    QPushButton *bDelete;
    QPushButton *bShowKarnaugh;

    QGroupBox *bgType;
    QRadioButton *rbFinal;
    QRadioButton *rbInternal;
    QRadioButton *rbTemp;

    QGroupBox *bgNormalize_s;
    QRadioButton *rbSubjunctive_s;
    QRadioButton *rbDisjunctive_s;
    QPushButton *bNormalize_s;

    QGroupBox *bgNormalize;
    QRadioButton *rbSubjunctive;
    QRadioButton *rbDisjunctive;
    QPushButton *bNormalize;
    int normalize_status;

    QLabel *edit_equation_label;
    KLineEdit *edit_output_name;
    KLineEdit *edit_equation;

    QLabel *list_equ_label;
    QListWidget *list_equ;

    QLabel *global_label;
    QGroupBox *bgReplace;
    QPushButton *bReplace;
    KLineEdit *edit_replace;
    KLineEdit *edit_replace_with;

    QGroupBox *sep1, *sep2;
    QPushButton *bOK;
    QPushButton *bCancel;
    QPushButton *bExtend;
    int dlg_extended;

    QString edit_key;			// the output name of the currently edited equation
    Circuit *net;			// equation source
    XDevice *dev;			// alternate equation source
    KlogicList<OutputInfo> *equations;	// the list of equations
    KlogicList<OutputInfo> *deleted_equations;// the list of deleted equations
    int substituted;			// flag wether the equations are substituted or not
    int edit_slot;			// key of the currently selected equation
};

#endif
