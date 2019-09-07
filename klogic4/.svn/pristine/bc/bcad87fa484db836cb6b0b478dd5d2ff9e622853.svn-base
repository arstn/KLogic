/*************************************************/
/* methods for class DlgEqu                      */
/*                                               */
/* dialog to modify device or circuit equations  */
/* if editing circuit equations, new equation    */
/* can be created with this dialog               */
/*                                               */
/* Andreas Rostin                                */
/* 15.01.99                                      */
/*                                               */
/* replaced the PropEqu dialog using this one:   */
/* 06.10.00                                      */
/*************************************************/
#include <klineedit.h>

#include <qnamespace.h>
#include <qdialog.h>
#include <QGroupBox>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <QListWidget>

#include "klogic4.h"
#include "KlogicGlobal.h"
#include "CircuitWidget.h"
#include "circuit.h"
#include "dlgEqu.h"
#include "dlgPattern.h"
#include "symbolicCalc.h"
#include "DeviceType.h"

#include "dlgEqu.moc"

// -----------------------------------------
// static const members
// -----------------------------------------
const QString DlgEqu::TYPE_INTERN = "(intern)";
const QString DlgEqu::TYPE_FINAL = "(final)";
const QString DlgEqu::TYPE_EQUALS = " = ";
const QString DlgEqu::TYPE_NONE = "";

// -----------------------------------------
// circuit equation editing / device creator
// -----------------------------------------
DlgEqu::DlgEqu(QWidget *parent, QString _caption, Circuit *_net)
	: QDialog(parent, Qt::Dialog)
{
    setModal(true);
    setWindowTitle(_caption);
    net = _net;
    dev = (XDevice *)NULL;

    equations = (KlogicList<OutputInfo> *)NULL;
    deleted_equations = (KlogicList<OutputInfo> *)NULL;
    substituted = 0;
    edit_slot = 0;
    edit_key = "";

    createDialogContent(_caption);
    parseCircuit();
}

// -----------------------------------------
// device equation editing
// -----------------------------------------
DlgEqu::DlgEqu(QWidget *parent, QString _caption, XDevice *_dev)
	: QDialog(parent, Qt::Dialog)
{
    net = (Circuit *)NULL;
    dev = _dev;

    equations = (KlogicList<OutputInfo> *)NULL;
    deleted_equations = (KlogicList<OutputInfo> *)NULL;
    substituted = 0;
    edit_slot = 0;
    edit_key = "";

    createDialogContent(_caption);
    parseCircuit();
}

