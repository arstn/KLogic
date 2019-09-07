/*************************************************/
/* methods for class Selection                   */
/*                                               */
/* holds information about a selection           */
/* of wires/devices made in a CircuitWidget class*/
/*                                               */
/* Andreas Rostin                                */
/* 14.08.1999                                    */
/*************************************************/
#include <stdio.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qbuffer.h>

#include <qpen.h>
#include "FileIO.h"
#include "selection.h"
#include "KlogicGlobal.h"
#include "circuit.h"
#include "grid.h"

/***************************************************/
/* methods of Selection                            */
/***************************************************/

QBuffer Selection::copy_buffer;

Selection::Selection()
{
	m_poCircuit = 0;
	selstat = SEL_EMPTY;
	frame_moved = 0;
	tempdev = (XDevice *)NULL;
	frame.setX(0);
	frame.setWidth(0);
	frame.setY(0);
	frame.setHeight(0);
	paste_at.setX(20);
	paste_at.setY(20);
}

Selection::~Selection()
{
	KlogicList<XDevice> *ld;
	KlogicList<XWire> *lw;

	ld = dev.First();
	while(ld) {
		ld->Get()->select(false);
		ld = ld->Next();
	}
	dev.Destroy();

	lw = wire.First();
	while(lw) {
		lw->Get()->select(false);
		lw = lw->Next();
	}
	wire.Destroy();
}

// set parent poCircuit for a selection
void Selection::setCircuit(Circuit *poCircuit)
{
	m_poCircuit = poCircuit;
}

// add a device to the current selection
void Selection::add(XDevice *newdev)
{
	if (!dev.With(newdev)) dev.Append(newdev);
	newdev->select(true);
	selstat = SEL_EXISTS;
	setFrame();
}

// remove a device from the current selection
void Selection::remove(XDevice *olddev)
{
	dev.Destroy(olddev);
	olddev->select(false);
	if (dev.counter()==0 && wire.counter() == 0) remove();
	setFrame();
}

// remove a wire from the current selection
int Selection::contains(XDevice *adev)
{
	if (dev.With(adev)) return 1;
	return 0;
}

// add a device temporary to the current selection
void Selection::addTemp(XDevice *newdev)
{
	if (!dev.With(newdev)) dev.Append(newdev);
	tempdev = newdev;
	newdev->select(true);
	selstat = SEL_EXISTS;
	setFrame();
}

// remove a device from the current selection
void Selection::removeTemp(XDevice *olddev)
{
	if (tempdev != olddev) return;
	dev.Destroy(olddev);
	olddev->select(false);
	if (dev.counter()==0 && wire.counter() == 0) remove();
	setFrame();
	tempdev = (XDevice *)NULL;
}

// return if a device is temporarily in the selection
int Selection::isTemp(XDevice *adev)
{
	if (tempdev == adev) return 1;
	return 0;
}

// add a temp device to the selection
void Selection::fixTemp(XDevice *adev)
{
	if (tempdev == adev) tempdev = (XDevice *)NULL;
}

// add a wire to the current selection
void Selection::add(XWire *newwire)
{
	if (!wire.With(newwire)) wire.Append(newwire);
	newwire->select(true);
	selstat = SEL_EXISTS;
	setFrame();
}

// remove a wire from the current selection
void Selection::remove(XWire *oldwire)
{
	wire.Destroy(oldwire);
	oldwire->select(false);
	if (dev.counter()==0 && wire.counter() == 0) remove();
	setFrame();
}

// remove a wire from the current selection
int Selection::contains(XWire *awire)
{
	if (wire.With(awire)) return 1;
	return 0;
}

// start a group selection: fix corner 1
void Selection::beginFrame(QPoint newp)
{
	if (selstat != SEL_EMPTY && selstat != SEL_EXISTS) return;

	corner1.setX(newp.x());
	corner1.setY(newp.y());

	align(&corner1);

	setFrame(corner1);

	m_poCircuit->selectDev(frame, &dev);
	m_poCircuit->selectWire(frame, &wire);

	selstat = SEL_START;
}

// change corner 2 of a group selection
void Selection::changeFrame(QPoint newp)
{
	if (selstat != SEL_START) return;

	setFrame(newp);

	m_poCircuit->selectDev(frame, &dev);
	m_poCircuit->selectWire(frame, &wire);
}

// stop changing the group selection: make selection
void Selection::endFrame()
{
	if (selstat != SEL_START) return;
	selstat = SEL_EXISTS;
}

