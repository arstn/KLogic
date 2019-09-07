// ------------------------------------------------
// bus device can be a multiplexer or demultiplexer
//
// multiplexer has one input and many outputs
// demultiplexer has many inputs and one output
//
// Andreas Rostin
// 07.03.2001
// ------------------------------------------------

#ifndef __DEVICE_BUS__
#define __DEVICE_BUS__

#include <qpainter.h>
#include "grid.h"
#include "xdevice.h"

class BusDevice : public XDevice
{
public:
	BusDevice(const QPoint& oPos, int size = -1)
		: XDevice(DeviceType::fBUS, oPos, size)
	{
		if (size == -1) {
			setSize(8);
		} else {
			setSize(size);
		}
		setUndefinedValue(KlogicGlobal::Device::TRISTATE);
	}

	virtual ~BusDevice()
	{
	}

	// not in use .. (constructor)
	virtual void init()
	{
		setMux(false);	// fan in per default
		XDevice::init();
	}

	virtual void setMux(bool flag)
	{
		m_bMuxMode = flag;
		m_oImageKey.setDevType(m_bMuxMode);
		setImage();
		setEquation();
		parseEquation();
	}

	virtual bool isMux()
	{
		return m_bMuxMode;
	}

	virtual bool hasBooleanOutput(int) const
	{
		return m_bMuxMode;
	}

	virtual bool setSize(int iSize)
	{
		bool retval = XDevice::setSize(iSize);
		setEquation();
		parseEquation();
		return retval;
	}

	virtual void setEquation()
	{
		KlogicList<OutputInfo> output_list;
		KlogicList<OutputInfo> *ls;

		// create list with needed equations
		if (m_bMuxMode) {
			// -----------------------------------------------
			// multiplexing mode (fan out): single input, many output
			// -----------------------------------------------
			for(int i = 0; i < getSize(); i++) {
				OutputInfo *oi = new OutputInfo;

				oi->output_name.sprintf("D%d", i);
				oi->output_id = getOutputID(oi->output_name);

				oi->equation = "DI " + Operator::CHR_BIT + " ";
				oi->equation += QString::number(i);

				oi->output_type = KlogicGlobal::Device::FINAL_OUTPUT;

				ls = output_list.Append(oi);
				ls->setText(oi->output_name);
				ls->setID2(oi->output_type);
			}
		} else {
			// -----------------------------------------------
			// demultiplexing mode (fan in): many inputs, single output
			// -----------------------------------------------
			OutputInfo *oi = new OutputInfo;
			oi->output_name.sprintf("DO");
			oi->output_id = getOutputID(oi->output_name);
			oi->output_type = KlogicGlobal::Device::FINAL_OUTPUT;

			QString input;
			for(int i = 0; i < getSize(); i++) {
				input.sprintf("D%d", i);
				if (oi->equation.isEmpty()) {
					oi->equation = input;
				} else {
					oi->equation += " " + Operator::CHR_BIN + " ";	// bin operator (OP_BIN)
					oi->equation += input;
				}
			}
			ls = output_list.Append(oi);
			ls->setText(oi->output_name);
			ls->setID2(oi->output_type);
		}

		// change device, but not the size
		setAllEquations(&output_list, false);

		// destroy list
		ls = output_list.First();
		while(ls) {
			delete ls->Get();
			ls = ls->Next();
		}
	}

	virtual bool hasNamedInput() const
	{
		return true;
	}

	virtual bool hasNamedOutput() const
	{
		return true;
	}

	virtual bool isDeviceTristateEnabled() {
		return false;
	}

	virtual Image * createImage(const ImageKey& oKey)
	{
		QPainter p;
		QPoint pt(0,0);
		int width = 21;

		// resize image first, erase everything
		Image *poImage = new Image(width, 6 + ((getSize() - 1) * Grid::GRID));
		poImage->fill();

		p.begin(poImage);

		// top of the device
		pt.setY(pt.y() + 3);

		// body
		p.setPen(Qt::black);
		p.setBrush(Qt::black);
		p.drawRect(8, 0, width - 17, getSize() * Grid::GRID);

		// input/output lines
		if (oKey.getDevType()) {
			// one input, many output
			for(int i = 0; i < getSize(); i++) {

				// input line
				if (!i) p.drawLine(3, pt.y(), 8, pt.y());
				// output line
				p.drawLine(width - 9, pt.y(), width - 3, pt.y());

				pt.setY(pt.y() + Grid::GRID);
			}
		} else {
			// many input, one output
			for(int i = 0; i < getSize(); i++) {
				// output line
				if (!i) p.drawLine(width - 9, pt.y(), width - 3, pt.y());
				// input line
				p.drawLine(3, pt.y(), 8, pt.y());

				pt.setY(pt.y() + Grid::GRID);
			}
		}
		p.end();
		return poImage;
	}

	virtual void printImage(QPainter *, const QRect &)
	{
	}

	virtual bool outputIsActive(int)
	{
		return true;
	}

private:
	// type of bus: multiplexer or demultiplexer
	bool m_bMuxMode;
};

#endif
