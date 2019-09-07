/*************************************************/
/* methods for class DlgLib                      */
/*                                               */
/* maintain device libraries                     */
/*                                               */
/* Andreas Rostin                                */
/* 31.05.2001                                    */
/*                                               */
/*************************************************/
#include <klineedit.h>

#include <qdialog.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qfile.h>
#include <QGroupBox>

#include "KlogicGlobal.h"
#include "dlgEqu.h"
#include "xdevice.h"
#include "devLib.h"
#include "dlgLib.h"
#include "xmlExport.h"

#include "dlgLib.moc"

// -----------------------------------------
// library meta data dialog
// -----------------------------------------
DlgLibMeta::DlgLibMeta(QWidget *parent, const QString& name, DevLibrary *dev_lib)
	: QDialog(parent, Qt::Dialog)
{
	setModal(true);
	setWindowTitle(name);
	activeLib = dev_lib;

	QLabel *l7 = new QLabel((QWidget *)this);
	l7->setGeometry(15, 10, 80, 20);
	l7->setText(i18n("Author"));
	editAuthor = new KLineEdit((QWidget *)this);
	editAuthor->setGeometry(100, 10, 230, 20);

	QLabel *l9 = new QLabel((QWidget *)this);
	l9->setGeometry(15, 35, 80, 20);
	l9->setText(i18n("Comment"));
	editComment = new KLineEdit((QWidget *)this);
	editComment->setGeometry(100, 35, 230, 20);

	bOK = new QPushButton(this);
	bOK->setGeometry(120, 70, 80, 20);
	bOK->setText(i18n("OK"));
	connect(bOK, SIGNAL(clicked()), SLOT(accept()));
}

void DlgLibMeta::done(int r)
{
	if (r == Accepted) {
		activeLib->setMeta(editAuthor->text(), editComment->text());
	}
	QDialog::done(r);
}

// -----------------------------------------
// library editor
// -----------------------------------------
DlgLib::DlgLib(QWidget *parent, const QString& name, KlogicList<DevLibrary> *lib_list, DevLibrary *dev_lib, XDevice *new_dev)
	: QDialog(parent, Qt::Dialog)
{
	setModal(true);
	setWindowTitle(name);
	libList = lib_list;
	activeLib = dev_lib;
	newDev = new_dev;
	createDialogContent();
}

