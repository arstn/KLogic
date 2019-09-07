// to be placed in the posessing classes ..
#define NFAIL 		0
#define DFAIL 		0
#define NOK 		1
#define WOK 		1
#define DOK 		1
#define WCONN 		10
#define WDISC 		11
#define WSHORT 		12
#define DCONN 		13
#define DDISC 		14
#define DFAILMAXI 	15
#define DFAILMAXO 	16

// messages
#define NEWWIRE		"wire creation with point p not in wire???"
#define NOACTIVE	"no active node!"
#define NOACTIVECONN	"connectInput: no active node or node not end-node!"

#ifndef __KLOGIC_GLOBAL__
#define __KLOGIC_GLOBAL__
#include <qstring.h>
#include <klocale.h>
#include <qfont.h>

#include "DeviceType.h"

namespace KlogicGlobal
{
	static const int SIMSTEPTIME_MIN = 0;
	static const int SIMSTEPTIME_MAX = 400;

	static const QString EMPTYSTRING;

	struct Klogic
	{
		static const QString VERSION_STR;
		static const char *VERSION;
		static const QString Name;
		static const QString emptyFileName;
		
	};

	struct Screen
	{
		static const int SCREEN_SIZE_X = 800;
		static const int SCREEN_SIZE_Y = 600;

		static int VIRT_SCREEN_SIZE_X;
		static int VIRT_SCREEN_SIZE_Y;
	};

	struct CurrentCircuit
	{
		static QString filename;
		static QString version;
		static QString creation;
		static QString author;
		static QString title;
		static QString comment;
	};

	// constants and defaults for Device class
	class Device
	{
	public:
		enum IOTYPE { NOTYPE, INPUT, TMP_OUTPUT, INTERNAL_OUTPUT, FINAL_OUTPUT };
		enum CLOCKTYPE { CLK_RISING_2EDGE, CLK_FALLING_2EDGE,
			CLK_HIGH_VALUE, CLK_LOW_VALUE,
			CLK_NONE, CLK_RISING_1EDGE, CLK_FALLING_1EDGE };

		// default delay settings
		static const int MINDELAY = 1;
		static const int MAXDELAY = 99;
		static const int DEFAULT_DELAY = 1;
		static void setDelay(int iDelay);
		static int getDelay();

		// default undefined value setting
		static const int DEFAULT_UNDEFINED_VALUE = 0;
		static void setUndefinedValue(int iUndefinedValue);
		static int getUndefinedValue();

		// default clock setting
		static const CLOCKTYPE DEFAULT_CLOCK = CLK_FALLING_1EDGE;
		static void setClock(CLOCKTYPE iClock);
		static CLOCKTYPE getClock();

		// default size setting
		static const int DEFAULT_SIZE = 4;
		static const int MINSIZE = 2;
		static const int MAXSIZE = 99;
		static void setSize(int iSize);
		static int getSize();

		// default inverter setting
		static const bool DEFAULT_INVERTED = false;
		static void setInverted(bool bInverted);
		static bool isInverted();

		// default display names
		static const bool DEFAULT_DISPLAY_NAMES = false;
		static void setDisplayNames(bool bDisplayNames);
		static bool isDisplayNames();

		// default font
		static QString DEFAULT_FONTFAMILY;
		static int DEFAULT_FONTSIZE;
		static int DEFAULT_FONTAPPEARANCE;
		static void setFontFamily(const QString &);
		static const QString& getFontFamily();
		static const QFont& getFont();
		static void setFontSize(int);
		static int getFontSize();

		// default width
		static const int DEFAULT_WIDTH = 2;
		static void setWidth(int iWidth);
		static int getWidth();

		// symbolset setting
		static const int SYMBOLSET_DIN40900 = 0;
		static const int SYMBOLSET_IEC617 = 1;
		static void setSymbolSet(int);
		static int getSymbolSet();

		// device counting and naming
		static QString getName(int iFunction);

		// device output tristate value
		static const int TRISTATE = -1;

		// maximum input and output
		static const int MAX_INPUT_CONNECTORS = 99;
		static const int MAX_OUTPUT_CONNECTORS = 99;

		static QFont m_oFont;

	private:
		// for naming the devices uniquely
		static int Counter[DeviceType::fID_COUNT];
		static QString Name[DeviceType::fID_COUNT];

		static int m_iDelay;
		static int m_iUndefinedValue;
		static CLOCKTYPE m_iClock;

		static int m_iSize;
		static bool m_bInvert;
		static bool m_bDisplayNames;
		static QString m_sFontFamilyName;
		static int m_iFontSize;
		static int m_iWidth;
		static int m_iSymbolSet;
	};
};

class uniqueID
{
public:
	static void reset() { id = 0; };
	static int getID() { return ++id; };
private:
	static int id;
};

#endif
