/*************************************************/
/* member methods for class Calculator           */
/*                                               */
/* equation analyzer and calculator              */
/*                                               */
/* Andreas Rostin                                */
/* 15.10.99                                      */
/*************************************************/
#include <qstring.h>
#include <qvalidator.h>

#include <device.h>
#include <tool.h>

#include <calculator.h>
#include <Operator.h>
#include <QueueInfo.h>

int Calculator::instance = 0;
QIntValidator Calculator::intValidator(0, 64, 0);

Calculator::Calculator(Device *dev, int undef, int delay)
	:DelayedValue(delay)
{
	instance++;
	itemsOnStack = 0;
	currentStackSize = 20;
	m_poStack = new int[currentStackSize];
	setUndefinedValue(undef);
	m_dev = dev;
}

Calculator::Calculator()
	:DelayedValue(0)
{
	instance++;
	itemsOnStack = 0;
	currentStackSize = 20;
	m_poStack = new int[currentStackSize];
	m_dev = (Device *)NULL;
}

Calculator::~Calculator()
{
	instance--;
}

// set a new equation
void Calculator::setEquation(const QChar& equation)
{
	m_sEquation = equation;
}

// set a new equation
void Calculator::setEquation(const QString& sEquation)
{
	m_sEquation = sEquation;
}

// an empty equation has a content
bool Calculator::hasEquation()
{
	return m_sEquation.length() > 0;
}

const QString & Calculator::getEquation() const
{
	return m_sEquation;
}

// build equation with all non-terminal qualified by a string
QString Calculator::getEquation(const QString& sQualifier) const
{
	QString sResultingEquation;
	bool bNonTerminalCanStart = true;

	int idx = 0;
	while(idx < m_sEquation.length()) {
		bool bThisFound = false;
		if (!Operator::isDelimiter(m_sEquation[idx]) && bNonTerminalCanStart) {
			if (m_sEquation.mid(idx, CalcRules::ITEM_THIS.length()) == CalcRules::ITEM_THIS) {
				if (sQualifier.length()) {
					sResultingEquation.append(sQualifier.left(sQualifier.length() - 1)); // remove dot
					bNonTerminalCanStart = true;
					bThisFound = true;
				} else {
					sResultingEquation.append(CalcRules::ITEM_THIS);
				}
				idx += CalcRules::ITEM_THIS.length();
			} else {
				sResultingEquation.append(sQualifier);
			}
			bNonTerminalCanStart = false;
		} else {
			bNonTerminalCanStart = true;
		}

		if (!bThisFound) {
			sResultingEquation.append(m_sEquation[idx]);
			idx++;
		}
	}
	return sResultingEquation;
}

bool Calculator::isStatic()
{
	return m_oCalcRules.isStatic();
}

// debugging method!
void Calculator::stackDump()
{
	qDebug() << "\n**********\nEquation: ";
	qDebug() << m_sEquation;
	m_oCalcRules.dump();
	qDebug() << "**********\n";
}

bool Calculator::isNumber(const QString& str)
{
	int i = 0;
	return QValidator::Acceptable == intValidator.validate((QString&)str, i);
}

// public parse method
// transform equation into reverse polish notation and create a calc stack
const QueueInfo& Calculator::parse()
{
  m_oParseInfo.clear();
  m_oCalcRules.clear();

  if (m_sEquation.isEmpty()) {
    return m_oParseInfo;
  }
  if (Operator::isOP(m_sEquation)) {
    pushOperator(m_sEquation, 0, m_sEquation.length());
    m_oCalcRules.setStatic(true);
    return m_oParseInfo;
  }
//  qDebug() << "Parsing " << m_sEquation;
  QString equationCopy = QString(m_sEquation);
  parse(equationCopy, 0, equationCopy.length() - 1, 0);
  return m_oParseInfo;
}

/**
 * Parse the equation and push all operations and their operands to the calculation stack in RPM (reverse poslish notation) for
 * continous calculation.
 * The results of parsing the equation (the errors) are accessable by member m_oParseInfo.
 * @param equation The equation or a subset of it when brackets are processed
 * @param current_offset This method is called recursively from within. the current_offset is the most left index into equation which
 *                 should be accessed. Going beyond during parsing, to the left of the equation string, is forbidden.
 * @return errors in member m_oParseInfo.
 */
