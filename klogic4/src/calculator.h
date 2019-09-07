#ifndef __Calculator_CLASS__
#define __Calculator_CLASS__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <qvalidator.h>

class Device;
class QString;
#include<value.h>
class Fifo;
template <class T> class KlogicList;

#include "calcRules.h"

class Calculator : public DelayedValue {
public:
	static int instance;

	Calculator();
	Calculator(Device *, int, int);
	virtual ~Calculator();

	void setEquation(const QChar&);		// set equation
	void setEquation(const QString&);	// set equation
	const QString& getEquation() const;	// return equation
	QString getEquation(const QString&) const;	// return prefixed equation
	bool hasEquation();			// return wether we have an equation or not

	virtual const QueueInfo& parse();	// parse current equation, build calculation stack

	void push(int);				// push values to the calculation stack
	int calculate(bool);			// calculate the stack, return the result (needs a dev)

	void stackDump();			// debug
	void groupDump(int group_no, int **group, int *group_cnt);	// debug

	unsigned int getCount() const;		// return number of rules

	bool isStatic();			// returns wether eqation defines static operation or not

	static bool isNumber(const QString& str);

protected:
	virtual int pushOperand(const QString&, int, int);
	virtual void pushOperator(const QString&, int, int);
	virtual int getInputValue(int idx);

	CalcRules m_oCalcRules;
	int *m_poStack;				// calculation value stack
	int itemsOnStack;
	int currentStackSize;

private:
	void checkStackSize(int iNeededSize);
	
	void parse(QString &equation, int analysisStartIndex, int analysisEndIndex, int absPosOffset);
	int getNextOperatorIndex(const QString &equation, int startIdx);
	int getBracketCloseIndex(const QString &equation, int operatorIdx);
	int getRightOperandIndex(const QString &equation, int current_offset, int operatorIdx);
	int getLeftOperandIndex(const QString &equation, int current_offset, int operatorIdx);
	
	// results of parsing equations
	QueueInfo m_oParseInfo;
	// The equation
	QString m_sEquation;
	// The owner of the queue
	Device *m_dev;

	static QIntValidator intValidator;
};

#endif

