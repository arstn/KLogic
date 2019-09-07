/*************************************************/
/* methods for class PropGlob                    */
/*                                               */
/* global properties dialog                      */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <klineedit.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qfontdatabase.h>
#include <QList>

#include <KlogicGlobal.h>
#include <FileIO.h>
#include <xdevice.h>

#include "propGlob.h"
#include "klogic4.h"
#include "propGlob.moc"

PropGlob::PropGlob(QWidget *parent, const QString &name, CircuitWidget *_netw)
        : QDialog(parent, Qt::Dialog)
{
    setModal(true);
    setWindowTitle(name);
    QString value;

    netw = _netw;
//---> device settings
    gb_dev = new QGroupBox(i18n("Device Default Settings"), this);
    gb_dev->setGeometry(10, 30, 200, 380);

    lsize = new QLabel(gb_dev);
    lsize->setGeometry(10, 20, 130, 18);
    lsize->setText(i18n("Default Size"));
    defsize = new KLineEdit(gb_dev);
    QIntValidator *vDefSize = new QIntValidator(defsize);
    defsize->setValidator(vDefSize);
    defsize->setGeometry(140, 20, 50, 18);
    defsize->setMaxLength(2);
    defsize->setText(value.setNum(KlogicGlobal::Device::getSize()));

    ldelay = new QLabel(gb_dev);
    ldelay->setGeometry(10, 40, 130, 18);
    ldelay->setText(i18n("Default Delay"));
    defdelay = new KLineEdit(gb_dev);
    QIntValidator *vDefDelay = new QIntValidator(defdelay);
    defdelay->setValidator(vDefDelay);
    defdelay->setGeometry(140, 40, 50, 18);
    defdelay->setMaxLength(2);
    defdelay->setText(value.setNum(KlogicGlobal::Device::getDelay()));

    cb_display_name = new QCheckBox(i18n("Display Name"), gb_dev);
    cb_display_name->setGeometry(5, 65, 160, 15);
    if (KlogicGlobal::Device::isDisplayNames()) cb_display_name->setChecked(TRUE);
    else cb_display_name->setChecked(FALSE);

    cb_ext_width = new QCheckBox(i18n("Large Sub Circuit Width"), gb_dev);
    cb_ext_width->setGeometry(5, 85, 180, 15);
    if (KlogicGlobal::Device::getWidth() == 3) cb_ext_width->setChecked(TRUE);
    else cb_ext_width->setChecked(FALSE);

    bg_invert = new QGroupBox(i18n("Outputs Inverted"), gb_dev);
    bg_invert->setGeometry(10, 110, 180, 60);
    invert[0] = new QRadioButton(i18n("Not inverted"), bg_invert);
    invert[0]->setGeometry(10, 20, 160, 15);
    invert[1] = new QRadioButton(i18n("Inverted"), bg_invert);
    invert[1]->setGeometry(10, 40, 160, 15);
    curr_invert = KlogicGlobal::Device::isInverted();
    if (curr_invert)
        invert[1]->setChecked(TRUE);
    else
        invert[0]->setChecked(TRUE);

    bg_undef = new QGroupBox(i18n("Undefined value"), gb_dev);
    bg_undef->setGeometry(10, 180, 180, 60);
    undef[0] = new QRadioButton(i18n("Low"), bg_undef);
    undef[0]->setGeometry(10, 20, 160, 15);
    undef[1] = new QRadioButton(i18n("High"), bg_undef);
    undef[1]->setGeometry(10, 40, 160, 15);
    curr_undef = KlogicGlobal::Device::getUndefinedValue();
    if (curr_undef)
        undef[1]->setChecked(TRUE);
    else
        undef[0]->setChecked(TRUE);

    // font type and size
    QLabel *l1 = new QLabel(gb_dev);
    l1->setGeometry(10, 259, 40, 15);
    l1->setText(i18n("Font"));
    QLabel *l2 = new QLabel(gb_dev);
    l2->setGeometry(85, 259, 40, 15);
    l2->setText(i18n("Size"));
    cb_font_size = new QComboBox(gb_dev);
    cb_font_size->setGeometry(130, 255, 40, 20);
    //cb_font_type = new QComboBox(i18n("select font"), gb_dev);
    cb_font_type = new QComboBox(gb_dev);
    cb_font_type->setGeometry(10, 275, 180, 20);
    connect(cb_font_type, SIGNAL(activated(int)), SLOT(theFontChange(int)));

    // fill the font lists
    QFontDatabase fntdb;
    QStringList fnt_families = fntdb.families();
    QList<int> sizes;
    QString current_font = KlogicGlobal::Device::getFontFamily();
    int idx = 0;
    int idx2 = 0;
    int current_idx = -1;
    int current_idx_sz = -1;
    bool found;
    bool size_found;
    QString act_font;
    QString buf;
    for (int i = 0; i < fnt_families.count(); i++) {
        found = false;
        size_found = false;

        // do not add duplicates!
        act_font = fnt_families[i];
        for (int j = 0; j < cb_font_type->count(); j++) {
            if (cb_font_type->itemText(j) == act_font)
                found = true;
        }

        // only add families with an available size smaller 10 point
        sizes = fntdb.pointSizes(act_font);
        for (int k = 0; k < sizes.count(); k++)
            if (sizes[k] <= 10) size_found = true;

        // ok, add font family
        if (!found && size_found) {
            if (act_font == current_font) {
                current_idx = idx;
                idx2 = 0;
                for (int k = 0; k < sizes.count(); k++) {
                    if (sizes[k] <= 10) {
                        if (sizes[k] == KlogicGlobal::Device::getFontSize())
                            current_idx_sz = idx2;
                        buf.sprintf("%d", sizes[k]);
                        cb_font_size->addItem(buf, idx2++);
                    }
                }
            }
            cb_font_type->addItem(act_font, idx++);
        }
    }
    if (current_idx != -1)
        cb_font_type->setCurrentIndex(current_idx);
    if (current_idx_sz != -1)
        cb_font_size->setCurrentIndex(current_idx_sz);
//<---

//---> clock settings
    bg_clock = new QGroupBox(i18n("def. trigger (flipflop)"), this);
    bg_clock->setGeometry(220, 30, 200, 135);

    clock[5] = new QRadioButton(i18n("Rising edge"), bg_clock);
    clock[5]->setGeometry(10,20, 180, 15);
    clock[6] = new QRadioButton(i18n("Falling edge"), bg_clock);
    clock[6]->setGeometry(10,40, 180, 15);
    clock[2] = new QRadioButton(i18n("High value"), bg_clock);
    clock[2]->setGeometry(10,60, 180, 15);
    clock[3] = new QRadioButton(i18n("Low value"), bg_clock);
    clock[3]->setGeometry(10,80, 180, 15);
    curr_clk = KlogicGlobal::Device::getClock();
    clock[curr_clk]->setChecked(TRUE);

//<---

//---> simulation settings
    gb_sim = new QGroupBox(i18n("simulation"), this);
    gb_sim->setGeometry(220, 180, 200, 80);

    lstepTime = new QLabel(gb_sim);
    lstepTime->setGeometry(10, 20, 130, 20);
    lstepTime->setText(i18n("speed (ms)"));
    stepTime = new KLineEdit(gb_sim);
    QIntValidator *vStepTime = new QIntValidator(stepTime);
    stepTime->setValidator(vStepTime);
    stepTime->setGeometry(140, 20, 50, 20);
    stepTime->setMaxLength(5);
    stepTime->setText(value.setNum(klogic4::getInstance()->getSimStepTime()));

    lburst = new QLabel(gb_sim);
    lburst->setGeometry(10, 50, 130, 20);
    lburst->setText(i18n("burst stepping"));
    burst = new KLineEdit(gb_sim);
    QIntValidator *vBurst = new QIntValidator(burst);
    burst->setValidator(vBurst);
    burst->setGeometry(140, 50, 50, 20);
    burst->setMaxLength(5);
    burst->setText(value.setNum(klogic4::getInstance()->getSimBurstCount()));
//<---

//---> filetype settings
    bg_ftype = new QGroupBox(i18n("save file format"), this);
    bg_ftype->setGeometry(220, 275, 200, 65);

    xml_ftype = new QRadioButton(i18n("XML format"), bg_ftype);
    xml_ftype->setGeometry(10,20, 100, 15);
    klogic_ftype = new QRadioButton(i18n("old format"), bg_ftype);
    klogic_ftype->setGeometry(10,40, 100, 15);
    if (FileIO::getSaveFileFormat() == FileIO::SAVE_XML)
        xml_ftype->setChecked(TRUE);
    else
        klogic_ftype->setChecked(TRUE);
//<---

    bCancel = new QPushButton(this);
    bCancel->setGeometry(290, 430, 90, 20);
    bCancel->setText(i18n("Cancel"));

    bApply = new QPushButton(this);
    bApply->setGeometry(170, 430, 90, 20);
    bApply->setText(i18n("Apply"));

    bOK = new QPushButton(this);
    bOK->setGeometry(50, 430, 90, 20);
    bOK->setDefault(TRUE);
    bOK->setText(i18n("OK"));

    setFixedSize(430, 470);

    connect(bg_undef, SIGNAL(clicked(int)), this, SLOT(undefChange(int)));
    connect(bg_invert, SIGNAL(clicked(int)), this, SLOT(invertedChange(int)));
    connect(bg_clock, SIGNAL(clicked(int)), this, SLOT(clockChange(int)));

    connect(bOK, SIGNAL(clicked()), SLOT(accept()));
    connect(bApply, SIGNAL(clicked()), SLOT(apply()));
    connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
}

