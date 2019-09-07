/***************************************/
/* class selection                     */
/*                                     */
/* holds information about a selection */
/* of wires/devices                    */
/*                                     */
/* Andreas Rostin                      */
/* 14.08.1999                          */
/***************************************/
#ifndef __SELECTIONCLASS
#define __SELECTIONCLASS

#include <qrect.h>
class QBuffer;

class Circuit;
#include <xdevice.h>
#include <xwire.h>
#include "klogicList.h"

// selection status
#define SEL_EMPTY	0
#define SEL_START	1
#define SEL_EXISTS	2
#define SEL_MOVE	3

class Selection
{
public:
	Selection();
	~Selection();

	int getStatus();
	void setCircuit(Circuit *);

	// single object selection
	void add(XDevice *);
	void remove(XDevice *);
	int contains(XDevice *);
	void add(XWire *);
	void remove(XWire *);
	int contains(XWire *);
	void addTemp(XDevice *);
	void removeTemp(XDevice *);
	void fixTemp(XDevice *);
	int isTemp(XDevice *);

	// group selection
	void beginFrame(QPoint);
	void changeFrame(QPoint);
	void endFrame();
	int onFrame(QPoint);

	// moving selection
	void beginMove(QPoint);
	void moveFrame(QPoint);
	void endMove();
	int hasMoved();

	// copy selection
	void cut(Circuit *);
	void copy(Circuit *);
	void at(QPoint);
	void paste(Circuit *);

	// remove selection
	void remove(bool);
	void remove();

	// drawing methods (for the selection frame, not the content)
	void draw(QPainter *);

private:
	inline void setFrame(QPoint);
	inline void setFrame();
	inline QPoint *align(QPoint *);

	class UnsignedRect: public QRect
	{
	public:
		static void align(QPoint& p)
		{
			if (p.x() < 0)
				p.setX(0);
			if (p.y() < 0)
				p.setY(0);
		}

		void setX(int x)
		{
			int oldw = width();
			if (x < 0)
				QRect::setX(0);
			else
				QRect::setX(x);
			setWidth(oldw);
		}

		void setY(int y)
		{
			int oldh = height();
			if (y < 0)
				QRect::setY(0);
			else
				QRect::setY(y);
			setHeight(oldh);
		}
	};

	QPoint corner1;
	UnsignedRect frame;
	QPoint pt_move_start;

	// current status of a group selection, 0: begin must follow, 1: move or end must follow
	int selstat;

	int frame_moved;
	XDevice *tempdev;

	// objects a selection can contain
	KlogicList<XDevice> dev;
	KlogicList<XWire> wire;

	// buffer which contains the last pasted circuit
	static QBuffer copy_buffer;
	// position to paste at
	QPoint paste_at;

	Circuit *m_poCircuit;
};

#endif
