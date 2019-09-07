#ifndef __KLOGICIO_CLASS__
#define __KLOGICIO_CLASS__

class QTextStream;
class Circuit;
class QString;

// constants for reading/writing files
#define IO_B_NET "BEGIN NET"
#define IO_E_NET "END NET"
#define IO_B_WIRE "BEGIN WIRE"
#define IO_E_WIRE "END WIRE"
#define IO_B_DEVICE "BEGIN DEVICE"
#define IO_E_DEVICE "END DEVICE"
#define IO_B_SUBNET "BEGIN SUBNET"
#define IO_E_SUBNET "END SUBNET"
#define IO_B_CONNWIRE "BEGIN CONNECTION WIRES"
#define IO_E_CONNWIRE "END CONNECTION WIRES"
#define IO_B_CONNDEVICE "BEGIN CONNECTION DEVICES"
#define IO_E_CONNDEVICE "END CONNECTION DEVICES"

/*********************************************
* read/write a circuit
*********************************************/
class FileIO
{
public:
	FileIO(Circuit *);
	FileIO(QString);
	FileIO(QString, Circuit *);

	bool write(bool, bool, int, int);
	bool writeXML(bool, bool, int, int);
	bool writeXML(QIODevice&, bool, bool, int, int);
	bool readXML();
	bool readXML(QIODevice&, bool, int, int);

	// old format
	bool readCircuit(bool);
	bool readCircuit(bool, QIODevice&, int, Circuit *_net = (Circuit *)NULL, int dx = 0, int dy = 0);

	void setSubFilename();
	void setCircuit(Circuit *);

	static const int XML_SUB = 1;
	static const int XML_MAIN = 2;
	static const int KLOGIC_SUB = 3;
	static const int KLOGIC_MAIN = 4;
	int checkFileFormat();

	static const int SAVE_XML = 1;
	static const int SAVE_KLOGIC = 2;
	static void setSaveFileFormat(int);
	static int getSaveFileFormat();
private:
	int type(QString);

	QString fname;
	Circuit *circuit;
	static int file_format;

};

#endif