void DlgLib::createDialogContent()
{
	setFixedSize(340, 440);

	QGroupBox *sep0 = new QGroupBox(this);
	sep0->setGeometry(10, 17, 70, 20);
	sep0->setPalette(QPalette(QColor(180, 200, 180)));
	QLabel *l1 = new QLabel(sep0);
	l1->setGeometry(5, 3, 60, 15);
	l1->setText(i18n("Library"));
	cLibList = new QComboBox((QWidget *)this);
	cLibList->setGeometry(90, 17, 240, 20);
	connect(cLibList, SIGNAL(activated(int)), SLOT(setActiveLib(int)));

	bNewLib = new QPushButton(this);
	bNewLib->setGeometry(10, 40, 100, 25);
	bNewLib->setText(i18n("New"));
	connect(bNewLib, SIGNAL(clicked()), SLOT(newLib()));

	bOpen = new QPushButton(this);
	bOpen->setGeometry(120, 40, 100, 25);
	bOpen->setText(i18n("Open"));
	connect(bOpen, SIGNAL(clicked()), SLOT(openLib()));

	bClose = new QPushButton(this);
	bClose->setGeometry(230, 40, 100, 25);
	bClose->setText(i18n("Close"));
	connect(bClose, SIGNAL(clicked()), SLOT(closeLib()));

	// -------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------

	QGroupBox *sep1 = new QGroupBox(this);
	sep1->setGeometry(10, 97, 140, 20);
	sep1->setPalette(QPalette(QColor(180, 200, 180)));
	labelMode = new QLabel(sep1);
	labelMode->setGeometry(5, 3, 130, 15);
	if (newDev)
		labelMode->setText(i18n("Add Device"));
	else
		labelMode->setText(i18n("Choose a Device"));
	cNames = new QComboBox((QWidget *)this);
	cNames->setGeometry(170, 97, 160, 20);
	connect(cNames, SIGNAL(activated(int)), SLOT(setName(int)));

	QLabel *l5 = new QLabel((QWidget *)this);
	l5->setGeometry(15, 135, 80, 20);
	l5->setText(i18n("Creation"));
	lCreation = new QLabel((QWidget *)this);
	lCreation->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	lCreation->setGeometry(100, 135, 230, 20);

	QLabel *l6 = new QLabel((QWidget *)this);
	l6->setGeometry(15, 160, 80, 20);
	l6->setText(i18n("Last Update"));
	lUpdate = new QLabel((QWidget *)this);
	lUpdate->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	lUpdate->setGeometry(100, 160, 230, 20);

	QLabel *l4 = new QLabel((QWidget *)this);
	l4->setGeometry(15, 185, 80, 20);
	l4->setText(i18n("Name"));
	editName = new KLineEdit((QWidget *)this);
	editName->setGeometry(100, 185, 230, 20);
	bDevAction = new QPushButton((QWidget *)this);
	if (newDev) {
		activeName = newDev->getName();
		editName->setText(activeName);
		bDevAction->setText(i18n("Add"));
	} else {
		bDevAction->setText(i18n("Remove"));
	}

	QLabel *l7 = new QLabel((QWidget *)this);
	l7->setGeometry(15, 210, 80, 20);
	l7->setText(i18n("Author"));
	editAuthor = new KLineEdit((QWidget *)this);
	editAuthor->setGeometry(100, 210, 230, 20);

	QLabel *l9 = new QLabel((QWidget *)this);
	l9->setGeometry(15, 235, 80, 20);
	l9->setText(i18n("Comment"));
	editComment = new KLineEdit((QWidget *)this);
	editComment->setGeometry(100, 235, 230, 20);

	// not implemented yet!
	QLabel *l10 = new QLabel((QWidget *)this);
	l10->setGeometry(15, 260, 120, 15);
	l10->setText(i18n("Image Path"));
	cPixmaps = new QComboBox((QWidget *)this);
	cPixmaps->setGeometry(10, 275, 120, 20);
	connect(cPixmaps, SIGNAL(activated(int)), SLOT(setPixmap(int)));
	cPixmaps->setEnabled(false);
	editPixmap = new KLineEdit((QWidget *)this);
	editPixmap->setGeometry(135, 275, 195, 20);
	editPixmap->setText(i18n("not implemented"));
	editPixmap->setEnabled(false);

	bDevUpdate = new QPushButton((QWidget *)this);
	bDevUpdate->setGeometry(40, 315, 80, 20);
	bDevUpdate->setText(i18n("Update"));
	connect(bDevUpdate, SIGNAL(clicked()), SLOT(devUpdate()));

	// not implemented yet!
	bDevEdit = new QPushButton((QWidget *)this);
	bDevEdit->setGeometry(130, 315, 80, 20);
	bDevEdit->setText(i18n("Edit Device"));
	connect(bDevEdit, SIGNAL(clicked()), SLOT(devEdit()));
	bDevEdit->setEnabled(false);

	bDevAction->setGeometry(220, 315, 80, 20);
	connect(bDevAction, SIGNAL(clicked()), SLOT(devAction()));

/*
	// -------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------
	fLibDevEdit = new QFrame(this);
	fLibDevEdit->setGeometry(340, 10, 300, 390);
	ClientW *w = new ClientW(fLibDevEdit, DeviceType::fEQU);
	w->setGeometry(0, 0, 300, 390);
*/

	// -------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------
	QGroupBox *sep2 = new QGroupBox(this);
	sep2->setGeometry(10, 404, 320, 2);

	bOK = new QPushButton(this);
	bOK->setGeometry(100, 410, 60, 20);
	bOK->setText(i18n("OK"));
	connect(bOK, SIGNAL(clicked()), SLOT(accept()));

	bCancel = new QPushButton(this);
	bCancel->setGeometry(180, 410, 60, 20);
	bCancel->setText(i18n("Cancel"));
	connect(bCancel, SIGNAL(clicked()), SLOT(reject()));

	QGroupBox *sep3 = new QGroupBox(this);
	sep3->setGeometry(10, 434, 320, 2);

	updateLibCombo();
	if (activeLib) {
		bClose->setEnabled(true);
		cNames->setEnabled(true);
		updateNameCombo();

	} else {
		bClose->setEnabled(false);
		cNames->setEnabled(false);
		setName(-1);
	}
}

DlgLib::~DlgLib()
{
}