// if point lies on the frame return 1, otherwise 0
int Selection::onFrame(QPoint pt)
{	int xmin = pt.x() - Grid::GRID / 2;
	int xmax = pt.x() + Grid::GRID / 2;
	int ymin = pt.y() - Grid::GRID / 2;
	int ymax = pt.y() + Grid::GRID / 2;

	if (pt.x() >= frame.x() && pt.x() <= frame.x() + frame.width()) {
		if (frame.y() <= ymax && frame.y() >= ymin ) return 1;
		if (frame.y() + frame.height() <= ymax && frame.y() + frame.height() >= ymin) return 1;
	}
	if (pt.y() >= frame.y() && pt.y() <= frame.y() + frame.height()) {
		if (frame.x() <= xmax && frame.x() >= xmin) return 1;
		if (frame.x() + frame.width() <= xmax && frame.x() + frame.width() >= xmin) return 1;
	}
	return 0;
}

// start moving frame
void Selection::beginMove(QPoint pt)
{
	if (selstat != SEL_EXISTS) return;

	pt_move_start = pt;

	selstat = SEL_MOVE;
	frame_moved = 0;
}

// move frame with its content
void Selection::moveFrame(QPoint pt)
{	KlogicList<XDevice> *ld;
	KlogicList<XWire> *lw;

	if (selstat != SEL_MOVE) return;

	// ** calculate delta position
	int dx = pt.x() - pt_move_start.x();
	int dy = pt.y() - pt_move_start.y();

	if (dx + frame.x() < 0)
		dx = 0 - frame.x();
	if (dx + frame.x() + frame.width() > KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X)
		dx = KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X - frame.x() - frame.width();
	if (dy + frame.y() < 0)
		dy = 0 - frame.y();
	if (dy + frame.y()  + frame.height() > KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y)
		dy = KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y - frame.y() - frame.height();

	dx = dx / Grid::GRID * Grid::GRID;
	dy = dy / Grid::GRID * Grid::GRID;

	if (dx == 0 && dy == 0) return;

	// ** move wires
	lw = wire.First();
	while(lw) {
		lw->Get()->moveBy(QPoint(dx, dy));
		lw = lw->Next();
	}

	// ** move devices
	ld = dev.First();
	while(ld) {
		ld->Get()->moveBy(QPoint(dx, dy));
		ld = ld->Next();
	}

	// ** move frame
	frame.setX(dx + frame.x());
	frame.setY(dy + frame.y());

	// set new start point for further moves
	pt_move_start.setX(dx + pt_move_start.x());
	pt_move_start.setY(dy + pt_move_start.y());

	frame_moved = 1;
}

// stop moving frame
void Selection::endMove()
{
	if (selstat != SEL_MOVE) return;
	selstat = SEL_EXISTS;
}

// cut selection
void Selection::cut(Circuit *poCircuit)
{
	if (selstat != SEL_EXISTS) return;

	// ** first copy the current selection into the copy-buffer
	copy(poCircuit);

	// ** now remove the objects from the poCircuit
	KlogicList<XDevice> *ld = dev.First();
	while(ld) {
		poCircuit->deleteDevice(ld->Get());
		ld = ld->Next();
	}

	KlogicList<XWire> *lw = wire.First();
	while(lw) {
		poCircuit->deleteWire(lw->Get());
		lw = lw->Next();
	}

	// ** remove current selection
	remove(FALSE);
}

// copy selection into the copy-buffer
// copy_buffer contains a normal klogic file format
void Selection::copy(Circuit *poCircuit)
{
	if (selstat != SEL_EXISTS)
		return;

	int minx = 999999, miny = 999999;

	// detemine minimum coodinates of the selection
	KlogicList<XDevice> *ld = dev.First();
	while(ld) {
		QPoint oPos = ld->Get()->getPos();
		if (oPos.x() < minx) minx = oPos.x();
		if (oPos.y() < miny) miny = oPos.y();
		ld = ld->Next();
	}
	KlogicList<XWire> *lw = wire.First();
	while(lw) {
		int a = lw->Get()->getMinSelX();
		if (a < minx) minx = a;
		a = lw->Get()->getMinSelY();
		if (a < miny) miny = a;
		lw = lw->Next();
	}
	// align these points
	minx = (minx + Grid::GRIDHALF) / Grid::GRID * Grid::GRID;
	miny = (miny + Grid::GRIDHALF) / Grid::GRID * Grid::GRID;

	// export objects relative to minx/miny as zero-coord.
	copy_buffer.open(QBuffer::WriteOnly|QBuffer::Truncate);
	FileIO io(poCircuit);

	// save as main circuit: ignore the very first circuit
	io.writeXML(copy_buffer, false, true, minx, miny);

	copy_buffer.close();
}

// set position to paste to
void Selection::at(QPoint pt)
{
	paste_at.setX(pt.x() / Grid::GRID * Grid::GRID);
	paste_at.setY(pt.y() / Grid::GRID * Grid::GRID);
}

