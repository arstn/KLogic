#ifndef __GRID_H
#define __GRID_H

//*************************************************************************
//* singleton grid object                                                 *
//*************************************************************************
class Grid
{
	//=====================================================================
	//== constructor/destructor/initializing                             ==
	//=====================================================================
	public:
		Grid();

		//-- grid alignment
		static QPoint getAligned(const QPoint&);
		static QRect getAligned(const QRect&);

		//-- paints background and grid for entire virtual screen
		static	void 	draw( QPainter* );

		//-- paints the grid for the former position of an object
		static	void 	draw( QPainter*, QRect );

		static 	const int GRID		= 10;
		static 	const int GRIDHALF	= GRID / 2;
		static 	const int GRIDDOUBLE	= GRID * 2;

	private:
		//-- creates the grid in an image buffer
		static void init();

		//-- saves the background and grid of entire virtual screen
		static 	QImage	m_oGrid;
};

#endif
