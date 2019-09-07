/*
 * OscSlot.cpp
 *
 *  Created on: Dec 18, 2011
 *      Author: andi
 */

#include "KlogicGlobal.h"
#include "xdevice.h"
#include "OscChannel.h"


OscChannel::OscChannel(XDevice *dev, int outputId, int existingDepth) {
	this->dev = dev;
	this->outputId = outputId;
    name = dev->getName();
    if (outputId) {
		name.append(" (");
		name.append(dev->getOutputName(outputId));
		name.append(")");
    }
    size = 600;
    for (int i = 0; i < existingDepth; i++) {
    	int val = KlogicGlobal::Device::TRISTATE;
    	values.push_back(val);
    }
}

OscChannel::~OscChannel() {
}

const XDevice * OscChannel::getDevice() {
	return dev;
}

const QString & OscChannel::getName() {
	return name;
}

bool OscChannel::equals(XDevice *dev, int outputId) {
	if (this->dev == dev && this->outputId == outputId) {
		// device and output are the same but update the name
		QString currentName = dev->getName();
	    if (outputId) {
	    	currentName.append(" (");
	    	currentName.append(dev->getOutputName(outputId));
	    	currentName.append(")");
	    }
	    if (name != currentName) {
	    	name = currentName;
	    }
	    return true;
	}
    return false;
}

void OscChannel::simStep() {
	values.push_back(dev->output(outputId));
	while (values.size() > size) {
		values.pop_front();
	}
}

const list<int> & OscChannel::getValues() {
	return values;
}

void OscChannel::setSize(unsigned int size) {
	this->size = size;
	while (values.size() > size) {
		values.pop_front();
	}
	while (values.size() < size) {
    	int val = KlogicGlobal::Device::TRISTATE;
    	values.push_back(val);
	}
}
