/*************************************************/
/* methods for class FileIO                    */
/*                                               */
/* load/save digital circuits                    */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qregexp.h>

#include <KlogicGlobal.h>
#include <circuit.h>
#include <FileIO.h>
#include <xmlExport.h>
#include <xmlImport.h>
#include <xmlSymbolMap.h>

int FileIO::file_format = FileIO::SAVE_XML;

// static
void FileIO::setSaveFileFormat(int new_format)
{
	file_format = new_format;
}

// static
int FileIO::getSaveFileFormat()
{
	return file_format;
}

FileIO::FileIO(QString filename, Circuit *_circuit)
{
	circuit = _circuit;
	fname = filename;
	if (-1 == fname.indexOf(QRegExp("\\.circuit$")))
		fname.append(".circuit");
	circuit->initImport();
}

FileIO::FileIO(QString filename)
{
	circuit = (Circuit *)NULL;
	fname = filename;
	if (-1 == fname.indexOf(QRegExp("\\.circuit$")))
		fname.append(".circuit");
}

FileIO::FileIO(Circuit *_circuit)
{
	circuit = _circuit;
	fname = "";
	circuit->initImport();
}

// read first line of file
int FileIO::checkFileFormat()
{	QString line1;
	QString line2;

	QFile f(fname);
	if (!f.open(QFile::ReadOnly)) return -1;

	QTextStream ts(&f);
	line1 = ts.readLine();
	line2 = ts.readLine();
	f.close();

	// XML file format types
	if (line1.contains("xml")) {
		if (line2.contains(XmlSymbolMap::m_oMap[XmlSymbol::KLOGIC_MAIN])) {
			if (line2.contains(XmlObject::ATT_SUBCIRCUIT))
				return XML_SUB;
			return XML_MAIN;
		}
	}

	if (line1.contains(XmlSymbolMap::m_oMap[XmlSymbol::KLOGIC_MAIN])) {
		if (line1.contains(XmlObject::ATT_SUBCIRCUIT))
			return XML_SUB;
		return XML_MAIN;
	}

	// klogic file format types
	if (7 == type(line1))
		return KLOGIC_SUB;
	return KLOGIC_MAIN;

	// unknown file format
	return -1;
}

void FileIO::setCircuit(Circuit *_circuit)
{
	circuit = _circuit;
	circuit->initImport();
}

void FileIO::setSubFilename()
{
	if (-1 == fname.indexOf(QRegExp("\\.sub\\.circuit$")))
		fname.replace( QRegExp("\\.circuit$"), ".sub.circuit" );
}

bool FileIO::write(bool subcircuit, bool selected, int dx, int dy)
{
	return writeXML(subcircuit, selected, dx, dy);
}

bool FileIO::writeXML(bool subcircuit, bool selected, int dx, int dy)
{
	if (!circuit) return false;
	if (fname.isEmpty()) return false;

	QFile f(fname);
	if (!f.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	writeXML(f, subcircuit, selected, dx, dy);

	f.flush();
	f.close();
	return true;
}

bool FileIO::writeXML(QIODevice& ioDev, bool subcircuit, bool selected, int dx, int dy)
{
	// write global circuit information
	XmlObject header(XmlSymbolMap::m_oMap[XmlSymbol::KLOGIC_MAIN]);

	// type
	if (subcircuit) header.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::FILE_TYPE], XmlObject::ATT_SUBCIRCUIT);
	else header.addAttribute(XmlSymbolMap::m_oMap[XmlSymbol::FILE_TYPE], XmlObject::ATT_CIRCUIT);

	// meta tags
	header.setVersion();
	if (KlogicGlobal::CurrentCircuit::creation.isEmpty()) header.setCreation();
	else header.setCreation(KlogicGlobal::CurrentCircuit::creation);
	header.setUpdate();
	header.setAuthor(KlogicGlobal::CurrentCircuit::author);
	header.setComment(KlogicGlobal::CurrentCircuit::comment);

	ioDev.write(XmlObject::XML_HEADER.toUtf8());
	ioDev.write(header.getFieldStart().toUtf8());
	ioDev.write(header.getFieldContent().toUtf8());

	// export the circuit
	bool ret = circuit->exportXML(ioDev, selected, 0, dx, dy);

	ioDev.write(header.getFieldEnd().toUtf8());

	return ret;
}

bool FileIO::readXML()
{
	if (!circuit) return false;
	if (fname.isEmpty()) return false;
	
	QFile f(fname);
	return readXML(f, false, 0, 0);
}

bool FileIO::readXML(QIODevice& ioDev, bool selected, int dx = 0, int dy = 0)
{
	return XMLImportHandler::import(circuit, selected, dx, dy, ioDev);
}

/**
 * This method reads the old format used in last decade or so. 
 */
bool FileIO::readCircuit(bool create_sub)
{
	if (!circuit) return false;
	if (fname.isEmpty()) return false;

	QFile f(fname);
	if (!f.open(QFile::ReadOnly)) return false;

	bool ret = readCircuit(create_sub, f, NO_SELECTION);

	f.close();
	return ret;
}

