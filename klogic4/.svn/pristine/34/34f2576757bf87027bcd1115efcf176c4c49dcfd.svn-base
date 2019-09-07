/*
 * Simulator.h
 *
 *  Created on: Dec 21, 2011
 *      Author: andi
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <qthread.h>
#include <qtimer.h>

#include "circuit.h"
#include "SimulatorThread.h"

#include <list>
using namespace std;

class Simulator : public QObject {
    Q_OBJECT
public:
	static Simulator * getInstance();
	virtual ~Simulator();

	void setPaused(bool);
	bool isPaused();

    void simulationParametersChanged();
    void circuitAdd(Circuit *);
    void circuitRemove(Circuit *);
	void quit();
signals:
	void simStepped();

public slots:
	void simStep();

private:
	Simulator();
	static Simulator *instance;
	bool isCalculating();

	list<SimulatorThread *> threads;
    QTimer simTimer;
    int paused;
    int simulationMode;
};


#endif /* SIMULATOR_H_ */