void DlgEqu::createDialogContent(QString _caption)
{

//------------------------------------------------------------------
// a list box containing the equation list

    list_equ_label = new QLabel(this);
    list_equ_label->setGeometry(10, 10, 100, 20);
    if (net) list_equ_label->setText(i18n("Circuit Equations"));
    if (dev) list_equ_label->setText(i18n("Device Equations"));

    list_equ = new QListWidget(this);
    list_equ->setGeometry(10, 30, 590, 240);

//------------------------------------------------------------------
// elements for manipulating a single equation

    edit_output_name = new KLineEdit(this);
    edit_output_name->setGeometry(10, 285, 120, 20);

    edit_equation_label = new QLabel(this);
    edit_equation_label->setGeometry(136, 282, 10, 20);
    edit_equation_label->setText("=");

    edit_equation = new KLineEdit(this);
    edit_equation->setGeometry(150, 285, 450, 20);

    bgType = new QGroupBox(this);
    bgType->setGeometry(18, 318, 105, 70);
    rbFinal = new QRadioButton(bgType);
    rbFinal->setGeometry(5, 5, 90, 20);
    rbFinal->setText(i18n("final"));
    rbFinal->setChecked(true);
    rbInternal = new QRadioButton(bgType);
    rbInternal->setGeometry(5, 25, 90, 20);
    rbInternal->setText(i18n("internal"));
    rbTemp = new QRadioButton(bgType);
    rbTemp->setGeometry(5, 45, 90, 20);
    rbTemp->setText(i18n("temporary"));

    bEdit = new QPushButton((QWidget *)this);
    bEdit->setGeometry(140, 335, 55, 25);
    bEdit->setText(i18n("Change"));

    bNew = new QPushButton((QWidget *)this);
    bNew->setGeometry(205, 335, 45, 25);
    bNew->setText(i18n("Add"));
    bNew->setEnabled(TRUE);

    bDelete = new QPushButton((QWidget *)this);
    bDelete->setGeometry(260, 335, 55, 25);
    bDelete->setText(i18n("Delete"));
    bDelete->setEnabled(TRUE);

    bgNormalize_s = new QGroupBox(this);
    bgNormalize_s->setGeometry(325, 315, 110, 75);
    rbSubjunctive_s = new QRadioButton(bgNormalize_s);
    rbSubjunctive_s->setGeometry(5, 5, 100, 20);
    rbSubjunctive_s->setText(i18n("subjunctive"));
    rbSubjunctive_s->setChecked(TRUE);
    rbDisjunctive_s = new QRadioButton(bgNormalize_s);
    rbDisjunctive_s->setGeometry(5, 25, 100, 20);
    rbDisjunctive_s->setText(i18n("disjunctive"));
    bNormalize_s = new QPushButton(bgNormalize_s);
    bNormalize_s->setGeometry(5, 50, 100, 20);
    bNormalize_s->setText(i18n("Normalize"));

    bShowKarnaugh = new QPushButton(this);
    bShowKarnaugh->setGeometry(445, 335, 95, 25);
    bShowKarnaugh->setText(i18n("show Karnaugh"));

    bExtend = new QPushButton((QWidget *)this);
    bExtend->setGeometry(570, 335, 30, 25);
    if (net) {
    	dlg_extended = 0;	// will be switched!
    } else {
    	dlg_extended = 1;	// will be switched!
    }

//------------------------------------------------------------------
// elements for manipulating all equations

    global_label = new QLabel((QWidget *)this);
    global_label->setGeometry(610, 10, 110, 20);
    global_label->setText(i18n("List Methods"));

    bSubst = new QPushButton((QWidget *)this);
    bSubst->setGeometry(610, 30, 110, 20);
    bSubst->setText(i18n("Substitute"));

    bCollect = new QPushButton((QWidget *)this);
    bCollect->setGeometry(610, 66, 110, 20);
    bCollect->setText(i18n("Tidy Up"));
    bCollect->setEnabled(FALSE);

    bgReplace = new QGroupBox((QWidget *)this);
    bgReplace->setGeometry(610, 100, 110, 80);
    edit_replace = new KLineEdit((QWidget *)bgReplace);
    edit_replace->setGeometry(5, 5, 100, 20);
    edit_replace_with = new KLineEdit((QWidget *)bgReplace);
    edit_replace_with->setGeometry(5, 30, 100, 20);
    bReplace = new QPushButton((QWidget *)bgReplace);
    bReplace->setGeometry(5, 55, 100, 20);
    bReplace->setText(i18n("replace - with"));

    bgNormalize = new QGroupBox(this);
    bgNormalize->setGeometry(610, 195, 110, 75);
    rbSubjunctive = new QRadioButton(bgNormalize);
    rbSubjunctive->setGeometry(5, 5, 100, 20);
    rbSubjunctive->setText(i18n("subjunctive"));
    rbSubjunctive->setChecked(TRUE);
    rbDisjunctive = new QRadioButton(bgNormalize);
    rbDisjunctive->setGeometry(5, 25, 100, 20);
    rbDisjunctive->setText(i18n("disjunctive"));
    bNormalize = new QPushButton(bgNormalize);
    bNormalize->setGeometry(5, 50, 100, 20);
    bNormalize->setText(i18n("Normalize"));
    bNormalize->setEnabled(FALSE);

    bRestore = new QPushButton(this);
    bRestore->setGeometry(610, 285, 110, 20);
    bRestore->setText(i18n("Restore All"));

//------------------------------------------------------------------
// common elements

    sep1 = new QGroupBox(this);
    sep1->setGeometry(10, 394, 710, 2);

    bOK = new QPushButton(this);
    bOK->setGeometry(260, 402, 120, 25);
    if (net)
    	bOK->setText(i18n("Create sub circuit"));
    if (dev)
    	bOK->setText(i18n("OK"));

    bCancel = new QPushButton(this);
    bCancel->setGeometry(400, 402, 60, 25);
    bCancel->setText(i18n("Cancel"));

    sep2 = new QGroupBox(this);
    sep2->setGeometry(10, 433, 710, 2);

    setWindowTitle(_caption);
    extendDialog();	// depends on wether net or dev is set!

//------------------------------------------------------------------
// signal processing

    connect(list_equ, SIGNAL(currentRowChanged (int)), SLOT(editEquation(int)));
    connect(bEdit, SIGNAL(clicked()), SLOT(editEquationOK()));
    connect(bNew, SIGNAL(clicked()), SLOT(newEquation()));
    connect(bDelete, SIGNAL(clicked()), SLOT(deleteEquation()));
    connect(bNormalize_s, SIGNAL(clicked()), SLOT(normalizeEquation()));
    connect(bRestore, SIGNAL(clicked()), SLOT(parseCircuit()));
    connect(bSubst, SIGNAL(clicked()), SLOT(substituteEquations()));
    connect(bCollect, SIGNAL(clicked()), SLOT(collectEquations()));
    connect(bNormalize, SIGNAL(clicked()), SLOT(normalizeEquations()));
    connect(bReplace, SIGNAL(clicked()), SLOT(replaceString()));
    connect(bExtend, SIGNAL(clicked()), SLOT(extendDialog()));
    connect(bShowKarnaugh, SIGNAL(clicked()), SLOT(showKarnaugh()));

    connect(bOK, SIGNAL(clicked()), SLOT(accept()));
    connect(bCancel, SIGNAL(clicked()), SLOT(reject()));

}

