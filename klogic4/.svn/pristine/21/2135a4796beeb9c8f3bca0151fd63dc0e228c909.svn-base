/*************************************************/
/* methods for class DlgPattern                  */
/*                                               */
/* dialog used to display all patterns of an     */
/* equation                                      */
/*                                               */
/* Andreas Rostin                                */
/* 27.10.00                                      */
/*************************************************/
#include <qdialog.h>
#include <qwidget.h>
#include <qscrollbar.h>
#include <qpushbutton.h>
#include <QGroupBox>
#include <QMatrix>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <KlogicGlobal.h>

#include <dlgPattern.h>
#include "dlgPattern.moc"

DlgPattern::DlgPattern(QWidget *parent, QString caption, QString sEquation)
	: QDialog(parent, Qt::Dialog)
{
	setModal(true);
	setWindowTitle(caption);
	dframe = new QWidget(this);

	diagram = new KarnaughWidget(dframe, sEquation);
	diagram->setGeometry(0, 0, 400, 400);

	diagram->setBackgroundRole(QPalette::Base);
	QPalette palette(diagram->palette());
	palette.setColor(QPalette::Background, Qt::white);
	diagram->setPalette(palette);
	diagram->setAutoFillBackground(true);

	bgNType = new QGroupBox(this);
	bgNType->setGeometry(10, 410, 110, 50);
	rbSubjunctive = new QRadioButton(bgNType);
	rbSubjunctive->setGeometry(5, 5, 100, 20);
	rbSubjunctive->setText(i18n("subjunctive"));
	rbSubjunctive->setChecked(TRUE);
	rbDisjunctive = new QRadioButton(bgNType);
	rbDisjunctive->setGeometry(5, 25, 100, 20);
	rbDisjunctive->setText(i18n("disjunctive"));

	bgNSecurity = new QGroupBox(this);
	bgNSecurity->setGeometry(140, 410, 110, 50);
	rbSecure = new QRadioButton(bgNSecurity);
	rbSecure->setGeometry(5, 5, 100, 20);
	rbSecure->setText(i18n("secure"));
	rbSecure->setChecked(TRUE);
	rbInsecure = new QRadioButton(bgNSecurity);
	rbInsecure->setGeometry(5, 25, 100, 20);
	rbInsecure->setText(i18n("insecure"));

	bNormalize = new QPushButton(this);
	bNormalize->setText(i18n("Refresh"));
	bNormalize->setGeometry(260, 420, 60, 25);

	bOK = new QPushButton(this);
	bOK->setDefault(true);
	bOK->setText(i18n("OK"));

	connect(bOK, SIGNAL(clicked()), SLOT(accept()));
	connect(bNormalize, SIGNAL(clicked()), SLOT(refresh()));

	// the size depends on the number of inputs of the equation
	if (diagram->getInputCounter() <= 4) {
		setFixedSize(410, 465);
		dframe->setGeometry(5, 5, 400, 400);
		diagram->setGeometry(0, 0, 400, 400);
		bOK->setGeometry(360, 420, 40, 25);
	} else if (diagram->getInputCounter() == 5) {
		setFixedSize(710, 465);
		diagram->setGeometry(0, 0, 700, 400);
		dframe->setGeometry(5, 5, 700, 400);
		bOK->setGeometry(660, 420, 40, 25);
	} else if (diagram->getInputCounter() == 6) {
		setFixedSize(710, 460);
		diagram->setGeometry(0, 0, 700, 700);
		dframe->setGeometry(5, 5, 700, 400);
		//vscroll = new QScrollBar(0, diagram->height() - dframe->height(), 1, 80, 0, QScrollBar::Vertical, dframe);
		vscroll = new QScrollBar(Qt::Vertical, dframe);
		vscroll->setGeometry(dframe->width() - 10, 0, 10, dframe->height());
		connect(vscroll, SIGNAL(valueChanged(int)), SLOT(vScroll(int)));
		vScroll(0);
		vscroll->show();
		bOK->setGeometry(660, 420, 40, 25);
	} else {
		setFixedSize(410, 465);
		dframe->setGeometry(5, 5, 400, 400);
		diagram->setGeometry(0, 0, 400, 400);
		bOK->setGeometry(360, 420, 40, 25);
	}
}

