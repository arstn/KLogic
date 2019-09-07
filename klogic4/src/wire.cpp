/*************************************************/
/* methods for class Wire                        */
/*                                               */
/* managing the nodes of a wire                  */
/*                                               */
/* Andreas Rostin                                */
/* 15.03.99                                      */
/*************************************************/
#include <qpainter.h>
#include <qpen.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "xwire.h"
#include "KlogicGlobal.h"
#include "grid.h"

/***************************************************/
/* methods of Wire                                 */
/***************************************************/
Wire::Wire()
{
    activeNodeL = NULL;
    status = UNCONNECTED;
    m_poInputNode[0] = (QPoint *)NULL;
    m_poInputNode[1] = (QPoint *)NULL;
    m_poOutputNode[0] = (QPoint *)NULL;
    m_poOutputNode[1] = (QPoint *)NULL;
    nodeBorrowedToOthers[0] = (QPoint *)NULL;
    nodeBorrowedToOthers[1] = (QPoint *)NULL;
    m_iSize = 1;
    exp_selected = false;
    exp_dx = exp_dy = 0;
}

Wire::Wire(QPoint src)
{
    align(&src);
    QPoint *p = new QPoint(src);
    m_oNodes.Append(p);
    lockNode(p);
    status = UNCONNECTED;
    m_poInputNode[0] = (QPoint *)NULL;
    m_poInputNode[1] = (QPoint *)NULL;
    m_poOutputNode[0] = (QPoint *)NULL;
    m_poOutputNode[1] = (QPoint *)NULL;
    nodeBorrowedToOthers[0] = (QPoint *)NULL;
    nodeBorrowedToOthers[1] = (QPoint *)NULL;
    m_iSize = 1;
    exp_selected = false;
    exp_dx = exp_dy = 0;
}

Wire::~Wire()
{
    KlogicList<QPoint> *l = m_oNodes.First();
    while (l) {
        if (!m_oForeignWireNodes.With(l->Get())) {
            delete l->Get();
	}
        l = l->Next();
    }
    m_oNodes.Destroy();
    m_oForeignWireNodes.Destroy();
    m_oSelectedNodes.Destroy();
}

void Wire::setStatus(const WireStatus &newStatus) {
	status = newStatus;
}

const Wire::WireStatus & Wire::getStatus() const {
	return status;
}


// node clicked? then lock the node
KlogicList<QPoint> *Wire::contains(QPoint p)
{
    // all wire nodes are aligned to the grid
    align(&p);
    
    KlogicList<QPoint> *candidate = NULL;
    KlogicList<QPoint> *l = m_oNodes.First();
    while (l) {
        if (*(l->Get()) == p) {
        	if (nodeIsInput(l->Get()) != WFAIL || nodeIsOutput(l->Get()) != WFAIL || nodeIsBorrowed(l->Get()) != WFAIL) {
        		candidate = l;	/// for import purposes
        	} else {
        		lockNode(l);
        		return l;
        	}
        }
        l = l->Next();
    }
    lockNode(candidate);
    return candidate;
}

int Wire::nodeIsPart(QPoint *pt)
{
    if (m_oNodes.With(pt)) return 1;
    return 0;
}

// (selection method) select nodes contained in rect, or select all nodes if all is true
bool Wire::select(QRect r, bool all)
{
    KlogicList<QPoint> *l = m_oNodes.First();
    bool ret = false;

    while (l) {
        if (all || r.contains(*l->Get())) {
            if (!m_oSelectedNodes.With(l->Get())) m_oSelectedNodes.Append(l->Get());
            ret = true;
        } else {
            m_oSelectedNodes.Destroy(l->Get());
        }
        l = l->Next();
    }
    return ret;
}

// (selection method) unselect all nodes
bool Wire::isSelected()
{
    if (m_oSelectedNodes.First()) return true;
    return false;
}

// (selection method) unselect all nodes
void Wire::unselect()
{
    m_oSelectedNodes.Destroy();
}

// return minimum x cood over all selected nodes
int Wire::getMinSelX()
{
    KlogicList<QPoint> *l = m_oSelectedNodes.First();
    int ret;

    if (!l) return 0;
    else ret = l->Get()->x();

    while (l) {
        if (l->Get()->x() < ret) ret = l->Get()->x();
        l = l->Next();
    }
    return ret;
}

