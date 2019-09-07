/*
 * SimulatorThread.cpp
 *
 *  Created on: Dec 21, 2011
 *      Author: andi
 */

#include "Simulator.h"
#include "SimulatorThread.h"
#include "circuit.h"
#include "klogic4.h"

SimulatorThread::SimulatorThread(Circuit *circuit) {
	this->circuit = circuit;
	calculating = false;

	this->start();
	QObject::moveToThread(this);
}

SimulatorThread::~SimulatorThread() {
}

void SimulatorThread::run() {
	qDebug() << "SimulatorThread " << thread()->currentThreadId() << " started for circuit " << circuit;
	exec();
	qDebug() << "SimulatorThread " << thread()->currentThreadId() << " stopped for circuit " << circuit;
}

Circuit * SimulatorThread::getCircuit() {
	return circuit;
}

void SimulatorThread::setPaused(bool paused) {
	this->paused = paused;
}

void SimulatorThread::simStep() {
	if (!paused) {
		//qDebug() << "caculating " << circuit << " in " << thread()->currentThreadId();
		calculating = true;
		circuit->Burst(klogic4::getInstance()->getSimBurstCount());
		calculating = false;
	}
}

bool SimulatorThread::isCalculating() {
	return calculating;
}
