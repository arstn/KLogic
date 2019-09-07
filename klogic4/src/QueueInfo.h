#ifndef __QueueInfo_CLASS__
#define __QueueInfo_CLASS__

#include "Operator.h"

// ----------------------------------
// Calculator parsing result
// ----------------------------------
class QueueInfo
{
public:
	QueueInfo()
	{
		clear();
	}

	void clear()
	{
		sOperator = Operator::CHR_NONE;
		outputID = 0;
		value = 0;
		queueContentType = VAL_NOTYPE;
		posInEquation = 0;
		lenInEquation = 0;

		parseError = NOERR;
	}

	enum QueueContentType { VAL_NOTYPE, VAL_INPUT = 1, VAL_NUMBER, VAL_OUTPUT, VAL_THIS, VAL_OPERATOR };
	enum ErrType { NOERR, ERR_NTERM, ERR_PARENTHESIS };

	QString inputName;
	QString sOperator;
	QString outputName;
	int outputID;
	int value;
	QueueContentType queueContentType;
	int posInEquation;
	int lenInEquation;

	ErrType parseError;
};
#endif