// return minimum x cood over all selected nodes
int Wire::getMaxSelX()
{
    KlogicList<QPoint> *l = m_oSelectedNodes.First();
    int ret;

    if (!l) return 0;
    else ret = l->Get()->x();

    while (l) {
        if (l->Get()->x() > ret) ret = l->Get()->x();
        l = l->Next();
    }
    return ret;
}

// return minimum y cood over all selected nodes
int Wire::getMinSelY()
{
    KlogicList<QPoint> *l = m_oSelectedNodes.First();
    int ret;

    if (!l) return 0;
    else ret = l->Get()->y();

    while (l) {
        if (l->Get()->y() < ret) ret = l->Get()->y();
        l = l->Next();
    }
    return ret;
}

// return minimum y cood over all selected nodes
int Wire::getMaxSelY()
{
    KlogicList<QPoint> *l = m_oSelectedNodes.First();
    int ret;

    if (!l) return 0;
    else ret = l->Get()->y();

    while (l) {
        if (l->Get()->y() > ret) ret = l->Get()->y();
        l = l->Next();
    }
    return ret;
}

// set size of the wire line
// only set by XWire
void Wire::setLineSize(int iSize)
{
    if (m_iSize != iSize) {
        m_iSize = iSize;
    }
}

bool Wire::lockSelectedNode()
{
    activeNodeL = NULL;

    if (m_oSelectedNodes.First()) {
      lockNode(m_oSelectedNodes.First()->Get());
    }

    return activeNodeL != NULL;
}

// move all selected nodes
void Wire::moveBy(const QPoint& oDelta)
{
    KlogicList<QPoint> *l = m_oSelectedNodes.First();
    QPoint *pt;

    while (l) {
        pt = l->Get();
        // do not move nodes connected to a device or referenced nodes
        if (nodeIsInput(pt) == WFAIL && nodeIsOutput(pt) == WFAIL && !nodeIsBorrowed(pt)) {
            *pt += oDelta;
        }
        l = l->Next();
    }
}

// creates new node, if not i/o and not end-node
// otherwise locks the node
bool Wire::createNode()
{
      // new node, if no successor and not connected, otherwise existing node
      if (activeNodeL->Prev() && !activeNodeL->Next()) {
	  if (activeIsInput() == WFAIL && activeIsOutput() == WFAIL && activeIsBorrowed() == WFAIL) {
	      return lockNode(m_oNodes.Append(new QPoint(*activeNodeL->Get())));
	  }
      }

      // new node, if no successor and not connected, otherwise existing node
      if (!activeNodeL->Prev() && activeNodeL->Next()) {
	  if (activeIsInput() == WFAIL && activeIsOutput() == WFAIL && activeIsBorrowed() == WFAIL) {
	      return lockNode(m_oNodes.Insert(new QPoint(*activeNodeL->Get()), 0, activeNodeL->Get()));
	  }
      }

      // new node, if m_oWireNodes with single node
      if (!activeNodeL->Prev() && !activeNodeL->Next()) {
	  return lockNode(m_oNodes.Append(new QPoint(*activeNodeL->Get())));
      }

      // somewhat in between the wire..
      return FALSE;
}

// locks point by content: used for import only!
// tries only to lock the first and the last node!
bool Wire::lockNode(QPoint pt)
{
    KlogicList<QPoint> *f = m_oNodes.First();
    QPoint *fp = f->Get();
    if (*fp == pt && WFAIL == nodeIsBorrowed(fp)) {
        return lockNode(f);
    }

    KlogicList<QPoint> *l = m_oNodes.Last();
    QPoint *lp = l->Get();
    if (*lp == pt && WFAIL == nodeIsBorrowed(lp)) {
        return lockNode(l);
    }

    return FALSE;
}

// lock a node
bool Wire::lockNode(QPoint *pt)
{
    return lockNode(m_oNodes.With(pt));
}

