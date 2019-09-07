#include "xmlSymbolMap.h"

const XmlSymbolMap* XmlSymbolMap::getInstance()
{
	if (m_poInstance) return m_poInstance;

	m_poInstance = new XmlSymbolMap;
	m_poInstance->add(XmlSymbol::sFILE_TYPE, XmlSymbol::FILE_TYPE);

	m_poInstance->add(XmlSymbol::sKLOGIC_MAIN, XmlSymbol::KLOGIC_MAIN);
	m_poInstance->add(XmlSymbol::sCIRCUIT, XmlSymbol::CIRCUIT);
	m_poInstance->add(XmlSymbol::sDEV_CONN, XmlSymbol::DEV_CONN);
	m_poInstance->add(XmlSymbol::sWIRE_CONN, XmlSymbol::WIRE_CONN);
	m_poInstance->add(XmlSymbol::sWIRE, XmlSymbol::WIRE);
	m_poInstance->add(XmlSymbol::sDEVICE, XmlSymbol::DEVICE);
	m_poInstance->add(XmlSymbol::sID, XmlSymbol::ID);
	m_poInstance->add(XmlSymbol::sLIBDEV, XmlSymbol::LIBDEV);

	m_poInstance->add(XmlSymbol::sPROGRAM_VERSION, XmlSymbol::PROGRAM_VERSION);
	m_poInstance->add(XmlSymbol::sCREATION_DATE, XmlSymbol::CREATION_DATE);
	m_poInstance->add(XmlSymbol::sUPDATE_DATE, XmlSymbol::UPDATE_DATE);
	m_poInstance->add(XmlSymbol::sAUTHOR, XmlSymbol::AUTHOR);
	m_poInstance->add(XmlSymbol::sCOMMENT, XmlSymbol::COMMENT);

	m_poInstance->add(XmlSymbol::sFUNCTION, XmlSymbol::FUNCTION);
	m_poInstance->add(XmlSymbol::sDELAY, XmlSymbol::DELAY);
	m_poInstance->add(XmlSymbol::sUNDEF, XmlSymbol::UNDEF);
	m_poInstance->add(XmlSymbol::sINV, XmlSymbol::INV);
	m_poInstance->add(XmlSymbol::sCLK_TYPE, XmlSymbol::CLK_TYPE);
	m_poInstance->add(XmlSymbol::sMASTER, XmlSymbol::MASTER);
	m_poInstance->add(XmlSymbol::sOSC_ON, XmlSymbol::OSC_ON);
	m_poInstance->add(XmlSymbol::sOSC_OFF, XmlSymbol::OSC_OFF);
	m_poInstance->add(XmlSymbol::sOSC_STEP, XmlSymbol::OSC_STEP);
	m_poInstance->add(XmlSymbol::sSTAT, XmlSymbol::STAT);
	m_poInstance->add(XmlSymbol::sSIZE, XmlSymbol::SIZE);
	m_poInstance->add(XmlSymbol::sXPOS, XmlSymbol::XPOS);
	m_poInstance->add(XmlSymbol::sYPOS, XmlSymbol::YPOS);
	m_poInstance->add(XmlSymbol::sRAM, XmlSymbol::RAM);
	m_poInstance->add(XmlSymbol::sTEXT_DISP, XmlSymbol::TEXT_DISP);
	m_poInstance->add(XmlSymbol::sTEXT, XmlSymbol::TEXT);
	m_poInstance->add(XmlSymbol::sPUSH, XmlSymbol::PUSH);
	m_poInstance->add(XmlSymbol::sWIDTH, XmlSymbol::WIDTH);

	m_poInstance->add(XmlSymbol::sOUT_EQU, XmlSymbol::OUT_EQU);
	m_poInstance->add(XmlSymbol::sOUT_ID, XmlSymbol::OUT_ID);
	m_poInstance->add(XmlSymbol::sIN_NAME, XmlSymbol::IN_NAME);
	m_poInstance->add(XmlSymbol::sIN_ID, XmlSymbol::IN_ID);
	m_poInstance->add(XmlSymbol::sINTERN, XmlSymbol::INTERN);
	m_poInstance->add(XmlSymbol::sTRI, XmlSymbol::TRI);
	m_poInstance->add(XmlSymbol::sPOS, XmlSymbol::POS);
	m_poInstance->add(XmlSymbol::sNAME, XmlSymbol::NAME);
	m_poInstance->add(XmlSymbol::sBUSTYPE, XmlSymbol::BUSTYPE);
	m_poInstance->add(XmlSymbol::sCOLOR, XmlSymbol::COLOR);
	return m_poInstance;
}

XmlSymbolMap * XmlSymbolMap::m_poInstance = 0;
const XmlSymbolMap& XmlSymbolMap::m_oMap = *(XmlSymbolMap::getInstance());



