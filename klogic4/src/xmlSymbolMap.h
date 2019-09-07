/*******************************************/
/* XML Symbols                             */
/*                                         */
/* each symbol needs a name, number        */
/* and a map entry                         */
/*                                         */
/* Andreas Rostin 11.01.2003               */
/*******************************************/
#ifndef __XMLSYMBOLMAP__
#define __XMLSYMBOLMAP__

#include <map>
using namespace std;

#include <qstring.h>

namespace XmlSymbol
{
        static const QString sFILE_TYPE = "FILE_TYPE";

        static const QString sKLOGIC_MAIN = "KLOGIC_MAIN";
        static const QString sCIRCUIT = "CIRCUIT";
        static const QString sDEV_CONN = "DEV_CONN";
        static const QString sWIRE_CONN = "WIRE_CONN";
        static const QString sWIRE = "WIRE";
        static const QString sDEVICE = "DEV";
        static const QString sID = "ID";
        static const QString sLIBDEV = "LIBDEV";

        static const QString sPROGRAM_VERSION = "PROGRAM_VERSION";
        static const QString sCREATION_DATE = "CREATION_DATE";
        static const QString sUPDATE_DATE = "UPDATE_DATE";
        static const QString sAUTHOR = "AUTHOR";
        static const QString sCOMMENT = "COMMENT";

        static const QString sFUNCTION = "F";
        static const QString sDELAY = "DELAY";
        static const QString sUNDEF = "UNDEF";
        static const QString sINV = "INV";
        static const QString sCLK_TYPE = "CLK_TYPE";
        static const QString sMASTER = "MASTER";
        static const QString sOSC_ON = "OSC_ON";
        static const QString sOSC_OFF = "OSC_OFF";
        static const QString sOSC_STEP = "OSC_STEP";
        static const QString sSTAT = "STAT";
        static const QString sSIZE = "SZ";
        static const QString sXPOS = "X";
        static const QString sYPOS = "Y";
        static const QString sRAM = "RAM";
        static const QString sTEXT_DISP = "TEXT_DISP";
        static const QString sTEXT = "TEXT";
        static const QString sPUSH = "PUSH";
        static const QString sWIDTH = "WIDTH";

        static const QString sOUT_EQU = "OUT_EQU";
        static const QString sOUT_ID = "OUT_ID";
        static const QString sIN_NAME = "IN_NAME";
        static const QString sIN_ID = "IN_ID";
        static const QString sINTERN = "INT";
        static const QString sTRI = "TRI";
        static const QString sPOS = "POS";
        static const QString sNAME = "NAME";
        static const QString sBUSTYPE = "BUSTYPE";
        static const QString sCOLOR = "COLOR";

        // main elements
        static const int FILE_TYPE = 1;
        static const int KLOGIC_MAIN = 2;
        static const int CIRCUIT = 3;
        static const int DEV_CONN = 4;
        static const int WIRE_CONN = 5;
        static const int WIRE = 6;
        static const int DEVICE = 7;
        static const int ID = 8;        // object reference tag or attribute
        static const int LIBDEV = 9;

        // childs of KLOGIC_MAIN
        static const int PROGRAM_VERSION = 20;
        static const int CREATION_DATE = 21;
        static const int UPDATE_DATE = 22;
        static const int AUTHOR = 23;
        static const int COMMENT = 25;

        // childs of devices, circuits and wires
        static const int FUNCTION = 40;
        static const int DELAY = 41;
        static const int UNDEF = 42;
        static const int INV = 43;
        static const int CLK_TYPE = 44;
        static const int MASTER = 45;
        static const int OSC_ON = 46;
        static const int OSC_OFF = 47;
        static const int OSC_STEP = 48;
        static const int STAT = 49;
        static const int SIZE = 50;
        static const int XPOS = 51;
        static const int YPOS = 52;
        static const int RAM = 53;
        static const int TEXT_DISP = 54;
        static const int TEXT = 55;
        static const int PUSH = 56;
        static const int WIDTH = 57;

        // childs of devices
        static const int OUT_EQU = 60;
        static const int OUT_ID = 61;
        static const int IN_NAME = 62;
        static const int IN_ID = 63;
        static const int INTERN = 64;
        static const int TRI = 65;
        static const int POS = 66;
        static const int NAME = 67;
        static const int BUSTYPE = 68;
        static const int COLOR = 69;
}

// name - id mapping in both directions
class XmlSymbolMap
{
public:
	static const XmlSymbolMap* getInstance();

	inline const QString& operator[] (int id) const
	{
		return m_oIDMap[id];
	}

	inline int operator[] (const QString& sName) const
	{
		return m_oNameMap[sName];
	}

	inline bool isEqual(const QString& sName, int id) const
	{
		int localID = m_oNameMap[sName];
		return (id == localID);
	}

	static const XmlSymbolMap& m_oMap;

private:
	void add(const QString& name, int id)
	{
		m_oNameMap[name] = id;
		m_oIDMap[id] = name;
	}

	explicit XmlSymbolMap()
	{
	}

	mutable map<QString, int> m_oNameMap;
	mutable map<int, QString> m_oIDMap;

	static XmlSymbolMap *m_poInstance;
};

#endif