// lock a node in the list of nodes
bool Wire::lockNode(KlogicList<QPoint> *l) {
    if (l != NULL) {
      qDebug() << this << "node locked: " << l->Get() << *l->Get() << \
    		  " (in=" << (nodeIsInput(l->Get()) != WFAIL) << \
    		   " out=" << (nodeIsOutput(l->Get()) != WFAIL) << \
    		   " owner=" << (nodeIsBorrowed(l->Get()) != WFAIL) << ")";
    } else {
      qDebug() << this << "node locked: NULL";
    }

    activeNodeL = l;
    return activeNodeL != NULL;
}

// lock the start node of the wire
bool Wire::lockLastNode()
{
    return lockNode(m_oNodes.First());
}

// remove the node from the list
// (does not destroy the node)
void Wire::cutNode()
{
    if (activeNodeL) {
		int wireWireConIdx = activeIsBorrowed();

		m_oNodes.Destroy(activeNodeL->Get());
		m_oForeignWireNodes.Destroy(activeNodeL->Get());
		m_oSelectedNodes.Destroy(activeNodeL->Get());
		if (wireWireConIdx != WFAIL) {
		  nodeBorrowedToOthers[wireWireConIdx] = NULL;
		}

		lockNode((KlogicList<QPoint> *)NULL);
    }
}

// remove the node from the list
// (destroys the node)
void Wire::removeActiveNode()
{
    if (activeNodeL) {
		QPoint *node = activeNodeL->Get();
		bool foreignNode = m_oForeignWireNodes.With(node);
		int wireWireConIdx = activeIsBorrowed();
	
		m_oNodes.Destroy(node);
		m_oForeignWireNodes.Destroy(node);
		m_oSelectedNodes.Destroy(node);

		if (wireWireConIdx != WFAIL) {
		  qDebug() << this << "(removeNode) Borrowed node - not anymore " << node;
		  nodeBorrowedToOthers[wireWireConIdx] = NULL;
		}

		if (!foreignNode) {
		  qDebug() << this << "(removeNode) Deleting own node " << node;
		  delete node;
		} else {
		  qDebug() << this << "(removeNode) Removing foreign node " << node;
		}
        lockNode((KlogicList<QPoint> *)NULL);
    }
}

void Wire::releaseNode()
{
    lockNode((KlogicList<QPoint> *)NULL);
}

int Wire::countNodes()
{
    return m_oNodes.counter();
}

// check connection between wires
// given point lies near a node or a line of this wire
// if connection required, connect it (insert ref, lock node, set input/output)
bool Wire::checkWireWireConnection(QPoint *pt)
{
    // first of all check if point lies in a point of this wire (Used for import only, foreign/own info is not stored)
    KlogicList<QPoint> *pointInThisWireL = contains(*pt);
    if (pointInThisWireL) {
    	QPoint *ownNode = pointInThisWireL->Get();
    	// from now own do not use the list pointer again

        // never connect input- or output nodes!
        if (nodeIsInput(ownNode) != WFAIL || nodeIsOutput(ownNode) != WFAIL) {
            return FALSE;
        }

		// insert foreign node, append foreign one before the own node
        m_oNodes.Insert(pt, 0, ownNode);
		// remove the own node
        lockNode(ownNode);
        removeActiveNode();
        // mark the new node as foreign
        m_oForeignWireNodes.Append(pt);
        // lock the new node
        return lockNode(pt);
    }

    // check each line..
    KlogicList<QPoint> *l1 = m_oNodes.First();
    KlogicList<QPoint> *l2 = m_oNodes.First()->Next();
    while (l1 && l2) {
        // look if line goes through the given point
        int der_eq = derive(*l1->Get(), *l2->Get(), *pt);
        // if so, look for coordinates..
        if (der_eq) {
            int inbetw_eq = inBetween(*l1->Get(), *l2->Get(), *pt);
            if (inbetw_eq) {
                // insert foreign node, append pt between the existing two own nodes
                KlogicList<QPoint> *inserted = m_oNodes.Insert(pt, 0, l2->Get());
                m_oForeignWireNodes.Append(pt);
                return lockNode(inserted);
            }
        }
        l1 = l2;
        l2 = l2->Next();
    }
    return FALSE;
}

