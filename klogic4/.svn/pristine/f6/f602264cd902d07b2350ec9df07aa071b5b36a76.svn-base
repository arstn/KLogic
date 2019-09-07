// --------------------------------------------------------------
// BusOutputArray class
//
// adds an output bus to a device
// outputs "D0" .. "D8" will be added at position 0..7
//
// can also add an input bus to the device
// then, inputs "D0" .. "D8" will be added at position 0..7
//
// Andreas Rostin
// 2003-02-01
//
// --------------------------------------------------------------

#ifndef __BUS_OUTPUT_ARRAY__
#define __BUS_OUTPUT_ARRAY__

#include "xdevice.h"
#include "tool.h"

class BusOutputArray
{
public:
	// adds outputs D0..D7 to a device
	// as_tristate: adds outputs D0..D7 as tristate outputs
	// i_flag: if true, also adds inputs D0..D7
	BusOutputArray(XDevice *_dev, bool i_flag, bool as_tristate)
	{
		dev = _dev;

		// output bus only
		initOutputBus(as_tristate);

		// input- and output bus
		if (i_flag)
			initInputBus();
	}

	int getOutputWeight(int output_id)
	{
		if (!output_id) return -1;
		for(int i = 0; i < 8; i++)
			if (output_id == m_aBusOutputId[i])
				return Tool::pow2(i);
		return -1;
	}

	int operator [] (int idx)
	{
		return m_aBusOutputId[idx];
	}

private:
	void initOutputBus(bool as_tristate)
	{
		for (int i = 0; i < 8; i++) {
			m_aBusOutputId[i] = dev->addOutputName(IO_BUS_DATA[i], i + 1);
			//qDebug() << "Output added Name=" << IO_BUS_DATA[i] << " ID=" << m_aBusOutputId[i];
			if (as_tristate)
				dev->setOutputTristateEnabled(m_aBusOutputId[i], true);
		}
	}

	void initInputBus()
	{
		for (int i = 0; i < 8; i++) {
			dev->addInputName(IO_BUS_DATA[i], i + 1);
		}
	}

	static const QString IO_BUS_DATA[8];	// defined in xdevice.cpp!

	int m_aBusOutputId[8];   // output bus

	XDevice *dev;
};

#endif
