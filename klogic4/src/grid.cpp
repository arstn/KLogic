//*************************************************************************
//* singleton grid object                                                 *
//*************************************************************************

#include <qpainter.h>
#include <qimage.h>
#include "grid.h"

#include "KlogicGlobal.h"

//=====================================================================
//== init static variables                                           ==
//=====================================================================
QImage Grid::m_oGrid = QImage();

//=====================================================================
//== constructor/destructor/initializing                             ==
//=====================================================================
Grid::Grid()
{
	init();
}
//---------------------------------------------------------------------
//-- creates the grid in an image buffer
//--
//-- performance measure: 
//-- painting 19200 points takes roughly 50 millisecs and 7.5 MByte
void Grid::init()
{
	// at first create the only grid image
	if (m_oGrid.isNull()) {
		// background
		QColor black(Qt::black);
		QColor white(Qt::white);
		m_oGrid = QImage(KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X, KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y, QImage::Format_RGB32);
		m_oGrid.fill(white.rgb());

		// foreground
		for (int x = 0; x < KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X; x += Grid::GRID) {
			for (int y = 0; y < KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y; y += Grid::GRID) {
				m_oGrid.setPixel( x, y, black.rgb());
			}
		}
	}
}
//---------------------------------------------------------------------

//=====================================================================
//== painting                                                        ==
//=====================================================================
//-- paints background and grid for entire virtual screen
//-- this method is normaly called once in a program run
//--
//-- performance measure: painting needs nearly 250 millisecs
void Grid::draw( QPainter* p )
{
	QRect rcVirtScreen( 0, 0, KlogicGlobal::Screen::VIRT_SCREEN_SIZE_X, KlogicGlobal::Screen::VIRT_SCREEN_SIZE_Y );

	Grid::draw( p, rcVirtScreen );
}
//---------------------------------------------------------------------
//-- paints background and grid for the former position of
//-- a moved device
//--
//-- performance measure:
//-- painting needs nearly 5 millisecs per device movement
void Grid::draw( QPainter* p, QRect rc )
{
	p->drawImage(rc.x(), rc.y(), m_oGrid, rc.x(), rc.y(), rc.width(), rc.height());
}
//---------------------------------------------------------------------

//=====================================================================
//== grid alignment                                                  ==
//=====================================================================
//-- ptSrc	Coordinate to align
QPoint Grid::getAligned(const QPoint& ptSrc)
{
	// Align to the same grid size in x and y direction
	QPoint ptReturn = ( ptSrc / Grid::GRID ) * Grid::GRID;

	return ptReturn;
}
//---------------------------------------------------------------------
//-- rcSrc	Coordinate to align
QRect Grid::getAligned(const QRect& rcSrc)
{
	QRect 	rcReturn 	= rcSrc;

	// Align only one angle of the bounding rectangle
	QPoint	ptTopLeft	= getAligned( rcReturn.topLeft() );

	// Move along the axis
	rcReturn.moveTopLeft( ptTopLeft );

	return rcReturn;
}
//---------------------------------------------------------------------