// internal: derivation pt1:pt2 ~= pt1:pt?
int Wire::derive(QPoint pt1, QPoint pt2, QPoint pt)
{
    double d1, d2, alpha, beta;

    long dx1, dy1, dx2, dy2;

    // reduce influence of node-distance by factor for pt1
    dx1 = pt1.x() - pt2.x();
    dy1 = pt1.y() - pt2.y();
    dx2 = pt1.x() + dx1 - pt.x();
    dy2 = pt1.y() + dy1 - pt.y();

    if (dy1 == 0) d1 = 0.0;
    else if (dx1 != 0) d1 = (double)dy1 / (double)dx1;
    else d1 = 999999.0;
    if (dy2 == 0) d2 = 0.0;
    else if (dx2 != 0) d2 = (double)dy2 / (double)dx2;
    else d2 = 999999.0;

    alpha = atan(d1);
    beta = atan(d2);

    if ((alpha >= beta - 0.05) && (alpha <= beta + 0.05)) return 1;
    return 0;
}

// internal: point between line(-rect)?
int Wire::inBetween(QPoint pt1, QPoint pt2, QPoint pt)
{
    QRect r;

    if (pt1.x() > pt2.x()) {
        r.setX(pt2.x());
        r.setWidth(pt1.x() - pt2.x());
    }
    else {
        r.setX(pt1.x() - 1);
        r.setWidth(pt2.x() - pt1.x() + 2);
    }
    if (pt1.y() > pt2.y()) {
        r.setY(pt2.y());
        r.setHeight(pt1.y() - pt2.y());
    }
    else {
        r.setY(pt1.y() - 1);
        r.setHeight(pt2.y() - pt1.y() + 2);
    }
    if (r.contains(pt)) return 1;
    return 0;
}

const QPoint & Wire::getActive()
{
    return *activeNodeL->Get();
}

QPoint * Wire::getActiveNode()
{
    return activeNodeL->Get();
}

// returns idx, if active node is the input-node
int Wire::activeIsInput()
{
    return nodeIsInput(activeNodeL->Get());
}

// returns idx, if active node is the input-node
int Wire::nodeIsInput(QPoint *pt)
{
    if (!pt) return WFAIL;
    if (pt == m_poInputNode[0]) return 0;
    if (pt == m_poInputNode[1]) return 1;
    return WFAIL;
}

// returns idx, if active node is the output-node
int Wire::activeIsOutput()
{
    return nodeIsOutput(activeNodeL->Get());
}

// returns idx, if active node is the output-node
int Wire::nodeIsOutput(QPoint *pt)
{
    if (!pt) return WFAIL;
    if (pt == m_poOutputNode[0]) return 0;
    if (pt == m_poOutputNode[1]) return 1;
    return WFAIL;
}

// returns index of active node if it is inserted into another wire, there as a foreign node
int Wire::activeIsBorrowed()
{
	return nodeIsBorrowed(activeNodeL->Get());
}

// returns index of node if it is inserted into another wire, there as a foreign node
int Wire::nodeIsBorrowed(QPoint *pt)
{
    if (!pt) return WFAIL;
    if (pt == nodeBorrowedToOthers[0]) return 0;
    if (pt == nodeBorrowedToOthers[1]) return 1;
    return WFAIL;
}

// returns true, if active node is start- or end-point of wire
bool Wire::activeIsEnd()
{
    return (activeNodeL == m_oNodes.First() ||
            activeNodeL == m_oNodes.Last());
}

// draw wire
void Wire::draw(QPainter *p, const QColor &color)
{
    KlogicList<QPoint> *l = m_oNodes.First();
    bool bColorSet = false;

    p->setPen(QPen(color, m_iSize));
    while (l && l->Get() && l->Next() && l->Next()->Get()) {
        // draw selected nodes green
        if (!bColorSet && (m_oSelectedNodes.With(l->Get()) || m_oSelectedNodes.With(l->Next()->Get()))) {
			bColorSet = true;
			p->setPen(QPen(Qt::green, m_iSize));
        } else if (bColorSet) {
            bColorSet = false;
            p->setPen(QPen(color, m_iSize));
        }
        p->drawLine(*(l->Get()), *(l->Next()->Get()));
        l = l->Next();
    }
    if (nodeBorrowedToOthers[0])
        drawSolderedPoint(p, Qt::black, *nodeBorrowedToOthers[0]);
    if (nodeBorrowedToOthers[1])
        drawSolderedPoint(p, Qt::black, *nodeBorrowedToOthers[1]);

    drawConnections(p, BOTH);
}

