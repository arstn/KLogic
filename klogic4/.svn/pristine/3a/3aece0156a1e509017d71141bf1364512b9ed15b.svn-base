#ifndef KLOGIC_IMAGE_CACHE
#define KLOGIC_IMAGE_CACHE

#include <map>
#include <iostream>
#include <QPixmap>

#include "KlogicGlobal.h"
#include "tool.h"

using namespace std;

// --------------------------------------
// an image
// --------------------------------------
class Image: public Referenced, public QPixmap
{
public:
	Image() : QPixmap() {}
	Image(int x, int y) : QPixmap(x,y) {}
	virtual ~Image() {}
};

// --------------------------------------
// identify images
// --------------------------------------
class ImageKey
{
	friend ostream& operator<< (ostream& os, const ImageKey& oKey);

public:
	ImageKey();
	ImageKey(int iFunction, int iSize, int iWidth, KlogicGlobal::Device::CLOCKTYPE iClock, bool bMaster, int iOutput, int iColor, int iSymbolSet);
	// for output dependant images
	ImageKey(int iFunction, int iSize, int iWidth, KlogicGlobal::Device::CLOCKTYPE iClock, bool bMaster, int iSymbolSet);
	~ImageKey();

	int getType() const;
	void setColor(int iColor);
	int getColor() const;
	void setClock(KlogicGlobal::Device::CLOCKTYPE iClock);
	KlogicGlobal::Device::CLOCKTYPE getClock() const;
	void setWidth(int iWidth);
	void setSize(int iSize);

	void setMaster(bool iMaster);
	bool getMaster() const;

	void setOutput(int iOutput);
	int getOutput() const;

	void setSymbolSet(int iSymbolSet);

	void setDevType(int);
	int getDevType() const;

	void operator= (const ImageKey& other);
	bool operator== (const ImageKey& other) const;
	bool operator< (const ImageKey& other) const;
	int compareTo(const ImageKey& other) const;

private:
	int m_iFunction;
	int m_iSize;
	int m_iWidth;
	KlogicGlobal::Device::CLOCKTYPE m_iClock;
	bool m_bMaster;
	int m_iOutput;
	int m_iColor;
	int m_iSymbolSet;
	int m_iDevType;
};

ostream& operator<< (ostream&, const ImageKey&);

// --------------------------------------
// ready-to-use images as needed
// --------------------------------------
class ImageCache: private map<ImageKey, Image *>
{
public:
	ImageCache();
	~ImageCache();

	const Image* getImage(const ImageKey& oKey) const;
	void addImage(const ImageKey& oKey, Image *poImage);
private:
	static int m_iHitCounter;
	static int m_iFailCounter;
};

// --------------------------------------
// loads all standard images from file
// --------------------------------------
class BasicImageCache: private map<int, Image *>
{
public:
	BasicImageCache();
	~BasicImageCache();

	void init();

	Image* getImage(int iKey) const;

	static const int BODY_TOP_HUGE = 1;
	static const int BODY_MID_HUGE = 2;
	static const int BODY_BOT_HUGE = 3;
	static const int BODY_TOP_LARGE = 4;
	static const int BODY_MID_LARGE = 5;
	static const int BODY_BOT_LARGE = 6;
	static const int BODY_TOP = 7;
	static const int BODY_MID = 8;
	static const int BODY_BOT = 9;
	static const int BODY_TOP_BLANK = 10;
	static const int BODY_MID_BLANK = 11;
	static const int BODY_BOT_BLANK = 12;
	static const int BODY_POS = 13;
	static const int BODY_NEG = 14;
	static const int BODY_OUT = 15;
	static const int BODY_IN = 16;
	static const int BODY_SWI0 = 17;
	static const int BODY_SWI1 = 18;
	static const int BODY_JK = 19;
	static const int BODY_JKF = 20;
	static const int BODY_RS = 21;
	static const int BODY_RSC = 22;
	static const int BODY_RSCM = 23;
	static const int BODY_RSCMF = 24;
	static const int BODY_DM = 25;
	static const int BODY_D = 26;
	static const int BODY_SSB = 27;
	static const int SYMBOL_SS[16];		// 2000 .. 2015
	static const int BODY_LED0blue = 29;
	static const int BODY_LED1blue = 30;
	static const int BODY_LED0red = 31;
	static const int BODY_LED1red = 32;
	static const int BODY_LED0green = 33;
	static const int BODY_LED1green = 34;
	static const int BODY_LED0yellow = 35;
	static const int BODY_LED1yellow = 36;
	static const int BODY_TFF = 37;

	static const int CURSOR_SIMWIDGET = 38;

	static const int SYMBOL_AND = 1000;
	static const int SYMBOL_OR = 1001;
	static const int SYMBOL_XOR = 1002;
	static const int SYMBOL_NOT = 1003;
	static const int SYMBOL_OSC = 1004;
	static const int SYMBOL_MONOFLOP = 1005;
	static const int SYMBOL_IECAND = 1006;
	static const int SYMBOL_IECOR = 1007;
	static const int SYMBOL_IECXOR = 1008;
	static const int SYMBOL_IECNOT = 1009;

private:
	void addImage(int iKey, Image *poImage);
};

#endif
