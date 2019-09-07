#include <qpainter.h>
#include "deviceSimple.h"
#include "connection.h"

// simple device with same operation for all input and common output
SimpleDevice::SimpleDevice(int iFunction, const QPoint& oPos, int iSize)
        : XDevice(iFunction, oPos, iSize)
{
}

SimpleDevice::~SimpleDevice()
{
}

void SimpleDevice::init()
{
    XDevice::init();
}

void SimpleDevice::pushInputValuesToCalculationStack(Calculator *calculator) {
    for (ConnectionList::iterator iter = m_poConnectionList->begin(); iter != m_poConnectionList->end(); iter++) {
    	calculator->push((*iter)->getInput());
    }
}

void SimpleDevice::setEquation()
{
    switch (type()) {
    case DeviceType::fONE:
        Device::setEquation(Operator::CHR_OR);
        break;
    case DeviceType::fXOR:
        Device::setEquation(Operator::CHR_XOR);
        break;
    case DeviceType::fAND:
        Device::setEquation(Operator::CHR_AND);
        break;
    case DeviceType::fOR:
        Device::setEquation(Operator::CHR_OR);
        break;
    default:
        Device::setEquation(Operator::CHR_OR);
        break;
    }
}

// virtual, create a new image if not in cache already
Image * SimpleDevice::createImage(const ImageKey& oKey)
{
    // first resize the device pixmap, assign the base image to "part"..
    const Image *part = 0;
    Image *poImage;
    if (KlogicGlobal::Device::getSymbolSet() == KlogicGlobal::Device::SYMBOLSET_DIN40900 ||
            (type() != DeviceType::fAND && type() != DeviceType::fOR &&
             type() != DeviceType::fXOR && type() != DeviceType::fONE)) {
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP);
        int iHeight = part->height();
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID);
        iHeight += part->height() * getSize();
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT);
        iHeight += part->height();
        poImage = new Image(part->width(), iHeight);
    } else {
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP_BLANK);
        int iHeight = part->height();
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID_BLANK);
        iHeight += part->height() * getSize();
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT_BLANK);
        iHeight += part->height();
        poImage = new Image(part->width(), iHeight);
    }

    QPainter p;
    p.begin(poImage);

    // ..then draw the device image
    QPoint pt(0,0);
    if (KlogicGlobal::Device::getSymbolSet() == KlogicGlobal::Device::SYMBOLSET_DIN40900 ||
            (type() != DeviceType::fAND && type() != DeviceType::fOR &&
             type() != DeviceType::fXOR && type() != DeviceType::fONE)) {
        // top of the device
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_TOP);
        p.drawPixmap(pt, *part);
        pt.setY(pt.y() + part->height());

        // middle-part of device
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID);
        for (int i = 0; i < getSize(); i++) {
            p.drawPixmap(pt, *part);
            pt.setY(pt.y() + part->height());
        }

        // bottom of device
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT);
        p.drawPixmap(pt, *part);
    } else {
        // top of the device
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT_BLANK);
        p.drawPixmap(pt, *part);
        pt.setY(pt.y() + part->height());

        // middle-part of device
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_MID_BLANK);
        for (int i = 0; i < getSize(); i++) {
            p.drawPixmap(pt, *part);
            pt.setY(pt.y() + part->height());
        }

        // bottom of device
        part = m_oBasicImageCache.getImage(BasicImageCache::BODY_BOT_BLANK);
        p.drawPixmap(pt, *part);
    }

    // draw symbol
    QPoint pp(15,35);
    switch (oKey.getType()) {
    case DeviceType::fAND:
        if (KlogicGlobal::Device::getSymbolSet() == KlogicGlobal::Device::SYMBOLSET_DIN40900)
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_AND);
        else
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_IECAND);
        pp.setX(poImage->width()/2 - part->width()/2);
        pp.setY(poImage->height()/2 - part->height()/2);
        p.drawPixmap(pp, *part);
        break;
    case DeviceType::fOR:
        if (KlogicGlobal::Device::getSymbolSet() == KlogicGlobal::Device::SYMBOLSET_DIN40900)
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_OR);
        else
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_IECOR);

        pp.setX(poImage->width()/2 - part->width()/2);
        pp.setY(poImage->height()/2 - part->height()/2);
        p.drawPixmap(pp, *part);
        break;
    case DeviceType::fXOR:
        if (KlogicGlobal::Device::getSymbolSet() == KlogicGlobal::Device::SYMBOLSET_DIN40900)
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_XOR);
        else
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_IECXOR);
        pp.setX(poImage->width()/2 - part->width()/2);
        pp.setY(poImage->height()/2 - part->height()/2);
        p.drawPixmap(pp, *part);
        break;
    case DeviceType::fONE:
        if (KlogicGlobal::Device::getSymbolSet() == KlogicGlobal::Device::SYMBOLSET_DIN40900)
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_NOT);
        else
            part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_IECNOT);
        pp.setX(poImage->width()/2 - part->width()/2);
        pp.setY(poImage->height()/2 - part->height()/2);
        p.drawPixmap(pp, *part);
        break;
    default:
        break;
    }
    return poImage;
}