DlgEqu::~DlgEqu()
{
	freemem();
}

void DlgEqu::done(int r)
{
	if (r == Accepted) {
		KlogicList<OutputInfo> *lout;
		int i;
		QString input;
		int io_id;
		int res;

		if (net) {
			// create a new subcircuit
			// substitite if possible
			while(0 < (res = substituteEquations())) {};
			if (res == -1) {
				res = QMessageBox::warning(this,
					i18n("Create sub circuit"),
					i18n("Creating devices without substitution\nmay result in unuseable devices\n\ncontinue anyway?"),
					QMessageBox::Yes, QMessageBox::No);
					if (res == QMessageBox::No) QDialog::done(r);
			}
			collectEquations();

			// create the equation device
			dev = net->createDevice(DeviceType::fEQU, QPoint(20, 20));

			// create all outputs and set the equations
			lout = equations->First();
			while(lout) {
				if (lout->Get()->output_type == KlogicGlobal::Device::INTERNAL_OUTPUT) {
					io_id = dev->addInternalName(lout->Get()->output_name);
					dev->Device::setEquation(lout->Get()->equation, io_id);
				}
				if (lout->Get()->output_type == KlogicGlobal::Device::FINAL_OUTPUT) {
					dev->setSize(dev->neededSize() + 1);
					io_id = dev->addOutputName(lout->Get()->output_name);
					dev->Device::setEquation(lout->Get()->equation, io_id);
					dev->setSize(dev->neededSize());
				}
				lout = lout->Next();
			}

			// create all input vars
			lout = equations->First();
			while(lout) {
				// create a list of all available symbols
				SymbolicCalc oop;
				oop.setEquation(lout->Get()->equation);
				oop.parse();

				i = 0;
				while(NULL != (input = oop.getSymbol(i++))) {
					// if the current symbol is not an output, create a new input
					if (!equations->With(input) && dev->inputNameIsUnique(input)) {
						// names are currently unique, so look for an existing input with this name
						dev->setSize(dev->neededSize() + 1);
						dev->addInputName(input);
						dev->setSize(dev->neededSize());
					}
				}
				lout = lout->Next();
			}
		} else if (dev) {
			int output_id;

			// delete deleted equations
			lout = deleted_equations->First();
			while(lout) {
				output_id = lout->Get()->output_id;
				dev->removeOutputName(output_id);
				lout = lout->Next();
			}

			dev->setAllEquations(equations);
		}

		// prepare device operation
		dev->parseEquation();
	}
	QDialog::done(r);
}

void DlgEqu::extendDialog()
{
	if (dlg_extended) {
		setFixedSize(610, 440);
		dlg_extended = 0;
		bExtend->setText(">>");
		sep1->setGeometry(10, 394, 590, 2);
		bOK->setGeometry(200, 402, 120, 25);
		bCancel->setGeometry(340, 402, 60, 25);
		sep2->setGeometry(10, 433, 590, 2);
	} else {
		setFixedSize(730, 440);
		dlg_extended = 1;
		bExtend->setText("<<");
		sep1->setGeometry(10, 394, 710, 2);
		bOK->setGeometry(260, 402, 120, 25);
		bCancel->setGeometry(400, 402, 60, 25);
		sep2->setGeometry(10, 433, 710, 2);
	}
}

