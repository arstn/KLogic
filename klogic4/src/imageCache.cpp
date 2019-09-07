
#include <KStandardDirs>
#include <QDebug>

#include "imageCache.h"
#include <iostream>


const int BasicImageCache::SYMBOL_SS[16] = { 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
				2008, 2009, 2010, 2011, 2012, 2013, 2014, 1015};

int ImageCache::m_iHitCounter = 0;
int ImageCache::m_iFailCounter = 0;

ostream& operator<< (ostream& os, const ImageKey& oKey)
{
	os << " " << oKey.m_iFunction;
	os << " "<< oKey.m_iSize;
	os << " "<< oKey.m_iWidth;
	os << " "<< oKey.m_iClock;
	os << " "<< oKey.m_bMaster;
	os << " "<< oKey.m_iOutput;
	os << " "<< oKey.m_iColor;
	os << " "<< oKey.m_iSymbolSet;
	return os;
}

ImageKey::ImageKey()
{
	m_iFunction = 0;
	m_iSize = 0;
	m_iWidth = 0;
	m_iClock = KlogicGlobal::Device::CLK_NONE;
	m_bMaster = 0;
	m_iOutput = 0;
	m_iColor = 0;
	m_iSymbolSet = 0;
}

ImageKey::ImageKey(int iFunction, int iSize, int iWidth, KlogicGlobal::Device::CLOCKTYPE iClock, bool bMaster, int iOutput, int iColor, int iSymbolSet)
{
	m_iFunction = iFunction;
	m_iSize = iSize;
	m_iWidth = iWidth;
	m_iClock = iClock;
	m_bMaster = bMaster;
	m_iOutput = iOutput;
	m_iColor = iColor;
	m_iSymbolSet = iSymbolSet;
}

// for output dependant images
ImageKey::ImageKey(int iFunction, int iSize, int iWidth, KlogicGlobal::Device::CLOCKTYPE iClock, bool bMaster, int iSymbolSet)
{
	m_iFunction = iFunction;
	m_iSize = iSize;
	m_iWidth = iWidth;
	m_iClock = iClock;
	m_bMaster = bMaster;
	m_iOutput = 0;
	m_iColor = 0;
	m_iSymbolSet = iSymbolSet;
}

ImageKey::~ImageKey()
{
}

int ImageKey::getType() const
{
	return m_iFunction;
}

void ImageKey::setColor(int iColor)
{
	m_iColor = iColor;
}

int ImageKey::getColor() const
{
	return m_iColor;
}

void ImageKey::setClock(KlogicGlobal::Device::CLOCKTYPE iClock)
{
	m_iClock = iClock;
}

KlogicGlobal::Device::CLOCKTYPE ImageKey::getClock() const
{
	return m_iClock;
}

void ImageKey::setWidth(int iWidth)
{
	m_iWidth = iWidth;
}

void ImageKey::setSize(int iSize)
{
	m_iSize = iSize;
}

void ImageKey::setMaster(bool bMaster)
{
	m_bMaster = bMaster;
}

bool ImageKey::getMaster() const
{
	return m_bMaster;
}

void ImageKey::setOutput(int iOutput)
{
	m_iOutput = iOutput;
}

int ImageKey::getOutput() const
{
	return m_iOutput;
}

void ImageKey::setSymbolSet(int iSymbolSet)
{
	m_iSymbolSet = iSymbolSet;
}

int ImageKey::getDevType() const
{
	return m_iDevType;
}

void ImageKey::setDevType(int iDevType)
{
	m_iDevType = iDevType;
}

void ImageKey::operator= (const ImageKey& other)
{
	m_iFunction = other.m_iFunction;
	m_iSize = other.m_iSize;
	m_iWidth = other.m_iWidth;
	m_iClock = other.m_iClock;
	m_bMaster = other.m_bMaster;
	m_iOutput = other.m_iOutput;
	m_iColor = other.m_iColor;
	m_iSymbolSet = other.m_iSymbolSet;
	m_iDevType = 0;
}

bool ImageKey::operator== (const ImageKey& other) const
{
	return compareTo(other) == 0;
}

bool ImageKey::operator< (const ImageKey& other) const
{
	return compareTo(other) == -1;
}

int ImageKey::compareTo(const ImageKey& other) const
{
	if (m_iFunction < other.m_iFunction)
		return -1;
	if (m_iFunction > other.m_iFunction)
		return 1;

	if (m_iSize < other.m_iSize)
		return -1;
	if (m_iSize > other.m_iSize)
		return 1;

	if (m_iWidth < other.m_iWidth)
		return -1;
	if (m_iWidth > other.m_iWidth)
		return 1;

	if (m_iClock < other.m_iClock)
		return -1;
	if (m_iClock > other.m_iClock)
		return 1;

	if (m_bMaster < other.m_bMaster)
		return -1;
	if (m_bMaster > other.m_bMaster)
		return 1;

	if (m_iOutput < other.m_iOutput)
		return -1;
	if (m_iOutput > other.m_iOutput)
		return 1;

	if (m_iColor < other.m_iColor)
		return -1;
	if (m_iColor > other.m_iColor)
		return 1;

	if (m_iSymbolSet < other.m_iSymbolSet)
		return -1;
	if (m_iSymbolSet > other.m_iSymbolSet)
		return 1;

	if (m_iDevType < other.m_iDevType)
		return -1;
	if (m_iDevType > other.m_iDevType)
		return 1;
	
	return 0;
}

ImageCache::ImageCache()
{
}

ImageCache::~ImageCache()
{
	for (iterator iter = begin(); iter != end(); ++iter)
		iter->second->relRef();
}