void Wire::drawConnections(QPainter *p, IO_TYPE iType, int idx)
{
    if (iType == INPUT) {
        if (m_poInputNode[idx]) {
            QPoint oEndP(*(m_poInputNode[idx]) + QPoint(4,0));
            p->drawLine(*(m_poInputNode[idx]), oEndP);
            if (inputIsInverted(idx)) {
                QPoint oInvP(*(m_poInputNode[idx]) + QPoint(3,0));
                drawInverterPoint(p, Qt::black, oInvP);
            }
        }
    } else if (iType == OUTPUT) {
        if (m_poOutputNode[idx]) {
            QPoint oEndP(*(m_poOutputNode[idx]) + QPoint(-4,0));
            p->drawLine(*(m_poOutputNode[idx]), oEndP);
            if (outputIsInverted(idx)) {
                QPoint oInvP(*(m_poOutputNode[idx]) + QPoint(-1,0));
                drawInverterPoint(p, Qt::black, oInvP);
            }
        }
    } else if (iType == BOTH) {
        drawConnections(p, INPUT, 0);
        drawConnections(p, INPUT, 1);
        drawConnections(p, OUTPUT, 0);
        drawConnections(p, OUTPUT, 1);
    }
}

// draw connection between wires
void Wire::drawSolderedPoint(QPainter *p, QColor oColor, QPoint pt)
{
    p->setPen(QPen(oColor, m_iSize));
    p->setBrush(oColor);
    p->drawPie(pt.x() - 2, pt.y() - 2, 4, 4, 0, 5760);
}

// draw connection between wires
void Wire::drawInverterPoint(QPainter *p, QColor oColor, QPoint pt)
{
    p->setPen(QPen(oColor, m_iSize));
    p->setBrush(oColor);
    p->drawPie(pt.x() - 3, pt.y() - 3, 6, 6, 0, 5760);
}

void Wire::updateNode(QPoint p)
{
    align(&p);

    if (!activeNodeL) qFatal(NOACTIVE);
    *activeNodeL->Get() = p;
}

QPoint *Wire::align(QPoint *p)
{
    p->setX((p->x() + Grid::GRIDHALF) / Grid::GRID * Grid::GRID);
    p->setY((p->y() + Grid::GRIDHALF) / Grid::GRID * Grid::GRID);
    return p;
}

// remove useless nodes
void Wire::garbageCollection()
{
    KlogicList<QPoint> *lp = m_oNodes.First();
    QPoint *last = (QPoint *)NULL;
    QPoint *act = (QPoint *)NULL;
    int modified = 0;
    int flag1, flag3, flag4;

    while (lp && lp->Get()) {
        act = lp->Get();
        if (last && (*last == *act)) {
            // try to remove point "act"
            flag1 = nodeIsBorrowed(act);
            flag3 = nodeIsInput(act);
            flag4 = nodeIsOutput(act);
            if (flag1 == WFAIL && flag3 == WFAIL && flag4 == WFAIL) {
                // remove point act
                lp->Destroy(act);
                if (!activeNodeL || activeNodeL->Get() == act) {
                    lockNode((KlogicList<QPoint> *)NULL);
                }
                delete act;
                modified = 1;
            }

            // try to remove point "last"
            else {
                flag1 = nodeIsBorrowed(last);
                flag3 = nodeIsInput(last);
                flag4 = nodeIsOutput(last);
                if (flag1 == WFAIL && flag3 == WFAIL && flag4 == WFAIL) {
                    // remove point last
                    lp->Destroy(last);
                    if (!activeNodeL || activeNodeL->Get() == act) {
                        lockNode((KlogicList<QPoint> *)NULL);
                    }
                    delete last;
                    modified = 1;
                }
            }
        }
        if (modified) {
            modified = 0;
            last = (QPoint *)NULL;
            lp = m_oNodes.First();
        } else {
            last = lp->Get();
            lp = lp->Next();
        }
    }
}