// free memory allocated by equation parsing (in class Device)
void DlgEqu::freemem()
{	KlogicList<OutputInfo> *ls;

	if (equations) {
		list_equ->clear();
		ls = equations->First();
		while(ls) {
			delete ls->Get();
			ls = ls->Next();
		}
		equations->Destroy();
		delete equations;
		equations = (KlogicList<OutputInfo> *)NULL;
	}

	if (deleted_equations) {
		ls = deleted_equations->First();
		while(ls) {
			delete ls->Get();
			ls = ls->Next();
		}
		deleted_equations->Destroy();
		delete deleted_equations;
		deleted_equations = (KlogicList<OutputInfo> *)NULL;
	}
}

// list box item highlighted
void DlgEqu::editEquation(int selectedIndex)
{
	edit_slot = selectedIndex;
	
	// get the output name for which equation should be edited
	QString eq = "";
	if (edit_slot >= 0) {
		qDebug() << edit_slot << list_equ->item(edit_slot) << list_equ->item(edit_slot)->text();
		eq = list_equ->item(edit_slot)->text();
		int idx = eq.indexOf(TYPE_INTERN);
		if (idx >= 0) {
		  edit_key = eq.left(idx);
		} else {
		  idx = eq.indexOf(TYPE_FINAL);
		  if (idx >= 0) {
			edit_key = eq.left(idx);
		  } else {
			idx = eq.indexOf(TYPE_EQUALS);
			if (idx >= 0) {
			  edit_key = eq.left(idx);
			} else {
			  edit_key = "";
			  clearSelection();
			  return;
			}
		  }
		}
	} else {
		clearSelection();
		return;
	}
	
	qDebug() << "editKey is " << edit_key;
	KlogicList<OutputInfo> *ls = equations->With(edit_key);
	if (ls && ls->Get()) {
		// display equation in the edit-box
		edit_equation->setText(ls->Get()->equation);

		// display new edit box label
		if (ls->Get()->output_type == KlogicGlobal::Device::INTERNAL_OUTPUT) {
			rbInternal->setChecked(TRUE);
		} else if (ls->Get()->output_type == KlogicGlobal::Device::FINAL_OUTPUT) {
			rbFinal->setChecked(TRUE);
		} else {
			rbTemp->setChecked(TRUE);
		}
		edit_output_name->setText(ls->Get()->output_name);
	} else {
		clearSelection();
		return;
	}
}

// equation editing finished (change button pressed)
void DlgEqu::editEquationOK()
{	QString newTypeName;
	KlogicGlobal::Device::IOTYPE newType;

	if (edit_key != "") {
		KlogicList<OutputInfo> *ls = equations->With(edit_key);
		if (ls) {
			// set new output type
			if (rbInternal->isChecked()) {
				newTypeName = TYPE_INTERN;
				newType = KlogicGlobal::Device::INTERNAL_OUTPUT;
			} else if (rbFinal->isChecked()) {
				newTypeName = TYPE_FINAL;
				newType = KlogicGlobal::Device::FINAL_OUTPUT;
			} else {
				newTypeName = TYPE_NONE;
				newType = KlogicGlobal::Device::TMP_OUTPUT;
			}

			// change output name
			QString new_name = edit_output_name->text();
			if (ls->Get()->output_name != new_name) {
				if (changeOutputName(ls->Get()->output_name, new_name)) {
					if (new_name.contains(TYPE_INTERN) || new_name.contains(TYPE_FINAL)) {
						QString msg = i18n("Output names cannot contain\n");
						msg += TYPE_FINAL;
						msg += i18n(" or ");
						msg += TYPE_INTERN;
						QMessageBox::warning((QWidget *)this, i18n("Reserved content"), msg);
						return;
					}

					ls->Get()->output_name = new_name;
					ls->setText(new_name);
					//fillList();
					//return;
				} else {
					QMessageBox::information(this, i18n("Change output name"),
						i18n("Output name already exists"));
						return;
				}
			}

			ls->setID2(newType);
			ls->Get()->output_type = newType;

			// set new equation
			ls->Get()->equation = edit_equation->text();

			QString s = ls->Get()->output_name;
			s += newTypeName;
			s += " = ";
			s += ls->Get()->equation;
			QListWidgetItem *item = list_equ->item(edit_slot);
			item->setText(s);
		}
	}
}

