/*
 * OscSlot.h
 *
 *  Created on: Dec 18, 2011
 *      Author: andi
 */

#ifndef OSCCHANNEL_H_
#define OSCCHANNEL_H_

#include <qstring.h>

class XDevice;

#include <list>
using namespace std;


class OscChannel {
public:
	OscChannel(XDevice *dev, int outputId, int existingDepth);
	virtual ~OscChannel();

	const XDevice * getDevice();

	const QString & getName();
	void setName(const QString &name);

	void simStep();
	const list<int> & getValues();

	void setSize(unsigned int size);

	bool equals(XDevice *dev, int outputId);
private:
	QString name;
	XDevice *dev;
	int outputId;

	list<int> values;
	unsigned int size;
};

#endif /* OSCSLOT_H_ */