// set the active node as input-node, if it's an endpoint
int Wire::connectInput(int idx)
{
    if (activeNodeL && (!activeNodeL->Next() || !activeNodeL->Prev())) {
        if (m_poInputNode[idx])
            return WFAIL;

        m_poInputNode[idx] = activeNodeL->Get();
        return idx;
    }
    qFatal(NOACTIVECONN);
    return WFAIL;
}

// reset input-node
void Wire::disconnectInput(int idx)
{
    m_poInputNode[idx] = (QPoint *)NULL;
}

// set the active node as output-node, if it's an endpoint
int Wire::connectOutput(int idx)
{
    if (activeNodeL && (!activeNodeL->Next() || !activeNodeL->Prev())) {
        if (m_poOutputNode[idx])
            return WFAIL;

        m_poOutputNode[idx] = activeNodeL->Get();
        return idx;
    }
    qFatal(NOACTIVECONN);
    return WFAIL;
}

// reset output-node
void Wire::disconnectOutput(int idx)
{
    m_poOutputNode[idx] = (QPoint *)NULL;
}

// set io-point for connection to another wire
int Wire::connectWire(int idx)
{
    if (activeNodeL) {
        if (nodeBorrowedToOthers[idx])
            return WFAIL;

        nodeBorrowedToOthers[idx] = activeNodeL->Get();
        return idx;
    }
    return WFAIL;
}

void Wire::disconnectWire(int idx)
{
    nodeBorrowedToOthers[idx] = (QPoint *)NULL;
}

// change position of input-node (device-connection nodes) on the currently locked node
void Wire::setInputPosition(QPoint newp)
{
    if (!activeNodeL) qFatal("Wire::setInputPosition: no current node!?");
    if (activeNodeL->Get() == m_poInputNode[0]) *(m_poInputNode[0]) = newp;
    else if (activeNodeL->Get() == m_poInputNode[1]) *(m_poInputNode[1]) = newp;
}

// change position of input-node (device-connection nodes)
void Wire::setInputPosition(QPoint newp, int idx)
{
    if (m_poInputNode[idx]) *(m_poInputNode[idx]) = newp;
    else qFatal("Wire::setInputPosition: no connection point in >%d<", idx);
}

// change position of output-node (device-connection nodes)
void Wire::setOutputPosition(QPoint newp)
{
    if (!activeNodeL) qFatal("Wire::setOutputPosition: no current node!?");
    if (activeNodeL->Get() == m_poOutputNode[0]) *(m_poOutputNode[0]) = newp;
    else if (activeNodeL->Get() == m_poOutputNode[1]) *(m_poOutputNode[1]) = newp;
}

// change position of output-node (device-connection nodes)
void Wire::setOutputPosition(QPoint newp, int idx)
{
    if (m_poOutputNode[idx]) *(m_poOutputNode[idx]) = newp;
    else qFatal("Wire::setOutputPosition: no connection point in >%d<", idx);
}

// get position of input-node (device-connection nodes)
QPoint Wire::getInputPosition(int idx, int selonly)
{
    QPoint p(0,0);
    if (m_poInputNode[idx]) {
        if (!selonly) return *(m_poInputNode[idx]);
        if (m_oSelectedNodes.With(m_poInputNode[idx])) return *(m_poInputNode[idx]);
    }
    return p;
}

// get position of input-node (device-connection nodes)
QPoint Wire::getOutputPosition(int idx, int selonly)
{
    if (m_poOutputNode[idx]) {
        if (!selonly) return *(m_poOutputNode[idx]);
        if (m_oSelectedNodes.With(m_poOutputNode[idx])) return *(m_poOutputNode[idx]);
    }

    return QPoint(0,0);
}

// get position of input/output-node (wire-connection nodes)
QPoint Wire::getIOPosition(int idx, int selonly)
{
    if (nodeBorrowedToOthers[idx]) {
        if (!selonly) return *(nodeBorrowedToOthers[idx]);
        if (m_oSelectedNodes.With(nodeBorrowedToOthers[idx])) return *(nodeBorrowedToOthers[idx]);
    }
    return QPoint(0,0);
}