void Calculator::parse(QString &equation, int analysisStartIndex, int analysisEndIndex, int absPosOffset)
{
  int opIdx;
  while ((opIdx = getNextOperatorIndex(equation, analysisStartIndex)) >= 0) {
	  const QString &op = equation.at(opIdx);
	  int iOpType = Operator::opType(op);

	  // parenthesis found
	  if (iOpType == Operator::TYPE_PARENTHESIS) {
			int bracketClose = getBracketCloseIndex(equation, opIdx);
			if (bracketClose < 0) {
			  m_oParseInfo.parseError = QueueInfo::ERR_PARENTHESIS;
			  m_oParseInfo.posInEquation = opIdx + absPosOffset;
			  m_oParseInfo.lenInEquation = 1;
			  return;
			}
			QString subEquation = equation.mid(opIdx + 1, bracketClose - opIdx - 1);
//			qDebug() << "Start sub equation " << subEquation;
			parse(subEquation, 0, subEquation.length() - 1, opIdx + 1);
			for (int i = opIdx; i <= bracketClose; i++) {
				equation[i] = QChar(' ');
			}
//			qDebug() << "End sub equation";
	  } else {
		  // normal operator found - if of type LR (left and right operand), then add two operands - one otherwise
		  int startOfLeftOperand = -1;
		  if (iOpType == Operator::TYPE_LR) {
			startOfLeftOperand = getLeftOperandIndex(equation, analysisStartIndex, opIdx);
			if (startOfLeftOperand >= 0) {
			}
		  }
		  int endOfRightOperand = getRightOperandIndex(equation, analysisEndIndex, opIdx);

		  if (startOfLeftOperand >= 0) {
			  int lenOfLeftOperand = opIdx - startOfLeftOperand - 1;
			  QString leftOperand = equation.mid(startOfLeftOperand, lenOfLeftOperand).trimmed();
			  pushOperand(leftOperand, absPosOffset + startOfLeftOperand, lenOfLeftOperand);
			  if (m_oParseInfo.parseError != QueueInfo::NOERR) {
				  return;
			  }
//			  qDebug() << "   Left operand start=" << startOfLeftOperand << " len=" << lenOfLeftOperand << " OP=" << leftOperand;
			  for (int i = startOfLeftOperand; i < opIdx; i++) {
				equation[i] = QChar(' ');
			  }
		  }
		  if (endOfRightOperand >= 0) {
			  int lenOfRightOperand = endOfRightOperand - opIdx;
			  QString rightOperand = equation.mid(opIdx + 1, lenOfRightOperand).trimmed();
			  pushOperand(rightOperand, absPosOffset + opIdx + 1, lenOfRightOperand);
			  if (m_oParseInfo.parseError != QueueInfo::NOERR) {
				return;
			  }
//			  qDebug() << "   Right operand end=" << endOfRightOperand << " len=" << lenOfRightOperand << " OP=" << rightOperand;
			  for (int i = opIdx + 1; i <= endOfRightOperand; i++) {
				equation[i] = QChar(' ');
			  }
		  }

		  pushOperator(op, absPosOffset + opIdx,  op.length());
//		  qDebug() << "   Operator " << op << "at " << opIdx;
		  equation[opIdx] = QChar(' ');
	  }
	  if (m_oParseInfo.parseError != QueueInfo::NOERR) {
		  return;
	  }
//	  qDebug() << "Equation is now " << equation;
  }
  // test for operators without operand
  int endOfRightOperand = getRightOperandIndex(equation, analysisEndIndex, opIdx);
  if (endOfRightOperand >= 0) {
	  int lenOfRightOperand = endOfRightOperand - opIdx;
	  QString rightOperand = equation.mid(opIdx + 1, lenOfRightOperand).trimmed();
	  pushOperand(rightOperand, absPosOffset + opIdx + 1, lenOfRightOperand);
	  if (m_oParseInfo.parseError != QueueInfo::NOERR) {
		return;
	  }
//	  qDebug() << "   Right operand ends at " << endOfRightOperand << " len=" << lenOfRightOperand << " OP=" << rightOperand;
  }
}

