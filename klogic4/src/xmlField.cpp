/*************************************************/
/* methods for class XmlField                    */
/*                                               */
/* Device XML export                             */
/*                                               */
/* Andreas Rostin                                */
/* 20.08.01                                      */
/*************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <qstring.h>
#include <qregexp.h>
#include <qpoint.h>
#include <qdatetime.h>
#include <QDebug>

#include "xmlField.h"
#include "xmlExport.h"
#include "xmlSymbolMap.h"
#include "KlogicGlobal.h"

const QString XmlField::XML_HEADER = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

// static
QString& XmlField::unquote(QString& content)
{
	content.replace(QRegExp("&amp;"), "&");
	content.replace(QRegExp("&lt;"), "<");
	content.replace(QRegExp("&gt;"), ">");
	content.replace(QRegExp("&quot;"), "\"");
	content.replace(QRegExp("&apos;"), "'");

	content.replace(QRegExp("&#167;"), "�");
	content.replace(QRegExp("&#228;"), "�");
	content.replace(QRegExp("&#246;"), "�");
	content.replace(QRegExp("&#252;"), "�");
	content.replace(QRegExp("&#196;"), "�");
	content.replace(QRegExp("&#214;"), "�");
	content.replace(QRegExp("&#220;"), "�");
	return content;
}

// static
QString& XmlField::quote(QString& content)
{
	content.replace(QRegExp("&"), "&amp;");
	content.replace(QRegExp("<"), "&lt;");
	content.replace(QRegExp(">"), "&gt;");
	content.replace(QRegExp("\""), "&quot;");
	content.replace(QRegExp("'"), "&apos;");

	content.replace(QRegExp("�"), "&#228;");
	content.replace(QRegExp("�"), "&#246;");
	content.replace(QRegExp("�"), "&#252;");
	content.replace(QRegExp("�"), "&#196;");
	content.replace(QRegExp("�"), "&#214;");
	content.replace(QRegExp("�"), "&#220;");
	return content;
}

XmlField::XmlField()
{
	clear();
	setLevelStrings(0);
}

XmlField::XmlField(int _level)
{
	clear();
	setLevelStrings(_level);
}

XmlField::XmlField(QString _name, int _level)
{
	clear();
	name = _name;
	setLevelStrings(_level);
}

XmlField::XmlField(QString _name, QString _content, int _level)
{
	clear();
	name = _name;
	content = _content;
	setLevelStrings(_level);
}

int XmlField::getLevel()
{
	return level;
}

void XmlField::clear()
{
	name.truncate(0);
	content.truncate(0);
	att.truncate(0);
	no_content = true;
	hasMeta = false;
}

void XmlField::setLevelStrings(int _level)
{
	level = _level;
	if (level == -1) {
		level_prefix.truncate(0);
		level_suffix = "\n";
		level_content_prefix.truncate(0);
		level_content_suffix = "\n";
		return;
	}

	level_prefix.truncate(0);
	for (int i = 0; i < level; i++)
		level_prefix += "\t";

	level_suffix = "\n";
	level_suffix += level_prefix;

	level_content_prefix = level_prefix;
	level_content_prefix += "\t";

	level_content_suffix = "\n";
	level_content_suffix += level_content_prefix;

	single_line = true;
}

void XmlField::set(QString _name)
{
	name = _name;
	content.truncate(0);
	att.truncate(0);
}

void XmlField::set(QString _name, QString _content)
{
	name = _name;
	content = _content;
	att.truncate(0);
}

void XmlField::set(QString _name, int _content)
{
	name = _name;
	content.sprintf("%d", _content);
	att.truncate(0);
}

void XmlField::set(QString _name, QPoint _content)
{
	name = _name;
	content.sprintf("%d:%d", _content.x(), _content.y());
	att.truncate(0);
}

void XmlField::set(QString _name, int cnt, char *bin_fld)
{	QString buf;

	name = _name;
	content.sprintf("%d", cnt);
	for(int i = 0; i < cnt; i++) {
		buf.sprintf(";%d", bin_fld[i]);
		content += buf;
	}
	att.truncate(0);
}

void XmlField::setName(QString _name)
{
	name = _name;
}

void XmlField::setContent(QString _content)
{
	content = _content;
}

void XmlField::addContent(QString _content)
{
	content += _content;
}

void XmlField::setAttribute(QString _att)
{
	att = _att;
}

void XmlField::addAttribute(QString _att_name, QString _content)
{
	QString newAttr = _att_name + "=\"" + _content + "\"";
	if (att.size() > 0) {
		att += " " + newAttr;
	} else {
		att += newAttr;
	}
    qDebug () << "XML attr add " << att << " (added " << _content << ")";
}

void XmlField::addAttribute(QString _att_name, int _content)
{
	QString newAttr = _att_name + "=\"" + QString::number(_content) + "\"";
	if (att.size() > 0) {
		att += " " + newAttr;
	} else {
		att += newAttr;
	}
    qDebug () << "XML attr add " << att << " (added " << _content << ")";
}

void XmlField::addAttribute(QString _att_name, QPoint _content)
{
	QString newAttr = _att_name + "=\"" + QString::number(_content.x()) + ":" + QString::number(_content.y()) + "\"";
	if (att.size() > 0) {
		att += " " + newAttr;
	} else {
		att += newAttr;
	}
    qDebug () << "XML attr add " << att << " (added " << _content << ")";
}

QString XmlField::getFieldStart()
{	QString ret;
	QString prefix;

	ret = level_prefix;

	ret += "<";
	ret += name;
	if (att.length() > 0) {
		ret += ' ';
		ret += att;
	}
	if (!hasMeta && content.isEmpty()) {
		no_content = true;
		ret += "/>\n";
	} else {
		no_content = false;
		ret += '>';
	}
	return ret;
}

QString XmlField::getFieldContent()
{	QString meta_content;
	QString _content;

	if (hasMeta) {
		XmlField field;
		if (version.length()) {
			field.set(XmlSymbolMap::m_oMap[XmlSymbol::PROGRAM_VERSION], version);
			meta_content = field.getField();
		}

		field.set(XmlSymbolMap::m_oMap[XmlSymbol::CREATION_DATE], creation);
		meta_content += field.getField();

		field.set(XmlSymbolMap::m_oMap[XmlSymbol::UPDATE_DATE], update);
		meta_content += field.getField();

		QString text = author;
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::AUTHOR], XmlField::quote(text));
		meta_content += field.getField();

		text = comment;
		field.set(XmlSymbolMap::m_oMap[XmlSymbol::COMMENT], XmlField::quote(text));
		meta_content += field.getField();

		_content = meta_content + content;
	} else {
		_content = content;
	}

	// content may contain CR
	if (level >= 0) {
		int find = _content.lastIndexOf("\n");
		if (find != -1) {
			QString ret(_content);
			ret.prepend(level_content_prefix);	// indent
			find -= 1;
			while (-1 != (find = ret.lastIndexOf("\n", find - 1)))
				ret.replace(find, 1, level_content_suffix);	// indent
			ret.prepend("\n");
			single_line = false;
			return ret;
		}
	}
	single_line = true;
	return _content;
}

QString XmlField::getFieldEnd()
{	QString ret;

	if (!single_line) ret = level_prefix;
	ret += "</";
	ret += name;
	ret += ">\n";

	return ret;
}
	
QString XmlField::getField()
{	QString ret;

	// name and properties
	ret = getFieldStart();

	if (!no_content) {
		// content
		ret += getFieldContent();

		// end tag
		ret += getFieldEnd();
	}

	return ret;
}

void XmlField::setVersion()
{
	hasMeta = true;
	version = KlogicGlobal::Klogic::VERSION;
}

void XmlField::setCreation()
{
	hasMeta = true;
	QDateTime curr_date_time = QDateTime::currentDateTime();
	creation = curr_date_time.toString();
}

void XmlField::setCreation(const QString& _creation)
{
	hasMeta = true;
	creation = _creation;
}

void XmlField::setUpdate()
{
	hasMeta = true;
	QDateTime curr_date_time = QDateTime::currentDateTime();
	update = curr_date_time.toString();
}

void XmlField::setUpdate(const QString& _update)
{
	hasMeta = true;
	update = _update;
}

void XmlField::setAuthor(const QString& _author)
{
	hasMeta = true;
	author = _author;
}

void XmlField::setComment(const QString& _comment)
{
	hasMeta = true;
	comment = _comment;
}

QString XmlField::getVersion()
{
	return version;
}

QString XmlField::getCreation()
{
	return creation;
}

QString XmlField::getUpdate()
{
	return update;
}

QString XmlField::getAuthor()
{
	return author;
}

QString XmlField::getComment()
{
	return comment;
}

