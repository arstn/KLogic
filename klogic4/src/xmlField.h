#ifndef __XMLFIELD__
#define __XMLFIELD__
 
#include <stdlib.h>
 
class QString;
class QPoint;
 
class XmlField
{
public:
	XmlField();
	XmlField(int);
	XmlField(QString _name, int);
	XmlField(QString _name, QString _content, int);

	int getLevel();

	// arguments: field-name[, content]
	void set(QString);
	void set(QString, QString);
	void set(QString, int);
	void set(QString, QPoint);
	void set(QString, int, char *);

	void setName(QString);

	// argument: content
	void setContent(QString);
	void addContent(QString);

	// argument: complete attribute list
	void setAttribute(QString);

	// arguments: name, value
	void addAttribute(QString, int);
	void addAttribute(QString, QString);
	void addAttribute(QString, QPoint);

	QString getField();
	QString getFieldContent();
	QString getFieldStart();
	QString getFieldEnd();

	void clear(); 
	
	static const QString XML_HEADER;


	void setVersion();
	void setCreation();
	void setCreation(const QString&);
	void setUpdate();
	void setUpdate(const QString &);
	void setAuthor(const QString&);
	void setComment(const QString&);

	QString getVersion();
	QString getCreation();
	QString getUpdate();
	QString getAuthor();
	QString getComment();

	static QString& quote(QString& content);
	static QString& unquote(QString& content);
private:
	void setLevelStrings(int);
	int level;
	bool single_line;
	bool no_content;
	QString level_prefix;
	QString level_suffix;
	QString level_content_prefix;
	QString level_content_suffix;
	QString name;
	QString content;
	QString att;

	bool hasMeta;
	QString version;
	QString creation;
	QString update;
	QString author;
	QString comment;
};
 
#endif