// OK button pressed
void DlgPattern::done(int r)
{
	if (r == Accepted) {
	}
	QDialog::done(r);
}

// scroll vertically
void DlgPattern::vScroll(int newval)
{
	diagram->move(diagram->x(), -1 * newval);
}

// calculate and display again with changed parameters
void DlgPattern::refresh() {
	int security, type;

	if (rbSecure->isChecked()) security = 1;
	else security = 0;
	if (rbDisjunctive->isChecked()) type = 0;
	else type = 1;
	diagram->calculateNew(type, security);
}

KarnaughWidget::KarnaughWidget(QWidget *parent, QString sEquation)
	: QWidget(parent)
{
	m_oSymCalc.setEquation(sEquation);
	m_oSymCalc.parse();
	m_oSymCalc.calculateSymbolicNormalized(1,0);
	m_iSize = m_oSymCalc.getInputNames().size();
}

int KarnaughWidget::getInputCounter()
{
	return m_iSize;
}

void KarnaughWidget::calculateNew(int type, int secure)
{
	m_oSymCalc.calculateSymbolicNormalized(type, secure);
	m_iSize = m_oSymCalc.getInputNames().size();
	repaint();
}

void KarnaughWidget::paintEvent(QPaintEvent *)
{	QPainter p;

	if (!drawTable()) {
		p.begin(this);
		p.setPen(Qt::black);
		p.setFont( QFont( "helvetica", 10, QFont::Bold ) );
		p.drawText(50, 200, i18n("unable to display Karnaugh with more than 6 input variables ...\n"));
		p.end();
		return;
	}
	drawPattern();
	drawGroups();
}

bool KarnaughWidget::drawTable()
{	QPainter p;
	QMatrix m_rot;
	m_rot.rotate(-90.0);

	const vector<QString>& vInputNames =  m_oSymCalc.getInputNames();
	switch (m_iSize) {
		case 1:
			p.begin(this);
			p.setPen(Qt::black);
			p.setFont( QFont( "helvetica", 10, QFont::Bold ) );
			p.drawRect(100,100,100,50);
			p.drawLine(150, 90, 150, 150);
			p.drawText(110, 94, vInputNames[0]);
			p.end();
			break;
		case 2:
			p.begin(this);
			p.setPen(Qt::black);
			p.setFont( QFont( "helvetica", 10, QFont::Bold ) );
			p.drawRect(100,100,100,100);
			p.drawLine(90, 150, 200, 150);
			p.drawLine(150, 90, 150, 200);
			p.drawText(110, 94, vInputNames[0]);
			p.setWorldMatrix(m_rot, TRUE);
			p.drawText(-140, 94, vInputNames[1]);
			p.end();
			break;
		case 3:
			p.begin(this);
			p.setPen(Qt::black);
			p.setFont( QFont( "helvetica", 10, QFont::Bold ) );
			p.drawRect(100,100,200,100);
			p.drawLine(90, 150, 300, 150);

			p.drawLine(150, 100, 150, 210);
			p.drawLine(200, 90, 200, 200);
			p.drawLine(250, 100, 250, 210);

			p.drawText(110, 94, vInputNames[0]);
			p.drawText(160, 210, vInputNames[1]);
			p.setWorldMatrix(m_rot, TRUE);
			p.drawText(-140, 94, vInputNames[2]);
			p.end();
			break;
		case 4:
			drawQuadTable(100, 100);
			break;
		case 5:
			drawQuadTable(100, 100);
			drawQuadTable(400, 100);
			break;
		case 6:
			drawQuadTable(100, 100);
			drawQuadTable(400, 100);
			drawQuadTable(100, 400);
			drawQuadTable(400, 400);
			break;
		default:
			return false;
	}
	return true;
}

