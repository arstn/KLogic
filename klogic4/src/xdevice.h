#ifndef __XDevice_CLASS__
#define __XDevice_CLASS__

#include <klogicList.h>
#include <device.h>
#include <qpixmap.h>
#include <xobject.h>
#include <device.h>
#include <deviceInterface.h>
#include <imageCache.h>

class XWire;
class Circuit;

/********************************************
* a device on the screen                    *
*********************************************/
class XDevice: public XObject, public DeviceInterface
{
public:
    static int instance;

    XDevice(int iFunction, const QPoint& oPos, int iSize = -1);
    virtual ~XDevice();

    // positioning methods
    bool moveTo(const QPoint&);			// absolute move
    void moveBy(const QPoint&);			// relative move

    // drawing methods
    virtual void setImage();				// internal redraw of the device pixmap
    virtual void updateImage();				// internal replacement of the image for output dependent devices
    void draw(QPainter *);					// draw device as lines and text to a printer device
    void draw(QPaintDevice *, QPainter *);	// the usual method to draw devices!

    // graph methods
    virtual bool isObservable();
    bool isObserved();
    void setObserved(bool);
    int invertOutputValForObserver(int, int);

    // some graphical device properties
    int getSize();
    virtual bool setSize(int);
    virtual int neededSize();
    virtual bool sizeChangeable() const {
        return true;
    }

    bool isInverted();
    void setInverted(bool);
    int getMaxInputCount();
    int getCurrentInputCount();
    void setMaxInputCount(int);
    int getMaxOutputCount();
    int getCurrentOutputCount();
    void setMaxOutputCount(int);
    virtual void setName(const QString&);
    virtual void displayName(bool);
    bool nameDisplayed();
    void enableTristateInput(bool);
    int getTristateControlPos();
    void setTristateControlPos(int);
    void setTristateInputImport();
    virtual void setUndefinedValue(int);
    virtual void setWidth(int) {}
    virtual int width() {
        return KlogicGlobal::Device::DEFAULT_WIDTH;
    }

    virtual void setColor(int) {
    }

    virtual int getColor() {
    	return -1;
    }

    // special cases: properties dialog methods
    // return pointers to input/output lists
    // called by PropDev
    KlogicList<XWire> *getIRef();
    KlogicList<XWire> *getORef();

    // check and set/reset connections
    int checkConnection(XWire *);

    // called by wire: add/remove connections
    int inputIsInvertedAt(QPoint);
    void addInputWire(XWire *);
    void removeInputWire(XWire *);
    int outputIsInvertedAt(QPoint);
    void addOutputWire(XWire *);
    void removeOutputWire(XWire *);

    // remove garbage from wires
    void garbageCollection();

    // add/remove named inputs/outputs (overloaded from class Device)
    virtual int addInputName(const QString&, int pos = -1, int iInputID = 0);
    virtual int changeInputName(int, const QString&);
    virtual void removeInputName(int);
    virtual int addOutputName(const QString&, int pos = -1, int iOutputID = 0);
    virtual int changeOutputName(int, const QString&);
    virtual void removeOutputName(int);

    // circuit management
    virtual Circuit * devIsCircuit() {
        return 0;
    };	// this is not a Circuit
    virtual bool addChild(XDevice *, bool) {
        return true;
    };
    virtual void setParent(XDevice *) {};

    virtual void init();

    // image caches
    static BasicImageCache m_oBasicImageCache;
    static ImageCache m_oImageCache;

protected:
    virtual Image * createImage(const ImageKey&);
    virtual void setImageKey() {
        m_oImageKey.setSymbolSet(KlogicGlobal::Device::getSymbolSet());
    }
    virtual void printImage(QPainter *, const QRect &);

    // connected wires
    KlogicList<XWire> m_oIncomingWires;
    KlogicList<XWire> m_oOutgoingWires;

    int maxInputCount;
    int maxOutputCount;
    bool deviceOutputInverted;

    // text device, optionally displayed above this
    XDevice *m_poTextDev;

    ImageKey m_oImageKey;				// image cache key

private:
    void updateWires(const QPoint& deltaPos);

    int checkInput(XWire *, bool);
    int connectInput(XWire *, QPoint, int iInputID = 0);
    int checkOutput(XWire *, bool);
    int connectOutput(XWire *, QPoint, int iOutputID = 0);

    void prepareTristateInputImport();

    int m_iSize;

    // tristate handling
    int m_iTristateInputMoved;
    int m_iTristateImportPos;

    // device displayed in graph
    bool observed;
public:
    // old device import
    bool string2device(QString, int, int);

};

#endif