/**
 * Returns the index of the next operand starting at index startIdx.
 * @param equation The equation to analyze
 * @param startIdx The index where we should start the search
 * @return the index to the next found operator
 */
int Calculator::getNextOperatorIndex(const QString &equation, int analysisStartIndex) {
  if (analysisStartIndex >= equation.length()) {
    return -1;
  }
  int opBracket = equation.indexOf(Operator::CHR_BRACKET, analysisStartIndex);
  int opNot = equation.indexOf(Operator::CHR_NOT, analysisStartIndex);
  int opXor = equation.indexOf(Operator::CHR_XOR, analysisStartIndex);
  int opAnd = equation.indexOf(Operator::CHR_AND, analysisStartIndex);
  int opOr = equation.indexOf(Operator::CHR_OR, analysisStartIndex);
  int opBin = equation.indexOf(Operator::CHR_BIN, analysisStartIndex);
  int opBit = equation.indexOf(Operator::CHR_BIT, analysisStartIndex);

  // find two operators to the left for which is valid: op1 left of op2 left of op0, then return op1
  // (numbers: precedency!)
  if (opBracket >= 0) {
    if (opNot >= 0 && opNot < opBracket) {
      int followingXOR = equation.indexOf(Operator::CHR_XOR, opNot);
      if (opNot < followingXOR && followingXOR < opBracket) return opNot;
      int followingAND = equation.indexOf(Operator::CHR_AND, opNot);
      if (opNot < followingAND && followingAND < opBracket) return opNot;
      int followingOR = equation.indexOf(Operator::CHR_OR, opNot);
      if (opNot < followingOR && followingOR < opBracket) return opNot;
    }
    if (opXor >= 0 && opXor < opBracket) {
      int followingAND = equation.indexOf(Operator::CHR_AND, opXor);
      if (opXor < followingAND && followingAND < opBracket) return opXor;
      int followingOR = equation.indexOf(Operator::CHR_OR, opXor);
      if (opXor < followingOR && followingOR < opBracket) return opXor;
    }
    if (opAnd >= 0 && opAnd < opBracket) {
      int followingOR = equation.indexOf(Operator::CHR_OR, opAnd);
      if (opAnd < followingOR && followingOR < opBracket) return opAnd;
    }
    return opBracket;
  }
  if (opNot >= 0) {
    if (opXor >= 0 && opXor < opNot) {
      int followingAND = equation.indexOf(Operator::CHR_AND, opXor);
      if (opXor < followingAND && followingAND < opNot) return opXor;
      int followingOR = equation.indexOf(Operator::CHR_OR, opXor);
      if (opXor < followingOR && followingOR < opNot) return opXor;
    }
    if (opAnd >= 0 && opAnd < opNot) {
      int followingOR = equation.indexOf(Operator::CHR_OR, opAnd);
      if (opAnd < followingOR && followingOR < opNot) return opAnd;
    }
    return opNot;
  }

  if (opXor >= 0) {
    if (opAnd >= 0 && opAnd < opXor) {
      int followingOR = equation.indexOf(Operator::CHR_OR, opAnd);
      if (opAnd < followingOR && followingOR < opXor) return opAnd;
    }
    return opXor;
  }

  if (opAnd >= 0) return opAnd;
  if (opOr >= 0) return opOr;

  // special internal operators without interaction
  if (opBin >= 0) return opBin;
  return opBit;
}

/**
 * We have found a start bracket previously. Now we are looking for its end,
 * considering brackets in between.
 * @param equation The equation to analyze
 * @param startIdx The index where we should start the search
 * @return the index to the close bracket or -1 if no such
 */
