/*************************************************/
/* these classes are designed for calculating    */
/* boolean equations in symbolic mode -          */
/* and to create normalized equations from       */
/* mixed ones                                    */
/* used to draw karnaugh diagrams                */
/* used to calculate normalized equations        */
/*                                               */
/* (c) Andreas Rostin                            */
/* 11-12-2003                                    */
/*                                               */
/* (base hack creation: 04-09-2001)              */
/*************************************************/

#include <qstring.h>
#include <device.h>
#include <tool.h>

#include <symbolicCalc.h>

const QString SymbolicCalc::EMPTYSTRING;

ostream& operator<< (ostream& os, const Pattern& oPat)
{
	for (unsigned int j = 0; j < oPat.size(); j++) {
		os << ' ' << oPat[j];
	}
	os << " = " << oPat.iResult << endl;
	return os;
}

SymbolicCalc::SymbolicCalc()
{
	// use heap because we need really huge amounts of memory - maybe - 
	m_poPatterns = new Patterns();
	m_poGroups = new Groups();
}

SymbolicCalc::~SymbolicCalc()
{
	delete m_poPatterns;
	delete m_poGroups;
}

// symbolic stack calculation
// returns the equation
QString SymbolicCalc::calculateSymbolic()
{
	int lpar, rpar;
	QString sItem;

	unsigned int iTotal;
	if (m_oCalcRules.isStatic())
		iTotal = itemsOnStack;
	else
		iTotal = m_oCalcRules.size();

	int iPlane = itemsOnStack - 1;
	for (unsigned int i = 0; i < iTotal; ++i) {
		iPlane = m_oSStack.size() - 1;
		const QString& op = m_oCalcRules[i].sOperator;
		if (op == Operator::CHR_AND ||
		    op == Operator::CHR_OR ||
		    op == Operator::CHR_XOR ||
		    op == Operator::CHR_BIN ||
		    op == Operator::CHR_BIT) {
		  lpar = needsParenthesis(m_oSStack[iPlane - 1], m_oCalcRules[i].sOperator);
		  rpar = needsParenthesis(m_oSStack[iPlane], m_oCalcRules[i].sOperator);
		  if (!lpar) sItem = m_oSStack[iPlane - 1];
		  else sItem = '(' + m_oSStack[iPlane - 1] + ')';
		  sItem += ' ' + m_oCalcRules[i].sOperator + ' ';
		  if (!rpar) sItem += m_oSStack[iPlane];
		  else sItem = '(' + m_oSStack[iPlane] + ')';

		  m_oSStack[iPlane - 1] = sItem;
		  iPlane--;
		} else if (op == Operator::CHR_NOT) {
		  rpar = needsParenthesis(m_oSStack[iPlane], Operator::CHR_NOT);
		  sItem = Operator::CHR_NOT;
		  if (!rpar) sItem += m_oSStack[iPlane];
		  else sItem = '(' + m_oSStack[iPlane] + ')';
		  m_oSStack[iPlane] = sItem;
		  iPlane--;
		} else if (op == Operator::CHR_NONE ||
		           op == Operator::CHR_INTERFACE) {
		} else {
		  m_oSStack.push_back(m_oCalcRules[i].inputName);
		  iPlane++;
		}
	}

	if (iPlane != 0) {
		qWarning() << "invalid iPlane in SymbolicCalc::calculateSymbolic(..)";
		if (iPlane < 0) {	// should never occur!
			m_oSStack.clear();
			return "";
		}
	}
	QString sRet = m_oSStack[iPlane];
	m_oSStack.clear();
	return sRet;
}

