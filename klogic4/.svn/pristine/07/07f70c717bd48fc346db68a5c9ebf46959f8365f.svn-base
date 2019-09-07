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
/*************************************************/
#ifndef __SYMBOLOPSTACK_CLASS__
#define __SYMBOLOPSTACK_CLASS__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <map>
#include <set>
#include <iostream>

using namespace std;

class Device;
class QString;
class StackInfo; // see value.h
class Fifo;
template <class T> class KlogicList;
#include <calculator.h>
#include<value.h>
#include<tool.h>

// Pattern is a single input pattern and its result
// (no, it's a struct ..)
// example:
// input vector	<0 1 0 1>	(input as bits)
// iInput	<5>		(input as int)
// iResult	<1>		(output value for this input)
class Pattern: public vector<int>
{
	friend ostream& operator<< (ostream&, const Pattern& oPat);

public:
	Pattern()
	{
		iResult = 0;
		iInput = 0;
	}

	int iResult;
	int iInput;
};

ostream& operator<< (ostream&, const Pattern& oPat);

// Patterns is a collection of all possible input patterns
class Patterns: public vector<Pattern>
{
public:
	Patterns()
	{
		m_iVarCnt = 0;
	}

	void setVarCount(int iVarCnt)
	{
		m_iVarCnt = iVarCnt;
		clear();
		int iSize = Tool::pow2(iVarCnt);
		resize(iSize);
		m_vCurrentTable.resize(iSize);

		// all available input bits
		m_oBitSet.clear();
		m_oBitSet.insert(0);
		for (int i = 0; i < iVarCnt; i++)
			m_oBitSet.insert(Tool::pow2(i));
	}

	// exchange columns of patterns:
	// result value is important to find neighbors of equal value
	// switch input column i with input column j
	void createMatchPattern(int i, int j)
	{
		int xPow = Tool::pow2(i);
		int xCPow = Tool::pow2complement(m_iVarCnt - 1, i);
		int yPow = Tool::pow2(j);
		int yCPow = Tool::pow2complement(m_iVarCnt - 1, j);
		for (unsigned int idx = 0; idx < size(); idx++) {
			int new_idx = idx;
			if (i != j) {
				if (idx & xPow) new_idx |= yPow;	// set bit j
				else new_idx &= yCPow;			// clear bit j

				if (idx & yPow) new_idx |= xPow;	// set bit i
				else new_idx &= xCPow;			// clear bit i
			}
			m_vCurrentTable[new_idx] = idx;
		}
	}

	// original pattern index of pattern with exchanged columns
	int getMatchIndex(int idx)
	{
		return m_vCurrentTable[idx];
	}

	// result of a pattern with exchanged columns
	int getMatchResult(int idx)
	{
		Pattern &p = (*this)[m_vCurrentTable[idx]];
		return p.iResult;
	}

	// input of a pattern with exchanged columns
	int getMatchInput(int idx)
	{
		Pattern &p = (*this)[m_vCurrentTable[idx]];
		return p.iInput;
	}

	// debug (pattern of current equation)
	void patternDump()
	{
		cout << "idx\tvalue" << endl;
		for (unsigned int idx = 0; idx < size(); idx++) {
			Pattern &p = (*this)[idx];
			cout << '(' << idx << ')';
			cout << p << endl;
		}
	}

	// debug (pattern of current equation with exchanged input column)
	void tableDump()
	{
		cout << "idx\tvalue" << endl;
		for (unsigned int idx = 0; idx < size(); idx++) {
			Pattern &p = (*this)[m_vCurrentTable[idx]];
			cout << '(' << m_vCurrentTable[idx] << ')';
			cout << p << endl;
		}
	}

private:
	// exchange columns to find groups
	vector<int> m_vCurrentTable;
	set<int> m_oBitSet;
	int m_iVarCnt;
};