// normalize the currently selected equations
void DlgEqu::normalizeEquation()
{
	if (!edit_equation->text().length()) return;

	SymbolicCalc opp;
	// build result table
	opp.setEquation(edit_equation->text());
	opp.parse();
	if (rbSubjunctive_s->isChecked()) normalize_status = 1;
	else normalize_status = 0;
	edit_equation->setText(opp.calculateSymbolicNormalized(normalize_status, 0));
}

void DlgEqu::showKarnaugh()
{
	if (!edit_equation->text().length()) return;

	DlgPattern *dlg = new DlgPattern(this, "Karnaugh Diagram", edit_equation->text());
	dlg->exec();
	delete dlg;
}

// change an existing output name
int DlgEqu::changeOutputName(QString old_name, QString new_name)
{	int start, len;

	KlogicList<OutputInfo> *ls = equations->First();
	while(ls) {
		if (ls->Get()->output_name == new_name) return 0;
		ls = ls->Next();
	}

	len = old_name.length();
	ls = equations->First();
	while(ls) {
		while (-1 != (start = ls->Get()->equation.indexOf(old_name)))
			ls->Get()->equation.replace(start, len, new_name);
		ls = ls->Next();
	}
	return 1;
}

// replace button pressed: replace string in all equations
void DlgEqu::replaceString()
{	QString s_old, s_new;
	int start, len, new_len;

	s_old = edit_replace->text();
	s_new = edit_replace_with->text();

	len = s_old.length();
	new_len = s_new.length();
	if (!len) return;

	KlogicList<OutputInfo> *ls = equations->First();
	bool bFound = false;
	while(ls) {
		start = 0;
		while (-1 != (start = ls->Get()->equation.indexOf(s_old, start))) {
			ls->Get()->equation.replace(start, len, s_new);
			start += new_len;
			bFound = true;
		}
		start = 0;
		while (-1 != (start = ls->Get()->output_name.indexOf(s_old, start))) {
			ls->Get()->output_name.replace(start, len, s_new);
			start += new_len;
			bFound = true;
		}
		start = 0;
		while (-1 != (start = ls->Get()->prefix.indexOf(s_old, start))) {
			ls->Get()->prefix.replace(start, len, s_new);
			start += new_len;
			bFound = true;
		}

		bool bKeyFound = false;
		QString s;
		if (edit_key != "") {
			s = edit_key;
			start = 0;
			while (-1 != (start = s.indexOf(s_old, start))) {
				s.replace(start, len, s_new);
				start += new_len;
				bKeyFound = true;
			}
			if (bKeyFound) {
				edit_key = s;
				bFound = true;
			}
		}
		bKeyFound = false;
		s = ls->getText();
		start = 0;
		while (-1 != (start = s.indexOf(s_old, start))) {
			s.replace(start, len, s_new);
			start += new_len;
			bKeyFound = true;
		}
		if (bKeyFound) ls->setText(s);

		ls = ls->Next();
	}
	if (bFound) fillList();
}

void DlgEqu::clearSelection()
{
	list_equ->clearSelection();
	edit_slot = 0;
	edit_key = "";
	edit_equation->setText("");
	edit_output_name->setText("");
}

void DlgEqu::editEquationCancel()
{
	if (list_equ->currentIndex().row() >= 0) {
		clearSelection();
	} else {
		editEquation(list_equ->currentIndex().row());
	}
}

void DlgEqu::newEquation()
{
	if (edit_equation->text().size() && edit_output_name->text().size()) {
		// the new name must be unique!
		if (equations->With(edit_output_name->text())) {
			QMessageBox::warning((QWidget *)this, i18n("Output name already exists"), i18n("Output names of equation devices must be unique"));
			return;
		}
		if (edit_output_name->text().contains(TYPE_INTERN) || edit_output_name->text().contains(TYPE_FINAL)) {
			QString msg = i18n("Output names cannot contain\n");
			msg += TYPE_FINAL;
			msg += " or ";
			msg += TYPE_INTERN;
			QMessageBox::warning((QWidget *)this, i18n("Reserved content"), msg);
			return;
		}

		// retrieve the equation string
		OutputInfo *oi = new OutputInfo();
		oi->equation = edit_equation->text();
		oi->output_name = edit_output_name->text();

		// set output type
		if (rbInternal->isChecked()) {
			oi->output_type = KlogicGlobal::Device::INTERNAL_OUTPUT;
		} else if (rbFinal->isChecked()) {
			oi->output_type = KlogicGlobal::Device::FINAL_OUTPUT;
		} else {
			oi->output_type = KlogicGlobal::Device::TMP_OUTPUT;
		}

		// append the new "output<-equation" to the list
		KlogicList<OutputInfo> *ls = equations->Append(oi);
		ls->setText(oi->output_name);
		ls->setID2(oi->output_type);

		// update the displayed list
		fillList();

		// display new item highlighed
		int idx = list_equ->count() - 1;
		list_equ->setCurrentRow(idx);
		editEquation(idx);
	}
}