// import/export methods
void Wire::setExportSelected(bool selected)
{
    exp_selected = selected;
}

void Wire::setExportDelta(int dx, int dy)
{
    exp_dx = dx;
    exp_dy = dy;
}

// serialize
QString Wire::operator>> (QString &ret)
{
    QString buf;
    bool first = true;
    QPoint pt;

    KlogicList<QPoint> *lp = m_oNodes.First();
    KlogicList<QPoint> *ls = m_oSelectedNodes.First();

    while (lp) {
        if ((exp_selected && ls->With(lp->Get())) || !exp_selected) {
            pt.setX(lp->Get()->x() - exp_dx);
            pt.setY(lp->Get()->y() - exp_dy);
            if (first) {
                buf.sprintf("%d:%d", pt.x(), pt.y());
                first = false;
            } else buf.sprintf(",%d:%d", pt.x(), pt.y());
            ret += buf;
        }
        lp = lp->Next();
    }
    return ret;
}

// deserialize
bool Wire::operator<< (const QString &swire)
{
    if (swire.contains("{")) return string2wire(swire);

    int nodecounter = 0;
    int pos_delim = 0;
    int pos_y = 0;
    int pos_x = 0;
    int x;
    int y;

    while (pos_delim != -1) {
        pos_y = swire.indexOf(':', pos_x);
        pos_delim = swire.indexOf(',', pos_x);

        if (pos_y == -1) return false;
        x = swire.mid(pos_x, pos_y - pos_x).toInt();
        if (pos_delim == -1)
            y = swire.mid(pos_y + 1).toInt();
        else
            y = swire.mid(pos_y + 1, pos_delim - pos_y - 1).toInt();

        QPoint *newP = new QPoint(x + exp_dx, y + exp_dy);
        m_oNodes.Append(newP);
        nodecounter++;

        pos_x = pos_delim + 1;
    }

    if (nodecounter > 1) return true;
    return false;
}

// private, historic method
// makes a wire from a string containing points
// return 0 if unsuccessful
bool Wire::string2wire(const QString &swire)
{
    int nodecounter = 0;

    int startIdx = swire.indexOf('{');
    while (startIdx >= 0) {
        int endIdx1 = swire.indexOf(':', startIdx);
        int endIdx2 = swire.indexOf('}', startIdx);

        if (endIdx1 < 0 || endIdx2 < 0) {
            return 0;
        }

        QString xposStr = swire.mid(startIdx + 1, endIdx1 - startIdx - 1);
        int xpos = xposStr.toInt();
        QString yposStr = swire.mid(endIdx1 + 1, endIdx2 - endIdx1 - 1);
        int ypos = yposStr.toInt();

        QPoint *newP = new QPoint(xpos + exp_dx, ypos + exp_dy);
        m_oNodes.Append(newP);

        startIdx = swire.indexOf('{', endIdx2 + 1);
        nodecounter++;
    }

    return nodecounter > 1;
}

void Wire::debug() {
    KlogicList<QPoint> *lp = m_oNodes.First();

    QString buf;
    QString result;
    result.sprintf("%lx (", (unsigned long)this);
    while (lp) {
    	bool isBorrowed = nodeIsBorrowed(lp->Get()) != WFAIL;
    	bool isForeign = m_oForeignWireNodes.With(lp->Get()) != NULL;
    	if (isBorrowed) {
    		if (isForeign) {
    			buf.sprintf(" %d:%d(BF)", lp->Get()->x(), lp->Get()->y());
    		} else {
    			buf.sprintf(" %d:%d(B)", lp->Get()->x(), lp->Get()->y());
    		}
    	} else {
    		if (isForeign) {
    			buf.sprintf(" %d:%d(F)", lp->Get()->x(), lp->Get()->y());
    		} else {
    			buf.sprintf(" %d:%d()", lp->Get()->x(), lp->Get()->y());
    		}
    	}
    	result += buf;
        lp = lp->Next();
    }
    result += ")";
    qDebug() << result;
}