// contains indexes to Patterns
class Group: public set<int>
{
public:
	bool operator <= (const Group& other) const
	{
		if (other.size() < size())
			return false;
		for(iterator iter = begin(); iter != end(); iter++) {
			if (other.find(*iter) == other.end())
				return false;
		}
		return true;
	}

	bool has(int iValue) const
	{
		return (find(iValue) != end());
	}

	void dump() const
	{
		for(iterator iter = begin(); iter != end(); iter++)
			cout << *iter << ' ';
		cout << endl;
	}
};

// contains Group sorted by group size
class Groups: public multimap<int, Group>
{
public:
	Groups()
	{
		m_iVarCnt = 0;
	}

	// needed for validating groups
	void setVarCount(int iVarCnt)
	{
		m_iVarCnt = iVarCnt;
	}

	// remove groups that are a part of another group
	void removeEnclosed()
	{
		iterator iter = begin();
		while (iter != end()) {
			if (isEnclosed(iter->second, iter)) {
				erase(iter++);
			} else
				++iter;
		}
	}

	// remove groups that are part of one ore more others
	void removeContained()
	{
		iterator iter = begin();
		while (iter != end()) {
			const Group& g = (iter->second);

			// look if group is part of one or more others
			bool bComplete = true;
			for(Group::const_iterator giter = g.begin(); giter != g.end(); ++giter) {
				// each index of current group must be part of another
				if (!isContained(*giter, iter)) {
					bComplete = false;
					break;
				}
			}
			if (bComplete) {
				erase(iter++);
			} else
				++iter;
		}
	}

	// check if index is contained in groups
	bool has(int value) const
	{
		for (const_iterator iter = begin(); iter != end(); ++iter) {
			if ((iter->second).has(value))
				return true;
		}
		return false;
	}

	// valid sizes are all pow2 lower pow2(#input)
	// groups of size 1 are handled separately
	unsigned int isValid(int size, int startPos) const
	{
		if (size < 2) return 0;

		unsigned int exp = Tool::exp(size);
		if (exp > Tool::pow2(m_iVarCnt))
			return 0;	// impossible

		int iGrpSz = Tool::pow2(exp);
		while (startPos % iGrpSz != 0) {
			exp--;
			iGrpSz = Tool::pow2(exp);
		}
		return iGrpSz;
	}

private:
	bool isEnclosed(const Group& g, const const_iterator& originIter) const
	{
		for (const_iterator iter = begin(); iter != end(); ++iter) {
			if (iter != originIter) {
				if (g <= iter->second)
					return true;
			}
		}
		return false;
	}

	bool isContained(int value, const const_iterator& originIter) const
	{
		for (const_iterator iter = begin(); iter != end(); ++iter) {
			if (iter != originIter) {
				if ((iter->second).has(value))
					return true;
			}
		}
		return false;
	}

	int m_iVarCnt;
};

class SymbolicCalc : public Calculator {
public:
	SymbolicCalc();
	virtual ~SymbolicCalc();

	virtual const QueueInfo& parse();

	QString calculateSymbolic();                    // calculate the stack symbolic, return the new equation
	QString calculateSymbolicNormalized(int, bool); // build minimal normalized equation, return the new equation

	const QString& getSymbol(int) const;		// return the n-th symbol in the equation
	const vector<QString>& getInputNames();		// inputs found in equation
	const Groups& getGroups();			// groups found in patterns
	const Patterns& getPatterns();			// patterns calculated for equation
	CalcRules& getCalcRules();			// parse results for equation for free modification

protected:
	virtual int pushOperand(const QString&, int index, int length);
	virtual int getInputValue(int idx);

private:
	static const QString EMPTYSTRING;

	void scanInputList();
	void calculatePattern();
	static int needsParenthesis(const QString&, const QString&);

	// input names and stack position mapper
	vector<QString> m_vInputNames;
	map<int, int> m_mInputStackMap;
	map<QString, int> m_mInputAvail;

	Patterns *m_poPatterns;
	Groups *m_poGroups;

	Pattern m_vCurrentPattern;

	vector<QString> m_oSStack;
};

#endif