void DlgEqu::deleteEquation()
{
	if (edit_key != "") {
		KlogicList<OutputInfo> *ls = equations->With(edit_key);
		if (ls) {
			// move output info to deleted list
			OutputInfo *oi = ls->Get();
			deleted_equations->Append(oi)->setText(edit_key);
			ls->Destroy(oi);
			
			fillList();
			editEquationCancel();
		}
	}
}

// retrieve the list of equations; either from the Circuit or from the XDevice
void DlgEqu::parseCircuit()
{
	freemem();

	deleted_equations = new KlogicList<OutputInfo>;

	// optimize ! ! !
	if (net) {
		equations = new KlogicList<OutputInfo>;
		map<QString, OutputInfo> oEquations;
		net->getAllEquations(oEquations, "", false);
		for (map<QString,OutputInfo>::iterator iter = oEquations.begin(); iter != oEquations.end(); iter++) {
			OutputInfo *oi = new OutputInfo(iter->second);
			
			equations->Append(oi)->setText(iter->first);
		}
	}

	if (dev) {
		equations = new KlogicList<OutputInfo>;
		map<QString,OutputInfo> oEquations;
		dev->getAllEquations(oEquations, "", true);
		for (map<QString,OutputInfo>::iterator iter = oEquations.begin(); iter != oEquations.end(); iter++) {
			OutputInfo *oi = new OutputInfo(iter->second);
			equations->Append(oi)->setText(iter->first);
		}
	}

	fillList();

	bSubst->setEnabled(TRUE);
	bNormalize->setEnabled(FALSE);
	bCollect->setEnabled(FALSE);
	editEquationCancel();
}

// normalize all equations
void DlgEqu::normalizeEquations()
{	KlogicList<OutputInfo> *ls = equations->First();
	SymbolicCalc opp;

	// build result table
	while(ls) {
		opp.setEquation(ls->Get()->equation);
		opp.parse();
		if (rbSubjunctive->isChecked()) normalize_status = 1;
		else normalize_status = 0;
		ls->Get()->equation = opp.calculateSymbolicNormalized(normalize_status, 0);
		ls = ls->Next();
	}
	fillList();

	editEquationCancel();
}

// leave only internal and final equations left in the list
void DlgEqu::collectEquations()
{	KlogicList<OutputInfo> *ls1 = equations->First();

	while(ls1) {
		if (ls1->Get()->output_type != KlogicGlobal::Device::INTERNAL_OUTPUT && ls1->Get()->output_type != KlogicGlobal::Device::FINAL_OUTPUT) {
			OutputInfo *oi = ls1->Get();
			ls1->Destroy(oi);
			delete oi;
			ls1 = equations->First();
		} else ls1 = ls1->Next();
	}

	fillList();

	bSubst->setEnabled(FALSE);
	bNormalize->setEnabled(FALSE);
	bCollect->setEnabled(FALSE);
	editEquationCancel();

}