const Image* ImageCache::getImage(const ImageKey& oKey) const
{
	const_iterator iter = find(oKey);
	if (iter != end()) {
		m_iHitCounter++;
		return iter->second;
	}
	m_iFailCounter++;
	return 0;
}

void ImageCache::addImage(const ImageKey& oKey, Image *poImage)
{
	poImage->addRef();
	(*this)[oKey] = poImage;
}

BasicImageCache::BasicImageCache()
{
}

void BasicImageCache::init()
{
	QString imgPath;
	bool found = false;
	QStringList sl = KGlobal::dirs()->findDirs("data", "klogic");
	for (QStringList::ConstIterator it = sl.begin(); !found && it != sl.end(); ++it) {
	    QPixmap pix;
	    if (pix.load((*it) + "deviceIcons/AND.xpm")) {
	      qDebug() << "Found matching directory " << *it;
	      imgPath = (*it) + "deviceIcons/";
	      found = true;
	    }
	}
	if (!found) {
	  qFatal("It seems that KLogic is not installed properly. The application icons could not be found.");
	}

	Image *poImage;

	poImage = new Image();
	poImage->load(imgPath + "pxPOS.xpm");
	addImage(BODY_POS, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxNEG.xpm");
	addImage(BODY_NEG, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxRS.xpm");
	addImage(BODY_RS, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxRSC.xpm");
	addImage(BODY_RSC, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxRSCM.xpm");
	addImage(BODY_RSCM, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxRSCMF.xpm");
	addImage(BODY_RSCMF, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxDM.xpm");
	addImage(BODY_DM, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxD.xpm");
	addImage(BODY_D, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxJK.xpm");
	addImage(BODY_JK, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxJKF.xpm");
	addImage(BODY_JKF, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxOUT.xpm");
	addImage(BODY_OUT, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxIN.xpm");
	addImage(BODY_IN, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED0red.xpm");
	addImage(BODY_LED0red, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED1red.xpm");
	addImage(BODY_LED1red, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED0green.xpm");
	addImage(BODY_LED0green, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED1green.xpm");
	addImage(BODY_LED1green, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED0blue.xpm");
	addImage(BODY_LED0blue, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED1blue.xpm");
	addImage(BODY_LED1blue, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED0yellow.xpm");
	addImage(BODY_LED0yellow, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxLED1yellow.xpm");
	addImage(BODY_LED1yellow, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxSS.xpm");
	addImage(BODY_SSB, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxSwitch0.xpm");
	addImage(BODY_SWI0, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxSwitch1.xpm");
	addImage(BODY_SWI1, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "TopHuge.xpm");
	addImage(BODY_TOP_HUGE, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "MidHuge.xpm");
	addImage(BODY_MID_HUGE, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "BotHuge.xpm");
	addImage(BODY_BOT_HUGE, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "TopLarge.xpm");
	addImage(BODY_TOP_LARGE, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "MidLarge.xpm");
	addImage(BODY_MID_LARGE, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "BotLarge.xpm");
	addImage(BODY_BOT_LARGE, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "Top.xpm");
	addImage(BODY_TOP, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "Mid.xpm");
	addImage(BODY_MID, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "Bot.xpm");
	addImage(BODY_BOT, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "TopBlank.xpm");
	addImage(BODY_TOP_BLANK, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "MidBlank.xpm");
	addImage(BODY_MID_BLANK, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "BotBlank.xpm");
	addImage(BODY_BOT_BLANK, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "pxTFF.xpm");
	addImage(BODY_TFF, poImage);
	poImage->relRef();

	// -------------------------------------------
	// now loading symbols - drawn into bodies
	// -------------------------------------------

	poImage = new Image();
	poImage->load(imgPath + "AND.xpm");
	addImage(SYMBOL_AND, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "OR.xpm");
	addImage(SYMBOL_OR, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "XOR.xpm");
	addImage(SYMBOL_XOR, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "INV.xpm");
	addImage(SYMBOL_NOT, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "IEC_AND.xpm");
	addImage(SYMBOL_IECAND, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "IEC_OR.xpm");
	addImage(SYMBOL_IECOR, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "IEC_XOR.xpm");
	addImage(SYMBOL_IECXOR, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "IEC_NOT.xpm");
	addImage(SYMBOL_IECNOT, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "OSC.xpm");
	addImage(SYMBOL_OSC, poImage);
	poImage->relRef();

	poImage = new Image();
	poImage->load(imgPath + "MONOFLOP.xpm");
	addImage(SYMBOL_MONOFLOP, poImage);
	poImage->relRef();

	for (int i = 0; i < 16; i++) {
		QString sPath = imgPath + "pxSSs" + QString::number(i) + ".xpm";
		poImage = new Image();
		poImage->load(sPath);
		addImage(SYMBOL_SS[i], poImage);
		poImage->relRef();
	}

	poImage = new Image();
	poImage->load(imgPath + "cursor.xpm");
	addImage(CURSOR_SIMWIDGET, poImage);
	poImage->relRef();
}

BasicImageCache::~BasicImageCache()
{
	for (iterator iter = begin(); iter != end(); ++iter) {
		Image *poImage = iter->second;
		poImage->relRef();
	}
}

Image* BasicImageCache::getImage(int iKey) const
{
	const_iterator iter = find(iKey);
	if (iter != end())
		return iter->second;
	return 0;
}

void BasicImageCache::addImage(int iKey, Image *poImage)
{
	poImage->addRef();
	(*this)[iKey] = poImage;
}

