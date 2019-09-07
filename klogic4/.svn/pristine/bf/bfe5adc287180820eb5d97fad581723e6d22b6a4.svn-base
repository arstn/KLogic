#ifndef __DLGPATTERN
#define __DLGPATTERN

#include <QDialog>
#include <QWidget>
class QScrollBar;
class QPushButton;
class QRadioButton;
class QGroupBox;
class QString;
#include<symbolicCalc.h>

class KarnaughWidget : public QWidget
{
	Q_OBJECT
public:
	KarnaughWidget(QWidget *, QString);
	int getInputCounter();
	void calculateNew(int, int);
private:
	void paintEvent(QPaintEvent *);
	void drawKarnaugh();
	bool drawTable();
	void drawQuadTable(int, int);
	void getCoord(const Pattern& oPat, int&, int&);
	void drawPattern();
	void drawGroups();

	SymbolicCalc m_oSymCalc;

	int m_iSize;
};

class DlgPattern : public QDialog
{
	Q_OBJECT

public:
	DlgPattern(QWidget *, QString, QString);

private slots:
	void refresh();
	void vScroll(int);

private:
	void done(int);

	QGroupBox *bgNType;
	QRadioButton *rbSubjunctive;
	QRadioButton *rbDisjunctive;

	QGroupBox *bgNSecurity;
	QRadioButton *rbSecure;
	QRadioButton *rbInsecure;

	QPushButton *bNormalize;

	QPushButton *bOK;
	QWidget *dframe;
	KarnaughWidget *diagram;
	QScrollBar *vscroll;
};

#endif