int DlgEqu::substituteEquations()
{	KlogicList<OutputInfo> *ls1;				// all equations
	KlogicList<OutputInfo> *ls2;				// equation to substitute with
	KlogicList<QString> substituted;			// list of substituted non-terminal
	QString dummy;					// dummy entry for substituted - list
	int changed;					// circuit equations changed
	int iplane;
	int plane = KlogicGlobal::Device::TMP_OUTPUT;

	// security question
	if (equations->counter() > 50) {
		if (QMessageBox::Yes != QMessageBox::warning(this, i18n("Substitute Equations"),
			i18n("This may take a long time ...\nTry a lower circuit level!\ncontinue anyway"),
			QMessageBox::Yes, QMessageBox::No)) return -1;
	}

	changed = 0;
	for (iplane = 0; iplane < 3; iplane++) {
		if (iplane == 0) plane = KlogicGlobal::Device::TMP_OUTPUT;
		if (iplane == 2) plane = KlogicGlobal::Device::INTERNAL_OUTPUT;
		if (iplane == 1) plane = KlogicGlobal::Device::FINAL_OUTPUT;
		ls1 = equations->First();
		while(ls1) {
			if (plane == ls1->Get()->output_type) {
				// initialize list of already substituted non-terminal
				substituted.Destroy();

				bool bFound;
				do {
					bFound = false;

					// create a calculation queue with all names of the current equation in it
					SymbolicCalc oSymCalc;
					oSymCalc.setEquation(ls1->Get()->equation);
					oSymCalc.parse();
					CalcRules& oCalcRules = oSymCalc.getCalcRules();

					// search equation for each name, then substitute it
					// (the name could also be an operator: ignored)
					for (int i = 0; i < oCalcRules.size(); i++) {
						if (Operator::isOP(oCalcRules[i].sOperator))
							continue;

						// substitute name with its equation
						if (NULL != (ls2 = equations->With(oCalcRules[i].inputName))) {
							int iOutputType = 0;
							// already marked as internal output?
							if (ls2->Get()->output_type == KlogicGlobal::Device::INTERNAL_OUTPUT) {
								iOutputType = KlogicGlobal::Device::INTERNAL_OUTPUT;
							}

							// ls1 and ls2 are the same equation?
							// already subsituted? there is a loop, mark as internal output
							if (iOutputType != KlogicGlobal::Device::INTERNAL_OUTPUT && (ls1 == ls2 || substituted.With(oCalcRules[i].inputName))) {
								ls2->Get()->output_type = KlogicGlobal::Device::INTERNAL_OUTPUT;
								iOutputType = KlogicGlobal::Device::INTERNAL_OUTPUT;
							}

							// substitute..
							if (iOutputType != KlogicGlobal::Device::INTERNAL_OUTPUT) {
								QString s = ls1->Get()->equation.left(oCalcRules[i].posInEquation);
								s += "(";
								s += ls2->Get()->equation;
								s += ")";
								s += ls1->Get()->equation.right(ls1->Get()->equation.length() - oCalcRules[i].posInEquation - oCalcRules[i].lenInEquation);

								// correct the positions..
								for (int j = 0; j < oCalcRules.size(); j++)
									if (oCalcRules[j].posInEquation > oCalcRules[i].posInEquation)
										oCalcRules[j].posInEquation += s.length() - ls1->Get()->equation.length();

								// replace the equation with the substituted one
								ls1->Get()->equation = s;

								// substitute again
								changed = 1;
								bFound = true;
							}
						}
					}

					// remember all substituted
					for(int i = 0; i < oCalcRules.size(); i++) {
						if (Operator::isOP(oCalcRules[i].sOperator))
							continue;
						if (!substituted.With(oCalcRules[i].inputName))
							substituted.Append(&dummy)->setText(oCalcRules[i].inputName);
					}
				} while(bFound);

				// ls1 is substituted now. optimize the newly generated equation
				SymbolicCalc oOptCalc;
				oOptCalc.setEquation(ls1->Get()->equation);
				oOptCalc.parse();
				ls1->Get()->equation = oOptCalc.calculateSymbolic();
			} // if (plane)

			// retrieve the next equation
			ls1 = ls1->Next();
		} // while equations left
	} // next plane

	fillList();

	if (!changed) {
		bNormalize->setEnabled(TRUE);
		bCollect->setEnabled(TRUE);
	}
	editEquationCancel();
	return changed;
}

void DlgEqu::fillList()
{	KlogicList<OutputInfo> *ls = equations->First();

	list_equ->clear();	// the list box
	QString s;
	while(ls) {
		s = ls->Get()->output_name;
		if (ls->Get()->output_type == KlogicGlobal::Device::INTERNAL_OUTPUT) s += TYPE_INTERN;
		else if (ls->Get()->output_type == KlogicGlobal::Device::FINAL_OUTPUT) s += TYPE_FINAL;
		s += " = ";
		s += ls->Get()->equation;
		if (s.length() > 4096) {
			s.truncate(4096);
			s += " (...)";
		}
		list_equ->addItem(new QListWidgetItem(s));
		ls = ls->Next();
	}
}