/**
 * This method reads the old format used in last decade or so. 
 */
bool FileIO::readCircuit(bool create_sub, QIODevice& ioDev, int select, Circuit *_circuit, int dx, int dy)
{
	int told=0, tnew=0;
	int sub = 0;
	int sub_is_main = 0;
	Circuit *__circuit;
	bool ret = true;
	bool retmsg = false;
	int event_cnt = 0;	// for better debugging

	if (!_circuit) _circuit = circuit;
	else {
		sub = 1;
		told = 1;
	}

	Circuit::magic_version = 0;
	while(!ioDev.atEnd()) {
		QString s = ioDev.readLine();
		if (s.isEmpty()) continue;

		// change status
		if (0 != (tnew = type(s))) {
			// avoid impossible status
			if ((tnew != told + 1) &&
			    (!(told == 1 && tnew == 3)) &&
			    (!(told == 6 && tnew == 9)) &&
			    (!(told == 8 && tnew == 7)) &&
			    (!(told == 0 && tnew == 7)) &&
			    (tnew != 8) && (tnew != 2)) {
				QString errmsg = "corrupt file content\nimpossible status change (told=";
				errmsg += QString::number(told);
				errmsg += " tnew=";
				errmsg += QString::number(tnew);
				errmsg += ")\n";
				qWarning() << errmsg << endl;
				return false;
			}
		}
		else tnew = told;

		// special case: subcircuit is the main circuit
		if (told == 0 && tnew == 7 && sub == 0) {
			tnew = 1;
			sub_is_main = 1;
		}

		// process current status
		if (told == tnew) {
			switch (tnew) {
			case 1:
				event_cnt++;
				if (create_sub) {
					// create sub circuit
					ret = ret && _circuit->importInstance(s, dx, dy);
					__circuit = _circuit->subCircuit(s);
					if (!__circuit) {
						qWarning("corrupt content\ncannot find sub-circuit");
						return false;
					}
					// (NO_SELECTION) do never select the content of a sub circuit
					ret = ret && readCircuit(0, ioDev, NO_SELECTION, __circuit);
					if (!ret && !retmsg) {
						qWarning("error during sub circuit creation");
						retmsg = true;
					}
					told = 8;
				} else {
					if (!Circuit::magic_version) {
						// find the very last Crtl-A
						int posa = s.lastIndexOf(1);
						if (posa != -1) {       // of course not!
							Circuit::magic_version = s.right(s.length() - posa - 1).toInt();
						}
					}
				}
				break;
			case 3:
				event_cnt++;
				ret = ret && _circuit->importWire(s, select, dx, dy);
				if (!ret && !retmsg) {
					qWarning("error during wire import creation");
					retmsg = true;
				}
				break;
			case 5:
				event_cnt++;
				ret = ret && _circuit->importDevice(s, select, dx, dy);
				if (!ret && !retmsg) {
					qWarning("error during device import creation");
					retmsg = true;
				}
				break;
			case 7:
				event_cnt++;
				__circuit = _circuit->subCircuit(s);
				if (!__circuit) {
					qWarning("corrupt content\ncannot find sub-circuit");
					return false;
				}
				// (NO_SELECTION) do never select the content of a sub circuit
				ret = ret && readCircuit(0, ioDev, NO_SELECTION, __circuit);
				if (!ret && !retmsg) {
					qWarning("error during sub circuit creation");
					retmsg = true;
				}
				told = 8;
				break;
			case 9:
				event_cnt++;
				ret = ret && _circuit->importWireConn(s, select, dx, dy);
				if (!ret && !retmsg) {
					qWarning("error during wire-wire connection");
					retmsg = true;
				}
				break;
			case 11:
				event_cnt++;
				ret = ret && _circuit->importDevConn(s, select, dx, dy);
				if (!ret && !retmsg) {
					qWarning("error during wire-dev connection");
					retmsg = true;
				}
				break;
			default:
				event_cnt++;
				qWarning("corrupt content\nunknown status while reading file");
				return false;
				break;
			}
		}
		// current status actually changed
		else {
			told = tnew;
			if (tnew == 2 || (tnew == 8 && sub_is_main)) {
				return ret;
			}
			if (tnew == 8) return ret;
		}
	}
	return ret;
}

/**
 * This method is used by the old format routines
 */
int FileIO::type(QString s)
{
	if (s.contains(IO_B_NET)) return 1;
	if (s.contains(IO_E_NET)) return 2;
	if (s.contains(IO_B_WIRE)) return 3;
	if (s.contains(IO_E_WIRE)) return 4;
	if (s.contains(IO_B_DEVICE)) return 5;
	if (s.contains(IO_E_DEVICE)) return 6;
	if (s.contains(IO_B_SUBNET)) return 7;
	if (s.contains(IO_E_SUBNET)) return 8;
	if (s.contains(IO_B_CONNWIRE)) return 9;
	if (s.contains(IO_E_CONNWIRE)) return 10;
	if (s.contains(IO_B_CONNDEVICE)) return 11;
	if (s.contains(IO_E_CONNDEVICE)) return 12;
	return 0;
}