void KarnaughWidget::drawQuadTable(int x, int y)
{
	const vector<QString>& vInputNames =  m_oSymCalc.getInputNames();

	QPainter p;
	QMatrix m_rot;
	m_rot.rotate(-90.0);

	p.begin(this);
	p.setPen(Qt::black);
	p.setFont( QFont( "helvetica", 10, QFont::Bold ) );

	p.drawRect(x, y, 200, 200);
	p.drawLine(x, y + 50, x + 210, y + 50);
	p.drawLine(x - 10, y + 100, x + 200, y + 100);
	p.drawLine(x, y + 150, x + 210, y + 150);

	p.drawLine(x + 50, y, x + 50, y + 210);
	p.drawLine(x + 100, y - 10, x + 100, y + 200);
	p.drawLine(x + 150, y, x + 150, y + 210);

	if (x == 400 && y == 100) {
		p.drawLine(350, 20, 350, 380);
		QString s = vInputNames[4];
		s += " = 1";
		p.drawText(180, 20, s);
	}
	if (x == 400 && y == 400) {
		p.drawLine(350, 20, 350, 680);
		p.drawLine(20, 350, 680, 350);
	}

	p.drawText(x + 10, y - 6, vInputNames[0]);		// b Q1 top
	p.drawText(x + 60, y + 210, vInputNames[2]);		// a Q3 bottom

	p.setWorldMatrix(m_rot, TRUE);
	if (x == 400 && y == 400) {
		QString s = vInputNames[5];
		s += " = 1";
		p.drawText(-220, 20, s);
	}
	p.drawText(y * -1 - 90, x - 6, vInputNames[3]);		// c Q4 left
	p.drawText(y * -1 - 140, x + 212, vInputNames[1]);	// d Q2 right

	p.end();
}

// calculate the coordinates for the pattern at index i
void KarnaughWidget::getCoord(const Pattern& oPat, int& x, int& y)
{
	x = 125;
	y = 130;
	if (m_iSize == 1) {
		if (oPat[0] == 0) x+= 50;
	} else if (m_iSize == 2) {
		if (oPat[0] == 0) x+= 50;
		if (oPat[1] == 0) y+= 50;
	} else if (m_iSize == 3) {
		if (oPat[0] == 1 && oPat[1] == 1) x+= 50;
		if (oPat[0] == 0 && oPat[1] == 1) x+= 100;
		if (oPat[0] == 0 && oPat[1] == 0) x+= 150;
		if (oPat[2] == 0) y+= 50;
	} else if (m_iSize >= 4) {
		if (oPat[3] == 1 && oPat[1] == 1) y += 50;
		if (oPat[3] == 0 && oPat[1] == 1) y += 100;
		if (oPat[3] == 0 && oPat[1] == 0) y += 150;

		if (oPat[0] == 1 && oPat[2] == 1) x += 50;
		if (oPat[0] == 0 && oPat[2] == 1) x += 100;
		if (oPat[0] == 0 && oPat[2] == 0) x += 150;

		if (m_iSize >= 5 && oPat[4] == 0) x += 300;
		if (m_iSize >= 6 && oPat[5] == 0) y += 300;
	}
}

void KarnaughWidget::drawPattern()
{
	QString s;

	QPainter p;
	p.begin(this);
	p.setPen(Qt::black);
	p.setFont( QFont( "helvetica", 10, QFont::Bold ) );

	const Patterns& oPatterns = m_oSymCalc.getPatterns();

	int x, y;
	for(unsigned int i = 0; i < oPatterns.size(); i++) {
		getCoord(oPatterns[i], x, y);
		s.sprintf("%d", oPatterns[i].iResult);
		p.drawText(x, y, s);
	}
	p.end();
}

