#ifndef __Wire_CLASS__
#define __Wire_CLASS__

#include <klogicList.h>
#include <device.h>
#include <qpixmap.h>
#include "xdevice.h"

/********************************************
* node handling of all nodes of a wire 
*********************************************/
class Wire
{
public:
	enum IO_TYPE {INPUT, OUTPUT, BOTH};
	enum WireStatus { UNCONNECTED, CONNECTED, SHORTCUT };

	Wire();
	Wire(QPoint);
	virtual ~Wire();

	void debug();

	// draw wire
	void draw(QPainter *, const QColor &);

	// called by XDevice
	// modify input- and output-nodes by devices (device-move)
	void setInputPosition(QPoint);
	void setInputPosition(QPoint, int);
	QPoint getInputPosition(int idx, int selonly = 0);
	void setOutputPosition(QPoint);
	void setOutputPosition(QPoint, int);
	QPoint getOutputPosition(int idx, int selonly = 0);
	// get io-position of connected wire
	QPoint getIOPosition(int idx, int selonly = 0);

	void garbageCollection();
	const QPoint & getActive();
	int activeIsInput();
	int activeIsOutput();
	int activeIsBorrowed();
	bool activeIsEnd();
	int countNodes();
	bool lockNode(QPoint);
	bool lockLastNode();
	bool lockSelectedNode();
	void updateNode(QPoint);	// update position of active node
	void releaseNode();

	// called by Selection
	int getMinSelX();
	int getMaxSelX();
	int getMinSelY();
	int getMaxSelY();
	void moveBy(const QPoint&);
	bool isSelected();

	static const int WFAIL = -1;

	// import/export mothods
	void setExportDelta(int dx, int dy);
	void setExportSelected(bool);
	QString operator>> (QString &);
	bool operator<< (const QString &);

	void setStatus(const WireStatus &newStatus);
	const Wire::WireStatus & getStatus() const;

protected:
	// draw grid into rect
	void drawConnections(QPainter *, IO_TYPE, int = 0);
	void restoreArry(QPainter *p, int, int, int, int);
	void drawSolderedPoint(QPainter *, QColor, QPoint);
	void drawInverterPoint(QPainter *, QColor, QPoint);

	// wire modifying methods
	KlogicList<QPoint> *contains(QPoint);
	bool lockNode(KlogicList<QPoint> *);
	bool lockNode(QPoint *);
	bool nodeIsCommissioned(QPoint *);
	int nodeIsInput(QPoint *);
	int nodeIsOutput(QPoint *);
	int nodeIsBorrowed(QPoint *);
	int nodeIsPart(QPoint *);

	// operations on active (locked) node
	bool createNode();		// creates a new node at the same position of current_node
	void cutNode();			// will not destroy it!
	void removeActiveNode();		// destroys active node
	QPoint *getActiveNode();

	// called by XWire: connection methods
	int connectInput(int);
	void disconnectInput(int);
	int connectOutput(int);
	void disconnectOutput(int);
	int connectWire(int);
	void disconnectWire(int);
	bool checkWireWireConnection(QPoint *);

	// wire-part selctions
	bool select(QRect, bool all = false);
	void unselect();

	// set wire size in pixels
	void setLineSize(int);

	virtual bool inputIsInverted(int) { return false; }
	virtual bool outputIsInverted(int) { return false; }

private:
	// *****************************************************
	// import/export
	// *****************************************************
	bool string2wire(const QString &);	// hist import
	int exp_dx;
	int exp_dy;
	bool exp_selected;

	// *****************************************************
	// connection detection methods
	// *****************************************************
	int derive(QPoint, QPoint, QPoint);
	int inBetween(QPoint, QPoint, QPoint);
	QPoint * align(QPoint *);

	// *****************************************************
	// connection management
	// *****************************************************
	// node of wire, which is input-node (input of a device)
	QPoint *m_poInputNode[2];

	// node of wire, which is output-node (output of a device)
	QPoint *m_poOutputNode[2];

	// connection nodes (wire-wire-connections, they must be deleted because they are owned. But make sure they are removed from foreign wires first!)
	QPoint *nodeBorrowedToOthers[2];

	// connection nodes of other wires (references, they must never be deleted by this!)
	KlogicList<QPoint> m_oForeignWireNodes;

	// actual locked node in list
	KlogicList<QPoint> *activeNodeL;

	// *****************************************************
	// the wire
	// *****************************************************
	// all nodes of this wire
	KlogicList<QPoint> m_oNodes;

	// all currenty selected nodes of this wire
	KlogicList<QPoint> m_oSelectedNodes;

	int m_iSize;
	WireStatus status;
};

#endif
