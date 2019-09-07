//****************************************************************
//** digital simulation 
//** 
//** Andreas Rostin
//** 12.10.1997
//****************************************************************
#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KLocale>

#include <KlogicGlobal.h>
#include <kdeversion.h>

const QString KlogicGlobal::Klogic::VERSION_STR = "4.0.2 2012-04-17";
const char *KlogicGlobal::Klogic::VERSION = "4.0.2";

// unique-id counter
int uniqueID::id = 0;

// global constants
const QString KlogicGlobal::Klogic::Name = "klogic";
const QString KlogicGlobal::Klogic::emptyFileName = i18n("new.circuit");


int KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X = 2600;
int KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y = 1200;

QString KlogicGlobal::CurrentCircuit::filename;
QString KlogicGlobal::CurrentCircuit::version;
QString KlogicGlobal::CurrentCircuit::creation;
QString KlogicGlobal::CurrentCircuit::author;
QString KlogicGlobal::CurrentCircuit::title;
QString KlogicGlobal::CurrentCircuit::comment;

int KlogicGlobal::Device::m_iDelay = KlogicGlobal::Device::DEFAULT_DELAY;
int KlogicGlobal::Device::m_iUndefinedValue = KlogicGlobal::Device::DEFAULT_UNDEFINED_VALUE;
KlogicGlobal::Device::CLOCKTYPE KlogicGlobal::Device::m_iClock = KlogicGlobal::Device::DEFAULT_CLOCK;
int KlogicGlobal::Device::m_iSize = KlogicGlobal::Device::DEFAULT_SIZE;
bool KlogicGlobal::Device::m_bInvert = KlogicGlobal::Device::DEFAULT_INVERTED;
bool KlogicGlobal::Device::m_bDisplayNames = KlogicGlobal::Device::DEFAULT_DISPLAY_NAMES;

QString KlogicGlobal::Device::DEFAULT_FONTFAMILY = "norasi";
int KlogicGlobal::Device::DEFAULT_FONTSIZE = 8;
int KlogicGlobal::Device::DEFAULT_FONTAPPEARANCE = QFont::Normal;
QString KlogicGlobal::Device::m_sFontFamilyName = KlogicGlobal::Device::DEFAULT_FONTFAMILY;
QFont KlogicGlobal::Device::m_oFont = QFont(KlogicGlobal::Device::m_sFontFamilyName, KlogicGlobal::Device::DEFAULT_FONTSIZE, KlogicGlobal::Device::DEFAULT_FONTAPPEARANCE, false);
int KlogicGlobal::Device::m_iFontSize = KlogicGlobal::Device::DEFAULT_FONTSIZE;
int KlogicGlobal::Device::m_iSymbolSet = KlogicGlobal::Device::SYMBOLSET_DIN40900;

int KlogicGlobal::Device::m_iWidth = KlogicGlobal::Device::DEFAULT_WIDTH;

int KlogicGlobal::Device::Counter[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				 0, 0, 0, 0, 0, 0};
QString KlogicGlobal::Device::Name[] = {"and", "or", "inv", "xor", "7s", "switch", "osc", "", "rs", "d",
                             "jk", "led", "in", "out", "net", "pwr", "text", "sub", "led", "led",
                             "led", "tri", "ram", "", "", "t"};

void KlogicGlobal::Device::setDelay(int iDelay)
{
	if (iDelay > KlogicGlobal::Device::MAXDELAY)
		iDelay = KlogicGlobal::Device::MAXDELAY;
	if (iDelay < KlogicGlobal::Device::MINDELAY)
		iDelay = KlogicGlobal::Device::MINDELAY;
	KlogicGlobal::Device::m_iDelay = iDelay;
}

int KlogicGlobal::Device::getDelay()
{
	return m_iDelay;
}

void KlogicGlobal::Device::setUndefinedValue(int iUndefinedValue)
{
	m_iUndefinedValue = iUndefinedValue;
}

int KlogicGlobal::Device::getUndefinedValue()
{
	return m_iUndefinedValue;
}

void KlogicGlobal::Device::setClock(KlogicGlobal::Device::CLOCKTYPE iClock)
{
	switch (iClock) {
		case CLK_RISING_2EDGE:
			iClock = CLK_RISING_1EDGE;
			break;
		case CLK_FALLING_2EDGE:
			iClock = CLK_FALLING_1EDGE;
			break;
		default:
			break;
	}
	m_iClock = iClock;
}

KlogicGlobal::Device::CLOCKTYPE KlogicGlobal::Device::getClock()
{
	switch (m_iClock) {
		case CLK_RISING_2EDGE:
			m_iClock = CLK_RISING_1EDGE;
			break;
		case CLK_FALLING_2EDGE:
			m_iClock = CLK_FALLING_1EDGE;
			break;
		default:
			break;
	}
	return m_iClock;
}

void KlogicGlobal::Device::setSize(int iSize)
{
	if (iSize < KlogicGlobal::Device::MINSIZE)
		iSize = KlogicGlobal::Device::MINSIZE;
	if (iSize > KlogicGlobal::Device::MAXSIZE)
		iSize = KlogicGlobal::Device::MAXSIZE;
	m_iSize = iSize;
}

int KlogicGlobal::Device::getSize()
{
	return m_iSize;
}

void KlogicGlobal::Device::setInverted(bool bInvert)
{
	m_bInvert = bInvert;
}

bool KlogicGlobal::Device::isInverted()
{
	return m_bInvert;
}

void KlogicGlobal::Device::setDisplayNames(bool bDisplayNames)
{
	m_bDisplayNames = bDisplayNames;
}

bool KlogicGlobal::Device::isDisplayNames()
{
	return m_bDisplayNames;
}

void KlogicGlobal::Device::setFontFamily(const QString &sFontFamilyName)
{
	m_sFontFamilyName = sFontFamilyName;
	m_oFont = QFont(m_sFontFamilyName, m_iFontSize, KlogicGlobal::Device::DEFAULT_FONTAPPEARANCE);
}

const QString& KlogicGlobal::Device::getFontFamily()
{
	return m_sFontFamilyName;
}

const QFont& KlogicGlobal::Device::getFont()
{
	return m_oFont;
}

void KlogicGlobal::Device::setFontSize(int iFontSize)
{
	m_iFontSize = iFontSize;
}

int KlogicGlobal::Device::getFontSize()
{
	return m_iFontSize;
}

void KlogicGlobal::Device::setWidth(int iWidth)
{
	m_iWidth = iWidth;
}

int KlogicGlobal::Device::getWidth()
{
	return m_iWidth;
}

void KlogicGlobal::Device::setSymbolSet(int iSymbolSet)
{
        m_iSymbolSet = iSymbolSet;
}

int KlogicGlobal::Device::getSymbolSet()
{
        return m_iSymbolSet;
}

QString KlogicGlobal::Device::getName(int iFunction)
{
	Counter[iFunction - DeviceType::fBASE]++;
	QString sName = Name[iFunction - DeviceType::fBASE];
	sName.append(QString::number(Counter[iFunction - DeviceType::fBASE]));
	return sName;
}