int Calculator::getBracketCloseIndex(const QString &equation, int bracketOpenIdx) {
  if (bracketOpenIdx >= equation.length()) {
    return -1;
  }

  int bracketCloseIdx = equation.indexOf(Operator::CHR_BRACKETC, bracketOpenIdx);
  if (bracketCloseIdx < 0) {
    return -1;
  }
  if (bracketOpenIdx < equation.length() - 2) {
    int anotherBracketOpenIdx = equation.indexOf(Operator::CHR_BRACKET, bracketOpenIdx + 1);
    if (anotherBracketOpenIdx >= 0 && anotherBracketOpenIdx < bracketCloseIdx) {
      int anotherBracketCloseIdx = getBracketCloseIndex(equation, anotherBracketOpenIdx);
      return getBracketCloseIndex(equation, anotherBracketCloseIdx + 1);
    }
  }
  return bracketCloseIdx;
}

/**
 * Returns the start index of the operand left to the operator found.
 * @param equation The equation to analyze
 * @param currentOffset The current start index (previous positions are already parsed)
 * @param operatorIdx The position of the operator we are currently working on
 * @return The index to the operand
 */
int Calculator::getLeftOperandIndex(const QString& equation, int analysisStartIndex, int operatorIdx) {
  bool firstCharFound = false;
  for (int i = operatorIdx - 1; i >= analysisStartIndex; i--) {
	  if (Operator::isOP(equation[i])) {
		  return -1;
	  }
	  if (Operator::isDelimiter(equation[i])) {
		  if (firstCharFound) {
			  return i;
		  }
	  } else {
		  if (!firstCharFound) {
			  firstCharFound = true;
		  }
		  if (i == analysisStartIndex) {
			  return i;
		  }
	  }
  }
  return -1;
}

/**
 * Returns the end index of the operand right to the operator found.
 * @param equation The equation to analyze
 * @param currentOffset The current start index (previous positions are already parsed)
 * @param operatorIdx The position of the operator we are currently working on
 * @return The index to the operand
 */
int Calculator::getRightOperandIndex(const QString& equation, int analysisEndIndex, int operatorIdx) {
  bool firstCharFound = false;
  for (int i = operatorIdx + 1; i <= analysisEndIndex; i++) {
	  if (Operator::isOP(equation[i])) {
		  return -1;
	  }
	  if (Operator::isDelimiter(equation[i])) {
		  if (firstCharFound) {
			  return i;
		  }
	  } else {
		  if (!firstCharFound) {
			  firstCharFound = true;
		  }
		  if (i == analysisEndIndex) {
			  return i;
		  }
	  }
  }
  return -1;
}

// add operator to rules
void Calculator::pushOperator(const QString& cOperator, int index, int length)
{
	QueueInfo oInfo;
	oInfo.sOperator = cOperator;
	oInfo.posInEquation = index;
	oInfo.lenInEquation = length;
	oInfo.queueContentType = QueueInfo::VAL_OPERATOR;
	m_oCalcRules.push(oInfo);
}

// add operand to rules
int Calculator::pushOperand(const QString& sOperand, int index, int length)
{
	QueueInfo oInfo;
	oInfo.inputName = sOperand;
	oInfo.posInEquation = index;
	oInfo.lenInEquation = length;

	if (m_dev) {
		const Connector *poConnector;
		// check if item exists as named input, output, const or itself
		if (sOperand == CalcRules::ITEM_THIS) {
			oInfo.outputName = m_dev->getName();
			oInfo.outputID = m_dev->getID();
			oInfo.queueContentType = QueueInfo::VAL_THIS;	// own output
		} else if (NULL != (poConnector = m_dev->getInputConnector(sOperand))) {
			oInfo.outputName = poConnector->getName();
			oInfo.outputID = poConnector->getConnectorID();
			oInfo.queueContentType = QueueInfo::VAL_INPUT;
		} else if (NULL != (poConnector = m_dev->getOutputConnector(sOperand))) {
			oInfo.outputName = poConnector->getName();
			oInfo.outputID = poConnector->getConnectorID();
			oInfo.queueContentType = QueueInfo::VAL_OUTPUT;
		} else if (isNumber(sOperand)) {
			oInfo.value = sOperand.toInt();
			oInfo.queueContentType = QueueInfo::VAL_NUMBER;
		} else {
			oInfo.parseError = QueueInfo::ERR_NTERM;
		}
	}
	return m_oCalcRules.push(oInfo);
}

