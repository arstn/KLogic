#ifndef __TOOL_CLASS
#define __TOOL_CLASS

#include <map>
#include <set>
using namespace std;

#include <qstring.h>

class Referenced
{
public:
	Referenced();
	virtual ~Referenced();

	int addRef() const;
	int relRef() const;
private:
	mutable int m_iRef;
};

class BitSet: public set<int>
{
public:
	BitSet();
};

class Tool
{
public:
	static unsigned int pow2(int exp);
	static unsigned int exp(unsigned int value);	// returns nearest lower match

	static int pow2complement(int base, int clearbit);
	static QString bitString(unsigned char byte, int len);

	static int singleBitToggled(int iVal1, int iVal2);
	static int singleBitSet(int iXORVal);
	static int xOr(int iVal1, int iVal2);
	static BitSet m_oBitSet;

	static const int MAXPOW = 30;
	static unsigned int powTab[MAXPOW + 1];
};

// this class can be used to map integers to strings
class IDMap
{
public:
	IDMap() {}
 
	~IDMap() {
	}
 
	void init() {
		m_oIntMap.clear();
		m_oStrMap.clear();
	}

	// add int id mapping
	inline void fileID(int file_id, int iid) {
		m_oIntMap[file_id] = iid;
	}

	// add string id mapping
	// if string can be converted to an integer, that one is used for mapping
	inline void fileID(QString sid, int iid) {
		m_oStrMap[sid] = iid;
	}

	// map int id's
	inline int mapID(int file_id) {
		return m_oIntMap[file_id];
	}

	// map a string to an id
	// if string can be converted to an integer, that one is used for mapping
	inline int mapID(QString sid) {
		return m_oStrMap[sid];
	}
private:
	map<int, int> m_oIntMap;
	map<QString, int> m_oStrMap;
};

#endif