// paste selection from the copy buffer
void Selection::paste(Circuit *poCircuit)
{
	// ** remove the current selection
	remove(TRUE);

	// ** paste content of the copy_buffer
	// ++ all pasted objects automaticly become status "selected"
	FileIO io(poCircuit);
	io.readXML(copy_buffer, SELECT_IT, paste_at.x(), paste_at.y());

	poCircuit->grabSelection(this);	// something like recursive call to this->add(..)
	if (dev.counter()==0 && wire.counter() == 0) remove();
	else selstat = SEL_EXISTS;
	setFrame();
}

// set dimension of the group selection frame (right lower corner)
inline void Selection::setFrame(QPoint corner2)
{
	align(&corner2);

	frame.setX(corner1.x());
	frame.setWidth(corner2.x() - corner1.x() + 1);
	frame.setY(corner1.y());
	frame.setHeight(corner2.y() - corner1.y() + 1);
	if (frame.width() < 0) {
		frame.setX(corner2.x());
		frame.setWidth(corner1.x() - corner2.x() + 1);
	}
	if (frame.height() < 0) {
		frame.setY(corner2.y());
		frame.setHeight(corner1.y() - corner2.y() + 1);
	}
}

// set dimension of the group selection frame in dependance of all selected objects
inline void Selection::setFrame()
{
	if (selstat == SEL_EMPTY) return;

	int minx = 999999, miny = 999999;
	int maxx = 0, maxy = 0;

	// detemine minimum/maximum coodinates of the selection
	KlogicList<XDevice> *ld = dev.First();
	while(ld) {
		const QRect& oGeo = ld->Get()->getGeometry();
		if (oGeo.x() < minx) minx = oGeo.x();
		if (oGeo.x() + oGeo.width() > maxx) maxx = oGeo.x() + oGeo.width();
		if (oGeo.y() < miny) miny = oGeo.y();
		if (oGeo.y() + oGeo.height() > maxy) maxy = oGeo.y() + oGeo.height();
		ld = ld->Next();
	}
	KlogicList<XWire> *lw = wire.First();
	while(lw) {
		int a = lw->Get()->getMinSelX();
		if (a < minx) minx = a;
		a = lw->Get()->getMaxSelX();
		if (a > maxx) maxx = a;
		a = lw->Get()->getMinSelY();
		if (a < miny) miny = a;
		a = lw->Get()->getMaxSelY();
		if (a > maxy) maxy = a;
		lw = lw->Next();
	}
	// align min/max coord.
	minx = (minx + Grid::GRIDHALF) / Grid::GRID * Grid::GRID;
	maxx = (Grid::GRIDDOUBLE + maxx + Grid::GRIDHALF) / Grid::GRID * Grid::GRID;
	miny = (miny + Grid::GRIDHALF) / Grid::GRID * Grid::GRID;
	maxy = (Grid::GRIDDOUBLE + maxy + Grid::GRIDHALF) / Grid::GRID * Grid::GRID;

	// set frame
	if (minx > 10) frame.setX(minx - Grid::GRID);
	else frame.setX(minx);
	frame.setWidth(maxx - minx + 1);
	if (miny > 10) frame.setY(miny - Grid::GRID);
	else frame.setY(miny);
	frame.setHeight(maxy - miny + 1);
}

inline QPoint * Selection::align(QPoint *p)
{       p->setX((p->x() + Grid::GRIDHALF) / Grid::GRID * Grid::GRID);
        p->setY((p->y() + Grid::GRIDHALF) / Grid::GRID * Grid::GRID);
        return p;
}

int Selection::getStatus()
{
	return selstat;
}

int Selection::hasMoved()
{
	return frame_moved;
}

// draw selection frame
void Selection::draw(QPainter *p)
{
	QPen pen(Qt::black, 0, Qt::DashLine);
	p->setPen(pen);
	p->setBrush(Qt::NoBrush);
	p->drawRect(frame);
}

// remove selection, go to initial status
// (leaves copy_buffer in place)
void Selection::remove(bool unselect)
{
	if (unselect) {
		// ** unselect wires
		KlogicList<XWire> *lw = wire.First();
		while(lw) {
			lw->Get()->select(0);
			lw = lw->Next();
		}

		// ** unselect devices
		KlogicList<XDevice> *ld = dev.First();
		while(ld) {
			ld->Get()->select(0);
			ld = ld->Next();
		}
	}

	// ** selection clean up
	remove();
}

void Selection::remove()
{
	dev.Destroy();
	wire.Destroy();
	selstat = SEL_EMPTY;
	frame_moved = 0;
	tempdev = (XDevice *)NULL;
	frame.setX(0);
	frame.setWidth(0);
	frame.setY(0);
	frame.setHeight(0);
}