// push an input value to the stack
void Calculator::push(int val)
{
	checkStackSize(itemsOnStack + 1);
	if (val != KlogicGlobal::Device::TRISTATE) {
		m_poStack[itemsOnStack] = val;
		itemsOnStack++;
	} else {
		if (m_oCalcRules.isStatic() && m_oCalcRules[0].sOperator == Operator::CHR_INTERFACE) {
			m_poStack[itemsOnStack] = val;
			itemsOnStack++;
		}
	}
}

int Calculator::getInputValue(int idx)
{
	if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_THIS)
		return m_dev->output();
	else if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_OUTPUT)
		return m_dev->output(m_oCalcRules[idx].outputID);
	else if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_INPUT)
		return m_dev->input(m_oCalcRules[idx].outputID);
	else if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_NUMBER)
		return m_oCalcRules[idx].value;
	return undefined_result;
}

/**
 * This method calculates a single output of a device.
 * Either it is a named output (id > 0) or the unnamed one (id = 0)
 *
 * If the device has a static equation such as AND,
 * all input values are pushed to the stack first,
 * and then the static equation is applied until only one value is left on the stack
 * for the unnamed output.
 *
 * If a named output is calculated, the equation delivers the input values at the right place
 * by the calculation stack which contains all operators and operands in RPN sequence, and the operands
 * which must be applied to the input values afterwards.
 *
 * @param tristate_active whether tristate is active or not
 * @return the calculated value (which is stored in the result fifo of the device in addition)
 */
