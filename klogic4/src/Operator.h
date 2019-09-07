#ifndef __Operator_CLASS__
#define __Operator_CLASS__

#include <QChar>
#include <QString>
class Operator
{
public:
	static bool isOP(const QString &c)
	{
		return (c == CHR_AND || c == CHR_OR || c == CHR_XOR ||
			c == CHR_NOT || c == CHR_BIN || c == CHR_BIT ||
			c == CHR_NONE || c == CHR_INTERFACE);
	}

	static bool isDelimiter(const QString &c)
	{
		return (c == CHR_DELIMITER || c == CHR_BRACKET || c == CHR_BRACKETC || isOP(c));
	}

	static bool isBlank(const QString &c)
	{
		return (c == CHR_DELIMITER);
	}

	// return type of operation:
	//   OP_LR operator needs left and right operand
	//   OP_R operator needs right operand
	static int opType(QString c)
	{
		if (c == CHR_AND || c == CHR_OR || c == CHR_XOR || c == CHR_BIN || c == CHR_BIT) {
			return TYPE_LR;
		}
		if (c == CHR_NOT) {
			return TYPE_R;
		}
		if (c == CHR_BRACKET) {
			return TYPE_PARENTHESIS;
		}
		return UNDEF;
	}

	static bool containsOp(const QString &str)
	{
		return str.contains(CHR_AND) || str.contains(CHR_OR) || str.contains(CHR_XOR) ||
		  str.contains(CHR_NOT) || str.contains(CHR_BIN) || str.contains(CHR_BIT) ||
		  str.contains(CHR_NONE) || str.contains(CHR_INTERFACE);
	}

	static const QString CHR_DELIMITER;
	static const QString CHR_BRACKET;
	static const QString CHR_BRACKETC;
	static const QString CHR_AND;
	static const QString CHR_OR;
	static const QString CHR_XOR;
	static const QString CHR_NOT;
	static const QString CHR_BIN;
	static const QString CHR_BIT;
	static const QString CHR_NONE;
	static const QString CHR_INTERFACE;

	// operation types
	enum OperatorType {UNDEF, TYPE_PARENTHESIS, TYPE_LR, TYPE_R };
};

#endif