// return a minimized and normalized equation
// iSubjunctive: search groups of 0 (disjunctive) or 1 (subjunctive)
// bSecure: ensure groups do always overlap
QString SymbolicCalc::calculateSymbolicNormalized(int iSubjunctive, bool bSecure)
{
	calculatePattern();

	int iVarCnt = m_vInputNames.size();
	int iPatCnt = m_poPatterns->size();

	if (!m_poGroups)
		m_poGroups = new Groups();
	m_poGroups->clear();
	m_poGroups->setVarCount(iVarCnt);
	Group oCurrentGroup;

	// search groups
	// group is a sequence of neighbor patterns with same output value
	// neghborhood is defined as one and only one bit is toggled in input
	// observe that every <group-size> patterns, sequence is interrupted (2 bit toggle)
	// all possible groups are detected by exchanging inputs (colums) in pattern list
	// thus group start must be aligned at group size
	// additionally, only pow2 group-sizes are allowed (1,2,4,8,..)
	//
	// example (1st table is the originally created pattern)
	// idx     value
	// (0) 0 0 = 0 <-- 1st pattern of 1st pattern group with size 2 (disjunctive)
	// (2) 1 0 = 0 <-- 2nd pattern of 1st pattern group with size 2 (disjunctive)
	// <---------- two bits will toggle for groups of size 2
	// (2) 0 1 = 0
	// (3) 1 1 = 1 <-- single pattern group (subjunctive)
	//
	// now columns are exchanged, row-index re-calculated (2nd table)
	// idx     value
	// (0) 0 0 = 0 <-- 1st pattern of 2nd pattern group with size 2 (disjunctive)
	// (2) 0 1 = 0 <-- 2nd pattern of 2nd pattern group with size 2 (disjunctive)
	// <---------- two bits will toggle for groups of size 2
	// (1) 1 0 = 0
	// (3) 1 1 = 1 <-- single pattern group (subjunctive), but still exists, will be discarded
	//
	// see karnaugh graphical analytical method for more details
	//
	for (int iVarIdx1 = 0; iVarIdx1 < iVarCnt; iVarIdx1++) {
		int iStartIdx = iVarIdx1 + 1;
		if (iVarIdx1 == 0) iStartIdx = 0;	// analyze original pattern once
		for (int iVarIdx2 = iStartIdx; iVarIdx2 < iVarCnt; iVarIdx2++) {
			// create result table with exchanged cols, then find sequences of same result
			m_poPatterns->createMatchPattern(iVarIdx1, iVarIdx2);
			//m_poPatterns->tableDump();

			int iGrpStartIdx = 0;
			int iGrpEndIdx = 0;
			while (iGrpStartIdx < iPatCnt) {
				iGrpEndIdx = iGrpStartIdx;
				int iFoundGrpSize = 0;
				while (iGrpEndIdx < iPatCnt) {
					// next pattern needs the same output
					if (m_poPatterns->getMatchResult(iGrpEndIdx) != iSubjunctive)
						break;

					iFoundGrpSize++;
					iGrpEndIdx++;
				}
				int iTakeGroupSize = m_poGroups->isValid(iFoundGrpSize, iGrpStartIdx);
				if (iTakeGroupSize) {
					// create new group
					iGrpEndIdx = iGrpStartIdx + iTakeGroupSize;
					oCurrentGroup.clear();
					for (int l = iGrpStartIdx; l < iGrpEndIdx; l++)
						oCurrentGroup.insert(m_poPatterns->getMatchIndex(l));
					m_poGroups->insert(Groups::value_type(iTakeGroupSize, oCurrentGroup));
				}
				iGrpStartIdx += 2;
			}
		}
	}

	// remove redundant groups
	if (bSecure) {
		// remove groups entirely contained in another group
		m_poGroups->removeEnclosed();
	} else {
		// remove groups contained in more than one other group
		m_poGroups->removeContained();
	}

	// add single bit groups
	for (int i = 0; i < iPatCnt; i ++) {
		if ((*m_poPatterns)[i].iResult == iSubjunctive && (*m_poPatterns)[i + 1].iResult != iSubjunctive) {
			if (!m_poGroups->has(i)) {
				oCurrentGroup.clear();
				oCurrentGroup.insert(i);
				m_poGroups->insert(Groups::value_type(1, oCurrentGroup));
			}
		}
	}

	// create an equation from the groups found
	QString sEquation = "";
	for (Groups::const_iterator groupsIter = m_poGroups->begin(); groupsIter != m_poGroups->end(); ++groupsIter) {
		const Group& oGrp = groupsIter->second;
		if (groupsIter == m_poGroups->begin()) {
			if (!iSubjunctive)
				sEquation.append("(");
		} else {
			if (iSubjunctive)
				sEquation.append(" + ");
			else
				sEquation.append(") & (");
		}

		bool bFirst = true;
		// scan group patterns column wise (input bits of that column)
		// column term only needed if bit changes within column within group
		for (int iColIdx = 0; iColIdx < iVarCnt; iColIdx++) {
			Group::const_iterator groupIter = oGrp.begin();
			if (groupIter == oGrp.end())
				continue;

			int iPatIdx = (*groupIter);
			const Pattern& oPat = (*m_poPatterns)[iPatIdx];
			int iBit = oPat[iColIdx];
			bool bNeeded = true;

			// if bit of this input toggles, bit is not relevant for equation
			++groupIter;
			while (groupIter != oGrp.end()) {
				iPatIdx = (*groupIter);
				const Pattern& oPat2 = (*m_poPatterns)[iPatIdx];
				if (iBit != oPat2[iColIdx]) {
					bNeeded = false;
					break;
				}
				++groupIter;
			}

			if (bNeeded) {
				if (bFirst) {
					bFirst = false;
				} else {
					if (iSubjunctive)
						sEquation.append(" & ");
					else
						sEquation.append(" + ");
				}

				if ((iSubjunctive && iBit) || (!iSubjunctive && !iBit)) {
					sEquation.append(m_vInputNames[iColIdx]);
				} else {
					sEquation.append("/");
					sEquation.append(m_vInputNames[iColIdx]);
				}
			}
		}
	}
	if (sEquation.length() && !iSubjunctive)
		sEquation += ")";

	return sEquation;
}