// does not apply changes to existing devices
void PropGlob::done(int r)
{
    QString value;

    if (r == Accepted) {
        value = defdelay->text();
        KlogicGlobal::Device::setDelay(value.toInt());
        if (cb_display_name->isChecked()) KlogicGlobal::Device::setDisplayNames(true);
        else KlogicGlobal::Device::setDisplayNames(false);
        if (cb_ext_width->isChecked()) KlogicGlobal::Device::setWidth(3);
        else KlogicGlobal::Device::setWidth(2);
        value = defsize->text();
        KlogicGlobal::Device::setSize(value.toInt());
        KlogicGlobal::Device::setUndefinedValue(curr_undef);
        KlogicGlobal::Device::setInverted(curr_invert);
        KlogicGlobal::Device::setClock(curr_clk);
        KlogicGlobal::Device::setFontFamily(cb_font_type->currentText());
        KlogicGlobal::Device::setFontSize(cb_font_size->currentText().toInt());

        value = stepTime->text();
        if (value.toInt() != klogic4::getInstance()->getSimStepTime()) {
            klogic4::getInstance()->setSimStepTime(value.toInt());
        }
        value = burst->text();
        klogic4::getInstance()->setSimBurstCount(value.toInt());
        if (xml_ftype->isChecked()) FileIO::setSaveFileFormat(FileIO::SAVE_XML);
        else FileIO::setSaveFileFormat(FileIO::SAVE_KLOGIC);
    }
    QDialog::done(r);
}

