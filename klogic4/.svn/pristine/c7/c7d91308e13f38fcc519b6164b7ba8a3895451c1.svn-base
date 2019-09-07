/*
 * SimulatorThread.h
 *
 *  Created on: Dec 21, 2011
 *      Author: andi
 */

#ifndef SIMULATORTHREAD_H_
#define SIMULATORTHREAD_H_

#include <qthread.h>
class Circuit;

class SimulatorThread : public QThread {
    Q_OBJECT
public:
	SimulatorThread(Circuit *circuit);
	virtual ~SimulatorThread();

	void run();
	Circuit * getCircuit();
	bool isCalculating();
	void setPaused(bool paused);

public slots:
	void simStep();

private:
	Circuit *circuit;
	bool calculating;
	bool paused;
};

#endif /* SIMULATORTHREAD_H_ */