const QueueInfo& SymbolicCalc::parse()
{
	m_vInputNames.clear();
	m_mInputStackMap.clear();
	m_mInputAvail.clear();
	return Calculator::parse();
}

// return the first/next symbol of the queue
const QString& SymbolicCalc::getSymbol(int no) const
{
	if (no >= 0 && no < (int)m_vInputNames.size())
		return m_vInputNames[no];
	return EMPTYSTRING;
}

int SymbolicCalc::pushOperand(const QString& str, int index, int length)
{
	int iPos = Calculator::pushOperand(str, index, length);
	map<QString, int>::iterator iter = m_mInputAvail.find(str);
	if (iter != m_mInputAvail.end()) {
		m_mInputStackMap[iPos] = iter->second;
	} else {
		m_vInputNames.push_back(str);
		m_mInputStackMap[iPos] = m_vInputNames.size() - 1;
		m_mInputAvail[str] = m_vInputNames.size() - 1;
	}
	return iPos;
}

// stack calculation for the whole number of possible patterns
void SymbolicCalc::calculatePattern()
{
	if (!m_poPatterns)
		m_poPatterns = new Patterns();
	m_poPatterns->clear();
	int iVarCnt = m_vInputNames.size();
	m_poPatterns->setVarCount(iVarCnt);

	// ****** calculate patterns ******
	for (unsigned int i = 0; i < m_poPatterns->size(); i++) {
		m_vCurrentPattern.clear();
		// create new input vector containing binary representation of i
		// (needed for creating equations efficiently)
		for (int j = 0; j < iVarCnt; j++) {
			if (i & Tool::pow2(j)) m_vCurrentPattern.push_back(1);
			else m_vCurrentPattern.push_back(0);
		}
		// calculate new pattern
		m_vCurrentPattern.iInput = i;
		m_vCurrentPattern.iResult = calculate(true);
		(*m_poPatterns)[i] = m_vCurrentPattern;
	}
}

// simulated input values requested by calculate(), see calculatePattern()
int SymbolicCalc::getInputValue(int idx)
{
	map<int, int>::iterator mapper = m_mInputStackMap.find(idx);
	if (mapper != m_mInputStackMap.end())
	  return m_vCurrentPattern[mapper->second];
	else {
	  qCritical() << "SymbolicCalc::getInput: no such input index <" << idx << "> in mapper m_mInputStackMap";
	  return 0;
	}
}

// figure out whether string needs an parenthesis
// assumes that there are no parenthesis errors!
int SymbolicCalc::needsParenthesis(const QString& eq, const QString& op)
{
	int bracketOpen = eq.indexOf(Operator::CHR_BRACKET);
	if (bracketOpen == -1) {
	  if (op == Operator::CHR_NOT) {
	    if (eq.indexOf(Operator::CHR_XOR) >= 0) return 2;
	    if (eq.indexOf(Operator::CHR_AND) >= 0) return 2;
	    if (eq.indexOf(Operator::CHR_OR) >= 0) return 2;
	  } else if (op == Operator::CHR_XOR) {
	    if (eq.indexOf(Operator::CHR_AND) >= 0) return 2;
	    if (eq.indexOf(Operator::CHR_OR) >= 0) return 2;
	  } else if (op == Operator::CHR_AND) {
	    if (eq.indexOf(Operator::CHR_OR) >= 0) return 2;
	  }
	  return 0;
	}
	
	int bracketClose = eq.indexOf(Operator::CHR_BRACKETC);
	if (bracketClose == -1 || bracketClose <= bracketOpen) {
	  qCritical() << "Unexpected error: Equation has parenthesis errors: " << eq;
	  return 0;
	}
	
	if (op == Operator::CHR_NOT) {
	  if (eq.indexOf(Operator::CHR_XOR) >= 0) return 2;
	  if (eq.indexOf(Operator::CHR_AND) >= 0) return 2;
	  if (eq.indexOf(Operator::CHR_OR) >= 0) return 2;
	} else if (op == Operator::CHR_XOR) {
	  if (eq.indexOf(Operator::CHR_AND) >= 0) return 2;
	  if (eq.indexOf(Operator::CHR_OR) >= 0) return 2;
	} else if (op == Operator::CHR_AND) {
	  if (eq.indexOf(Operator::CHR_OR) >= 0) return 2;
	}
	return 0;
}

// return group information
const Groups& SymbolicCalc::getGroups()
{
	if (!m_poGroups)
		calculateSymbolicNormalized(1,0);
	return *m_poGroups;
}

// return input name information
const vector<QString>& SymbolicCalc::getInputNames()
{
	return m_vInputNames;
}

// return parse information
CalcRules& SymbolicCalc::getCalcRules()
{
	return m_oCalcRules;
}

// return pattern information
const Patterns& SymbolicCalc::getPatterns()
{
	if (!m_poPatterns)
		calculatePattern();
	return *m_poPatterns;
}