// apply changes to all existing devices
void PropGlob::apply()
{
    QString value;

    // set new default values
    value = defdelay->text();
    KlogicGlobal::Device::setDelay(value.toInt());
    value = defsize->text();
    KlogicGlobal::Device::setSize(value.toInt());
    if (cb_display_name->isChecked()) KlogicGlobal::Device::setDisplayNames(true);
    else KlogicGlobal::Device::setDisplayNames(false);
    if (cb_ext_width->isChecked()) KlogicGlobal::Device::setWidth(3);
    else KlogicGlobal::Device::setWidth(2);
    KlogicGlobal::Device::setUndefinedValue(curr_undef);
    KlogicGlobal::Device::setInverted(curr_invert);
    KlogicGlobal::Device::setClock(curr_clk);
    KlogicGlobal::Device::setFontFamily(cb_font_type->currentText());
    KlogicGlobal::Device::setFontSize(cb_font_size->currentText().toInt());

    value = stepTime->text();
    if (value.toInt() != klogic4::getInstance()->getSimStepTime()) {
        klogic4::getInstance()->setSimStepTime(value.toInt());
    }
    value = burst->text();
    klogic4::getInstance()->setSimBurstCount(value.toInt());
    if (xml_ftype->isChecked()) FileIO::setSaveFileFormat(FileIO::SAVE_XML);
    else FileIO::setSaveFileFormat(FileIO::SAVE_KLOGIC);

    netw->applyDefaults();
}

void PropGlob::undefChange(int newi)
{
    curr_undef = newi;
}

void PropGlob::invertedChange(int newi)
{
    curr_invert = newi;
}

void PropGlob::clockChange(int iIntClock)
{
    curr_clk = (KlogicGlobal::Device::CLOCKTYPE)iIntClock;
}

void PropGlob::theFontChange(int fnt)
{
    QFontDatabase fntdb;
    QList<int> sizes;
    QString buf;

    sizes = fntdb.pointSizes(cb_font_type->itemText(fnt));
    cb_font_size->clear();
    for (int k = 0; k < sizes.count(); k++) {
        if (sizes[k] <= 10) {
            buf.sprintf("%d", sizes[k]);
            cb_font_size->addItem(buf);
        }
    }
}