void KarnaughWidget::drawGroups()
{
	int x, y;
	int maxhier = 100;
	int* minx = new int[100];
	int* miny = new int[100];
	int* maxx = new int[100];
	int* maxy = new int[100];
	int* aInit = new int[100];


	QColor oCols[] = {Qt::darkMagenta, Qt::red, Qt::green, Qt::blue,
			   Qt::cyan, Qt::magenta, Qt::gray, Qt::darkYellow,
			   Qt::darkGreen, Qt::darkRed };

	for (int i = 0; i < maxhier; i++)
		aInit[i] = -1;

	const Groups& groups = m_oSymCalc.getGroups();
	Groups::const_iterator iter = groups.begin();
	const Patterns& oPatterns = m_oSymCalc.getPatterns();

	QPainter p;
	p.begin(this);
	int i = 0;
	while(iter != groups.end()) {
		const Group& oGrp = iter->second;

		int iHierCount = 0;
		// change the color for each group
		if (i % 10 < 10)
			p.setPen(oCols[i % 10]);

		// determine coordinates of the rectangle
		memcpy(minx, aInit, sizeof(aInit));
		memcpy(maxx, aInit, sizeof(aInit));
		memcpy(miny, aInit, sizeof(aInit));
		memcpy(maxy, aInit, sizeof(aInit));

		Group::const_iterator giter = oGrp.begin();
		while (giter != oGrp.end()) {
			getCoord(oPatterns[*giter], x, y);

			// try find a matching hierarchy
			bool bHierFound = false;
			int iHier = 0;
			while (!bHierFound && iHier < iHierCount) {
				if ((abs(minx[iHier] - x) <= 50 ||
				     abs(maxx[iHier] - x) <= 50) &&
				    (abs(miny[iHier] - y) <= 50 ||
				     abs(maxy[iHier] - y) <= 50)) {
					if (x < minx[iHier]) minx[iHier] = x;
					if (x > maxx[iHier]) maxx[iHier] = x;
					if (y < miny[iHier]) miny[iHier] = y;
					if (y > maxy[iHier]) maxy[iHier] = y;
					bHierFound = true;
				}
				iHier++;
			}

			// new hierarchy
			if (!bHierFound) {
				iHierCount++;
				iHier = iHierCount - 1;
				minx[iHier] = x;
				maxx[iHier] = x;
				miny[iHier] = y;
				maxy[iHier] = y;
			}
			giter++;
		}

		// draw the rectangle(s)
		for(int j = 0; j < iHierCount; j++) {
			// try to (visually) join the groups again before painting them
			int iHier = j + 1;
			while(iHier < iHierCount) {
				if ((abs(minx[iHier] - minx[j]) <= 50 ||
				     abs(maxx[iHier] - maxx[j]) <= 50) &&
				    (abs(miny[iHier] - miny[j]) <= 50 ||
				     abs(maxy[iHier] - maxy[j]) <= 50)) {
					if (minx[iHier] < minx[j]) minx[j] = minx[iHier];
					if (maxx[iHier] > maxx[j]) maxx[j] = maxx[iHier];
					if (miny[iHier] < miny[j]) miny[j] = miny[iHier];
					if (maxy[iHier] > maxy[j]) maxy[j] = maxy[iHier];
					iHierCount--;
					for(int k = iHier; k < iHierCount; k++) {
						minx[k] = minx[k + 1];
						maxx[k] = maxx[k + 1];
						miny[k] = miny[k + 1];
						maxy[k] = maxy[k + 1];
					}
					iHier = j + 1;
				} else iHier++;
			}
			// set corner radius
			int xrad = 30, yrad = 30;
			if (maxx[j] - minx[j] < maxy[j] - miny[j]) {
				xrad = 50;
				yrad = 25;
			} else if (maxx[j] - minx[j] > maxy[j] - miny[j]) {
				xrad = 25;
				yrad = 50;
			}

			// draw the rectangle
			p.drawRoundRect(minx[j] - 10,
					miny[j] - 20,
					maxx[j] - minx[j] + 25,
					maxy[j] - miny[j] + 30,
					xrad, yrad);
		}
		iter++;
		i++;
	}
	p.end();

	delete[] minx;
	delete[] miny;
	delete[] maxx;
	delete[] maxy;
	delete[] aInit;
}