int Calculator::calculate(bool tristate_active)
{	int bin_pow = 1;
	bool hadTristate = false;

	if (tristate_active && isTristate()) {
		setValue(KlogicGlobal::Device::TRISTATE);
		return KlogicGlobal::Device::TRISTATE;
	}

	setValue(undefined_result);

	int iTotal;
	if (m_oCalcRules.isStatic()) {
		m_dev->pushInputValuesToCalculationStack(this);
		iTotal = itemsOnStack;
	} else {
		iTotal = m_oCalcRules.size();
	}

	if (iTotal <= 0) {
		return undefined_result;		// nothing to do
	}
/*
	qDebug() << "Stack of " << m_dev->type() << ":";
	for (int idx = 0; idx < iTotal; ++idx) {
		if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_THIS)
			qDebug() << idx << ":" << m_oCalcRules[idx].inputName << "(tO)";
		else if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_OUTPUT)
			qDebug() << idx << ":" << m_oCalcRules[idx].inputName << "(O)";
		else if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_INPUT)
			qDebug() << idx << ":" << m_oCalcRules[idx].inputName << "(I)";
		else if (m_oCalcRules[idx].queueContentType == QueueInfo::VAL_NUMBER)
			qDebug() << idx << ":" << m_oCalcRules[idx].value << "(VAL)";
		else {
			if (m_oCalcRules[idx].sOperator == Operator::CHR_NONE) {
				qDebug() << idx << ":" << "[NONE]" << "(OP)";
			} else if (m_oCalcRules[idx].sOperator == Operator::CHR_INTERFACE) {
				qDebug() << idx << ":" << "[IFace]" << "(OP)";
			} else {
				qDebug() << idx << ":" << m_oCalcRules[idx].sOperator << "(OP)";
			}
		}
	}
	qDebug() << "\n";
*/
	int stackIdx = itemsOnStack - 1;
	for (int idx = 0; idx < iTotal; ++idx) {
		if (m_oCalcRules[idx].queueContentType != QueueInfo::VAL_OPERATOR) {
			checkStackSize(++stackIdx);
			m_poStack[stackIdx] = getInputValue(idx);
			if (m_poStack[stackIdx] == KlogicGlobal::Device::TRISTATE) {
				hadTristate = true;
			}
		} else {
			const QString& op = m_oCalcRules[idx].sOperator;
			if (op != Operator::CHR_NOT && stackIdx == 0) {
				// NOT is the only one which works on one operator only. All others can break here.
				continue;
			}
			if (op == Operator::CHR_AND) {
				m_poStack[stackIdx - 1] = m_poStack[stackIdx - 1] & m_poStack[stackIdx];
				stackIdx--;
			} else if (op == Operator::CHR_OR) {
				m_poStack[stackIdx - 1] = m_poStack[stackIdx - 1] | m_poStack[stackIdx];
				stackIdx--;
			} else if (op == Operator::CHR_XOR) {
				m_poStack[stackIdx - 1] = m_poStack[stackIdx - 1] ^ m_poStack[stackIdx];
				stackIdx--;
			} else if (op == Operator::CHR_BIN) {
				if (m_poStack[stackIdx - 1] == KlogicGlobal::Device::TRISTATE) {
					if (m_poStack[stackIdx] != KlogicGlobal::Device::TRISTATE) {
						m_poStack[stackIdx - 1] = m_poStack[stackIdx] * Tool::pow2(bin_pow);
					}
				} else {
					if (m_poStack[stackIdx] != KlogicGlobal::Device::TRISTATE) {
						m_poStack[stackIdx - 1] += m_poStack[stackIdx] * Tool::pow2(bin_pow);
					}
				}
				bin_pow++;
				stackIdx--;
			} else if (op == Operator::CHR_BIT) {
				if (m_poStack[stackIdx - 1] != KlogicGlobal::Device::TRISTATE) {
					m_poStack[stackIdx - 1] = (m_poStack[stackIdx - 1] & Tool::pow2(m_poStack[stackIdx])) > 0;
				}
				stackIdx--;
			} else if (op == Operator::CHR_NOT) {
				if (m_poStack[stackIdx] == 1) m_poStack[stackIdx] = 0;
				else m_poStack[stackIdx] = 1;
			} else if (op == Operator::CHR_NONE) {
			} else if (op == Operator::CHR_INTERFACE) {
				// use OR for interfaces; propagate TRISTATE
				// but do no calculation with TRISTATE values!
				if (m_poStack[stackIdx - 1] == KlogicGlobal::Device::TRISTATE) {
					m_poStack[stackIdx - 1] = m_poStack[stackIdx];
				}
				if (m_poStack[stackIdx] != KlogicGlobal::Device::TRISTATE && m_poStack[stackIdx - 1] != KlogicGlobal::Device::TRISTATE) {
					m_poStack[stackIdx - 1] |= m_poStack[stackIdx];
				} else {
					hadTristate = true;
				}
				stackIdx--;
				if (stackIdx == 0 && m_poStack[0] == KlogicGlobal::Device::TRISTATE) {
					hadTristate = true;
				}
			} else {
				qFatal("unknown operator");
			}
		}
	}

	if (stackIdx != 0) {
		if (stackIdx > 0) {
			// should never happen
			qWarning("Calculator::calculate: stack iPlane is positive!\n");
			return undefined_result;
		} else if (hadTristate) {
			// special case: interface m_dev
			if (m_oCalcRules.isStatic() && m_oCalcRules[0].sOperator == Operator::CHR_INTERFACE) {
				setValue(KlogicGlobal::Device::TRISTATE);
				return KlogicGlobal::Device::TRISTATE;
			}
			setValue(undefined_result);
			return undefined_result;
		} else if (stackIdx < 0) {
			// should never happen
			qWarning("Calculator::calculate: stack iPlane is negative!\n");
			return undefined_result;
		}
	}

	int iVal = m_poStack[stackIdx];
	setValue(iVal);
	itemsOnStack = 0;
	return iVal;
}

void Calculator::checkStackSize(int stackIdx)
{
	while (stackIdx + 1 >= currentStackSize) {
		int *poStack = new int[currentStackSize + 10];
		memcpy(poStack, m_poStack, currentStackSize * sizeof(int));
		delete[] m_poStack;
		m_poStack = poStack;
		currentStackSize += 10;
	}
}

unsigned int Calculator::getCount() const
{
	return m_oCalcRules.size();
}
