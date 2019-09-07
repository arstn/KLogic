#include "deviceColored.h"

const QString S7Device::INPUT_7S0 = "2^0";
const QString S7Device::INPUT_7S1 = "2^1";
const QString S7Device::INPUT_7S2 = "2^2";
const QString S7Device::INPUT_7S3 = "2^3";

LedDevice::LedDevice(const QPoint& oPos, int iSize)
	: SimpleDevice(DeviceType::fLED, oPos, iSize)
{
}

void LedDevice::init()
{
	m_iLEDColor = GREEN;
	maxOutputCount = 0;
	hasOutputRegion(false);
	XDevice::init();
}

bool LedDevice::sizeChangeable() const
{
	return false;
}

// device image key for image cache
void LedDevice::setImageKey()
{
	int iOutput = output();
	if (iOutput == KlogicGlobal::Device::TRISTATE)
		iOutput = getUndefinedValue();
	m_oImageKey.setOutput(iOutput);
	XDevice::setImageKey();
}

// image not in image cache - create new one
Image * LedDevice::createImage(const ImageKey& oKey)
{
	switch (oKey.getColor()) {
	case RED:
		if (oKey.getOutput())
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED1red);
		else
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED0red);
	case GREEN:
		if (oKey.getOutput())
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED1green);
		else
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED0green);
	case YELLOW:
		if (oKey.getOutput())
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED1yellow);
		else
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED0yellow);
	case BLUE:
		if (oKey.getOutput())
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED1blue);
		else
			return m_oBasicImageCache.getImage(BasicImageCache::BODY_LED0blue);
	}
	return 0;
}

void LedDevice::updateImage() {
	if (outputChanged()) {
	    setImageKey();
	    const Image *poCachedImage = m_oImageCache.getImage(m_oImageKey);
	    if (!poCachedImage) {
	    	Image *poImage = createImage(m_oImageKey);
			m_oImageCache.addImage(m_oImageKey, poImage);
			poCachedImage = poImage;
	    }
	    XObject::setImage(poCachedImage);
	}
}

// set image in dependance of output value (simulation)
bool LedDevice::Propagate(int burst_step) {
	bool outputHasChanged = Device::Propagate(burst_step);
    setOutputChanged(outputHasChanged);
    return outputHasChanged;
}

// set color of LED
void LedDevice::setColor(int iLEDColor)
{
	m_iLEDColor = iLEDColor;
	m_oImageKey.setColor(m_iLEDColor);

	const Image *poCachedImage = m_oImageCache.getImage(m_oImageKey);
	if (poCachedImage) {
	  XObject::setImage(poCachedImage);
	} else {
	  Image *poImage = createImage(m_oImageKey);
	  m_oImageCache.addImage(m_oImageKey, poImage);
	  XObject::setImage(poImage);

	}
}

int LedDevice::getColor()
{
	return m_iLEDColor;
}

bool LedDevice::isObservable() {
	return false;
}

S7Device::S7Device(const QPoint& oPos, int iSize)
	: XDevice(DeviceType::fSS, oPos, iSize)
{
}

void S7Device::init()
{
	Device::IMPORT_IGNORE_GLOBAL = true;
	addInputName(INPUT_7S0, 0);
	addInputName(INPUT_7S1, 1);
	addInputName(INPUT_7S2, 2);
	addInputName(INPUT_7S3, 3);
	Device::IMPORT_IGNORE_GLOBAL = false;
	maxInputCount = 4;
	maxOutputCount = 0;
	hasOutputRegion(false);
	XDevice::init();
}

bool S7Device::sizeChangeable() const
{
	return false;
}

bool S7Device::hasNamedInput() const
{
	return true;
}

void S7Device::setEquation()
{
	QString sEquation = INPUT_7S0 + " " + Operator::CHR_BIN + " " + INPUT_7S1 + " " + Operator::CHR_BIN + " " + INPUT_7S2 + " " + Operator::CHR_BIN + " " + INPUT_7S3;
	Device::setEquation(sEquation);
}

// device image key for image cache
void S7Device::setImageKey()
{
	int iOutput = output();
	if (iOutput == KlogicGlobal::Device::TRISTATE)
		iOutput = getUndefinedValue();
	m_oImageKey.setOutput(iOutput);
}

// image not in image cache - create new one
Image * S7Device::createImage(const ImageKey& oKey)
{
	QPainter p;
	QPoint pt;
	QPoint pp;

	const Image *part = m_oBasicImageCache.getImage(BasicImageCache::BODY_SSB);
	Image *poImage = new Image(part->width(), part->height());
	p.begin(poImage);
	p.drawPixmap(pt, *part);
	part = m_oBasicImageCache.getImage(BasicImageCache::SYMBOL_SS[oKey.getOutput()]);
	pp.setX(poImage->width()/2 - part->width()/2 + Grid::GRIDHALF - 3);
	pp.setY(poImage->height()/2 - part->height()/2);
	p.drawPixmap(pp, *part);
	p.end();

	return poImage;
}

void S7Device::updateImage() {
	if (outputChanged()) {
	    setImageKey();
	    const Image *poCachedImage = m_oImageCache.getImage(m_oImageKey);
	    if (!poCachedImage) {
	    	Image *poImage = createImage(m_oImageKey);
			m_oImageCache.addImage(m_oImageKey, poImage);
			poCachedImage = poImage;
	    }
	    XObject::setImage(poCachedImage);
	}
}

// set image in dependance of output value (simulation)
bool S7Device::Propagate(int burst_step) {
	bool outputHasChanged = Device::Propagate(burst_step);
    setOutputChanged(outputHasChanged);
    return outputHasChanged;
}

bool S7Device::isObservable() {
	return false;
}


