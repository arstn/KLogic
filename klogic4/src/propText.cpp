/*************************************************/
/* methods for class PropText                    */
/*                                               */
/* text property dialog                          */
/*                                               */
/* Andreas Rostin                                */
/* 02.10.99                                      */
/*************************************************/
#include <klineedit.h>

#include <qmessagebox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
 
#include <KlogicGlobal.h>
#include <xdevice.h>
#include <circuit.h>
#include <propText.h>
#include "propText.moc"

PropText::PropText(QWidget *parent, const QString &name, Circuit *_dnet, XDevice *_dev)
	: QDialog(parent, Qt::Dialog)
{
    setModal(true);
    setWindowTitle(name);
    // main variable for this dialog:
    dev = _dev;
    dnet = _dnet;

    devtext = new KLineEdit(this);
    devtext->setGeometry(10, 10, 220, 20);
    devtext->setMaxLength(40);

    bCancel = new QPushButton(this);
    bCancel->setGeometry(130, 50, 60, 25);
    bCancel->setText(i18n("Cancel"));

    bOK = new QPushButton(this);
    bOK->setGeometry(50, 50, 60, 25);
    bOK->setDefault(true);
    bOK->setText(i18n("OK"));

    setFixedSize(240, 95);
    devtext->setText(dev->getName());

    connect(bOK, SIGNAL(clicked()), SLOT(accept()));
    connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
}

void PropText::done(int r)
{	const QString& text = devtext->text();

	if (r == Accepted) {
		if (dev->getName() != text) {
			dnet->setDeviceName(dev->getID(), text);
			dev->setImage();
		}
	}
	QDialog::done(r);
}