void DlgLib::done(int r)
{	KlogicList<DevLibrary> *ll = libList->First();

	if (r == Accepted) {
		// ask for saving changes!
		while(ll) {
			if (ll->Get()->libChanged()) {
				if (QMessageBox::information(this, i18n("close library"),
					i18n("save changes in ") + ll->getText() + "?",
					QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
					if (!ll->Get()->saveLib()) {
						QMessageBox::information(this,
							i18n("save library"),
							i18n("error occured during writing"));
					}
				}
			}
			ll = ll->Next();
		}
	} else {
		// reload changed libs!
		while(ll) {
			if (ll->Get()->libChanged()) {
				ll->Get()->closeLib();
				ll->Get()->readLib();
			}
			ll = ll->Next();
		}
	}
	QDialog::done(r);
}

void DlgLib::newLib()
{
	DevLibrary *new_lib = DevLibrary::newLib(this);

	if (new_lib) {
		activeLib = new_lib;
		DlgLibMeta dlg(this, i18n("Library Meta Data"), activeLib);
		dlg.exec();

		// actualize list
		KlogicList<DevLibrary> *ll = libList->With(activeLib->getFileName());
		if (ll) {
			DevLibrary *old_lib = ll->Put(activeLib);
			old_lib->closeLib();
			delete old_lib;
		} else {
			libList->Append(activeLib)->setText(activeLib->getFileName());
		}
		updateLibCombo();

		bClose->setEnabled(true);
		updateNameCombo();
	}
}

void DlgLib::openLib()
{
	// create a new one
	DevLibrary *new_lib = new DevLibrary();
	if (1 != (new_lib->openLib(this))) {
		delete new_lib;
		return;
	}
	activeLib = new_lib;

	// add the new one to the list of libraries
	qDebug() << "looking for lib " << activeLib->getFileName();
	KlogicList<DevLibrary> *ll = libList->With(activeLib->getFileName());
	if (ll) {
		DevLibrary *old_lib = ll->Put(activeLib);
		old_lib->closeLib();
		delete old_lib;
	} else {
		libList->Append(activeLib)->setText(activeLib->getFileName());
	}
	updateLibCombo();

	bClose->setEnabled(true);
	cNames->setEnabled(true);
	updateNameCombo();

}

void DlgLib::closeLib()
{
	if (!activeLib) return;

	// ask for saving changes!
	if (activeLib->libChanged()) {
		if (QMessageBox::information(this, i18n("close library"),
			i18n("save changes in ") + activeLib->getFileName() + "?",
			QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
			if (!activeLib->saveLib()) {
				QMessageBox::information(this,
					i18n("save library"),
					i18n("error occured during writing"));
			}
		}
	}
	activeLib->closeLib();

	// actualize list
	KlogicList<DevLibrary> *ll = libList->With(activeLib->getFileName());
	if (ll) {
		libList->Destroy(ll->Get());
	}

	// remove active lib
	delete activeLib;
	activeLib = (DevLibrary *)NULL;

	updateLibCombo();
}

// slot
// set active library
void DlgLib::setActiveLib(int i)
{
	if (!cLibList->count()) {
		cLibList->setEnabled(false);
		activeLib = (DevLibrary *)NULL;
		updateNameCombo();
		return;
	}
	cLibList->setEnabled(true);

	QString name = cLibList->itemText(i);
	KlogicList<DevLibrary> *ll = libList->With(name);
	if (ll) {
		activeLib = ll->Get();
		updateNameCombo();
	}
}

// slot
// change current device
void DlgLib::setName(int i)
{
	if (newDev) {
		// active name already set!
		editName->setEnabled(true);
		editAuthor->setEnabled(true);
		editComment->setEnabled(true);
		bDevEdit->setEnabled(false);
		bDevUpdate->setEnabled(false);
		bDevAction->setEnabled(true);
		cNames->setEnabled(false);
		return;
	}

	if (!activeLib || i == -1) {
		activeName.truncate(0);
		lCreation->setText("");
		lUpdate->setText("");
		editName->setText("");
		editAuthor->setText("");
		editComment->setText("");

		editName->setEnabled(false);
		editAuthor->setEnabled(false);
		editComment->setEnabled(false);
		bDevEdit->setEnabled(false);
		bDevUpdate->setEnabled(false);
		bDevAction->setEnabled(false);
		return;
	}

	activeName = cNames->itemText(i);
	XmlObject *obj = activeLib->getDev(activeName);
	if (obj) {
		editName->setEnabled(true);
		editAuthor->setEnabled(true);
		editComment->setEnabled(true);
		bDevEdit->setEnabled(false);		// not implemented yet!
		bDevUpdate->setEnabled(true);
		bDevAction->setEnabled(true);

		lCreation->setText(obj->getCreation());
		lUpdate->setText(obj->getUpdate());
		editName->setText(activeName);
		editAuthor->setText(obj->getAuthor());
		editComment->setText(obj->getComment());
	} else {
		editName->setEnabled(false);
		editAuthor->setEnabled(false);
		editComment->setEnabled(false);
		bDevEdit->setEnabled(false);
		bDevUpdate->setEnabled(false);
		bDevAction->setEnabled(false);

		lCreation->setText("");
		lUpdate->setText("");
		editName->setText("");
		editAuthor->setText("");
		editComment->setText("");
	}
}

void DlgLib::setPixmap(int)
{
}

DevLibrary * DlgLib::getActive()
{
	return activeLib;
}

// add/remove device button clicked
// device button clicked
void DlgLib::devAction()
{	XmlObject *obj;

	if (!activeLib) {
		QMessageBox::information(this,
			i18n("device action"),
			i18n("choose a library first!"));
		return;
	}

	if (newDev) {
		int ret = activeLib->addToLib(editName->text(), newDev);
		switch (ret) {
			case 0:
				break;	// success
			case -1:
				QMessageBox::information(this, i18n("add device"),
					i18n("device name already used"));
				return;
				break;
			case -2:
				QMessageBox::information(this, i18n("add device"),
					i18n("no device id slots left .."));
				return;
				break;
			default:
				break;
		}
		
		obj = activeLib->getDev(editName->text());
		if (obj) {
			activeLib->setChanged();
			activeName = editName->text();
			obj->setCreation();
			obj->setUpdate();
			obj->setAuthor(editAuthor->text());
			obj->setComment(editComment->text());
			lCreation->setText(obj->getCreation());
			lUpdate->setText(obj->getUpdate());
			bDevAction->setText(i18n("remove"));
			newDev = NULL;

			// actualize device list
			updateNameCombo();

			// activate save button
			bClose->setEnabled(true);
			cNames->setEnabled(true);
			labelMode->setText(i18n("Choose a Device"));
		} else qFatal("unable to retrieve inserted device from lib!?");
	} else {
		// remove device from lib
		activeLib->removeFromLib(cNames->currentText());
		activeName.truncate(0);

		// update device list
		updateNameCombo();
	}
}

void DlgLib::devUpdate()
{
	XmlObject *obj = activeLib->getDev(activeName);
	if (obj) {
		activeLib->setChanged();
		obj->setUpdate();
		obj->setAuthor(editAuthor->text());
		obj->setComment(editComment->text());
		lCreation->setText(obj->getCreation());
		lUpdate->setText(obj->getUpdate());
		bDevAction->setText(i18n("remove"));

		if (0 != activeName.compare(editName->text())) {
			// update device name
			activeLib->setName(activeName, editName->text());
			activeName = editName->text();

			updateNameCombo();
		}
	}
}

void DlgLib::devEdit()
{
}

void DlgLib::updateNameCombo()
{
	// update device list
	if (!activeLib) {
		cNames->clear();
		setName(-1);
		return;
	}
	QStringList names;
	activeLib->deviceList(names);
	cNames->clear();
	cNames->addItems(names);
	setCombo2ActiveName();
}

void DlgLib::setCombo2ActiveName()
{
	// if list is empty, disable dialog elements
	if (!cNames->count()) {
		setName(-1);
		return;
	}

	// set active device as current again
	for(int i = 0; i < cNames->count(); i++) {
		if (cNames->itemText(i).compare(activeName) == 0) {
			cNames->setCurrentIndex(i);
			setName(i);
			return;
		}
	}

	// no match: set dialog elements to current item
	activeName.truncate(0);
	setName(cNames->currentIndex());
}

// list changed
void DlgLib::updateLibCombo()
{
	cLibList->clear();
	KlogicList<DevLibrary> *ll = libList->First();
	while(ll) {
		cLibList->addItem(ll->getText());
		ll = ll->Next();
	}

	if (!activeLib) {
		setActiveLib(cLibList->currentIndex());
		return;
	}

	for(int i = 0; i < cLibList->count(); i++) {
		if (cLibList->itemText(i).compare(activeLib->getFileName()) == 0) {
			cLibList->setCurrentIndex(i);
			setActiveLib(i);
			return;
		}
	}
	activeLib = (DevLibrary *)NULL;
	setActiveLib(cLibList->currentIndex());
}






