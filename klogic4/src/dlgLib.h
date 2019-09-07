#ifndef __DLGLIB
#define __DLGLIB

#include <klogicList.h>
#include <QDialog>
class KLineEdit;

class QFrame;
class QWidget;
class QPushButton;
class QLabel;
class QComboBox;

class XDevice;
class DevLibrary;

class DlgLibMeta : public QDialog
{
	Q_OBJECT
public:
	DlgLibMeta(QWidget *, const QString&, DevLibrary *);

private:
	void done(int);

	DevLibrary *activeLib;
	KLineEdit *editAuthor;
	KLineEdit *editComment;
	QPushButton *bOK;
};

class DlgLib : public QDialog
{
	Q_OBJECT

public:
	DlgLib(QWidget *, const QString&, KlogicList<DevLibrary> *, DevLibrary *, XDevice *);
	~DlgLib();
	DevLibrary * getActive();

private slots:
	void newLib();
	void openLib();
	void closeLib();
	void setName(int);
	void setActiveLib(int);
	void setPixmap(int);
	void devEdit();
	void devUpdate();
	void devAction();

private:
	void createDialogContent();
	void updateLibCombo();
	void updateNameCombo();
	void setCombo2ActiveName();
	void done(int);

	QLabel *label_current;
	QLabel *labelMode;
	QComboBox *cLibList;
	QPushButton *bNewLib;
	QPushButton *bOpen;
	QPushButton *bClose;

	QComboBox *cNames;
	KLineEdit *editName;
	QLabel *lCreation;
	QLabel *lUpdate;
	KLineEdit *editAuthor;
	KLineEdit *editComment;
	QComboBox *cPixmaps;
	KLineEdit *editPixmap;

	QPushButton *bDevEdit;
	QPushButton *bDevUpdate;
	QPushButton *bDevAction;

	QPushButton *bOK;
	QPushButton *bCancel;

	KlogicList<DevLibrary> *libList;
	DevLibrary *activeLib;
	XDevice *newDev;
	QString activeName;
	QFrame *fLibDevEdit;
};

#endif
