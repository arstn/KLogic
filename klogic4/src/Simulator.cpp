/*
 * Simulator.cpp
 *
 *  Created on: Dec 21, 2011
 *      Author: andi
 */

#include "Simulator.h"
#include "klogic4.h"

#include "Simulator.moc"

//static
Simulator *Simulator::instance;
Simulator * Simulator::getInstance() {
	if (!instance) {
		instance = new Simulator();
	}
	return instance;
}

Simulator::Simulator() {
	paused = 1;
	simulationMode = klogic4::simModeSINGLE_STEP;
    connect(&simTimer, SIGNAL(timeout()), this, SLOT(simStep()));
}

Simulator::~Simulator() {
}

void Simulator::setPaused(bool paused) {
	int prevPaused = this->paused;
	if (paused) {
		this->paused++;
	} else {
		this->paused--;
	}

	if (!prevPaused && this->paused) {
		simTimer.stop();
		for (list<SimulatorThread *>::iterator iter = threads.begin(); iter != threads.end(); iter++) {
			(*iter)->setPaused(true);
		}
		while(isCalculating());
		qDebug() << "Simulator: paused";
	} else if (prevPaused && !this->paused) {
		if (klogic4::getInstance()->getSimulationMode() == klogic4::simModePERMANENT) {
			for (list<SimulatorThread *>::iterator iter = threads.begin(); iter != threads.end(); iter++) {
				(*iter)->setPaused(false);
			}
			simTimer.start(klogic4::getInstance()->getSimStepTime());
		}
		qDebug() << "Simulator: continued";
	}
}

bool Simulator::isPaused() {
	return paused;
}

void Simulator::quit() {
	setPaused(true);
	for (list<SimulatorThread *>::iterator iter = threads.begin(); iter != threads.end(); iter++) {
		(*iter)->exit();
	}
}

void Simulator::simulationParametersChanged() {
    if (klogic4::getInstance()->getSimulationMode() == klogic4::getInstance()->simModePERMANENT &&
    	simulationMode != klogic4::getInstance()->simModePERMANENT) {
    	setPaused(false);
    } else if (klogic4::getInstance()->getSimulationMode() == klogic4::getInstance()->simModeSINGLE_STEP &&
    	simulationMode != klogic4::getInstance()->simModeSINGLE_STEP) {
   		setPaused(true);
    } else {
    	simTimer.start(klogic4::getInstance()->getSimStepTime());
    }
    simulationMode = klogic4::getInstance()->getSimulationMode();
}

bool Simulator::isCalculating() {
	for (list<SimulatorThread *>::iterator iter = threads.begin(); iter != threads.end(); iter++) {
		if ((*iter)->isCalculating()) {
			return true;
		}
	}
	return false;
}

void Simulator::circuitAdd(Circuit *circuit) {
	for (list<SimulatorThread *>::iterator iter = threads.begin(); iter != threads.end(); iter++) {
		if ((*iter)->getCircuit() == circuit) {
			qFatal("circuitAdd: circuit does already exist in Simulator thread list.");
		}
	}
	SimulatorThread *theThread = new SimulatorThread(circuit);
	threads.push_back(theThread);
	qDebug() << "Simulator: circuit " << circuit << " added";
}

void Simulator::circuitRemove(Circuit *circuit) {
	setPaused(true);
	SimulatorThread *theThread = 0;
	for (list<SimulatorThread *>::iterator iter = threads.begin(); iter != threads.end(); iter++) {
		if ((*iter)->getCircuit() == circuit) {
			theThread = *iter;
			theThread->quit();
			theThread->wait();
			break;
		}
	}
	if (!theThread) {
		qFatal("circuitRemove: circuit does not exist in Simulator thread list.");
	}
	threads.remove(theThread);
	delete theThread;
	qDebug() << "Simulator: circuit " << circuit << " removed";
	setPaused(false);
}

void Simulator::simStep() {

	for (list<SimulatorThread *>::iterator iter = threads.begin(); iter != threads.end(); iter++) {
		QMetaObject::invokeMethod((*iter), "simStep", Qt::QueuedConnection);
	}
	emit simStepped();
}
