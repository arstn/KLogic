/***************************************************/
/* double linked KlogicList template                     */
/*  - implementation contained in this file!       */
/*  - two additional string identifiers            */ 
/*                                                 */
/* Andreas Rostin                                  */
/* 22.01.98                                        */
/***************************************************/
#ifndef __LIST_CLASS_T__
#define __LIST_CLASS_T__

#include <stdlib.h>
#include <qdebug.h>
#include <qstring.h>

template <class T> class KlogicList
{public:
	KlogicList();
	~KlogicList();

	int counter();	// no of elements in the KlogicList
	int current();	// no of the current element
	int IsEmpty();	// returns 1 if empty

	// ***** retrieve objects
	inline T *Get(int);
	inline T *Get(T *);
	inline T *Get();
	inline T *Get(const QString &);

	// ***** replace object
	inline T *Put(T *);

	// ***** the access keys
	inline const QString & getText() const;
	inline void setText(const QString &);

	inline int getID1() const;
	inline void setID1(int);
	inline int getID2() const;
	inline void setID2(int);

	// ***** navigation
	inline KlogicList<T> * First();
	inline KlogicList<T> * Last();
	inline KlogicList<T> * Prev();
	inline KlogicList<T> * Next();

	// ***** search (the first if not unique) element with attribute..
	inline KlogicList<T> * With(T *);
	inline KlogicList<T> * With(int);	// having id1 == value
	inline KlogicList<T> * NextWith(int);	// next having id1 == value
	inline KlogicList<T> * With(const QString &);
	inline KlogicList<T> * With(T *, int);
	inline KlogicList<T> * With(T *, int, int);

	// ***** insert/append/replace/destroy
	inline KlogicList<T> * Append(T *new_content);
	inline KlogicList<T> * Append(T *new_content, int ida);
	inline KlogicList<T> * Append(T *new_content, int ida, int idb);
	inline KlogicList<T> * Insert(T *new_content, int id, T *next_content);
	inline KlogicList<T> * Destroy(T *content);
	inline KlogicList<T> * Destroy(T *content, int ida);
	inline KlogicList<T> * Destroy(T *content, int ida, int idb);
	inline KlogicList<T> * Destroy(int ida);
	inline KlogicList<T> * Destroy();

private:
	KlogicList<T> *next;
	KlogicList<T> *prev;
	T *tContent;
	QString text;
	int id1;
	int id2;
};

template <class T>
KlogicList<T>::KlogicList()
{	prev = (KlogicList<T> *)NULL;
	next = (KlogicList<T> *)NULL;
	tContent = (T *)NULL;
	text = "";
	id1 = 0;
	id2 = 0;
}

template <class T>
KlogicList<T>::~KlogicList()
{
}	

template <class T>
inline int KlogicList<T>::getID1() const
{
	return id1;
}

template <class T>
inline void KlogicList<T>::setID1(int id)
{
	id1 = id;
}

template <class T>
inline int KlogicList<T>::getID2() const
{
	return id2;
}

template <class T>
inline void KlogicList<T>::setID2(int id)
{
	id2 = id;
}

template <class T>
inline const QString & KlogicList<T>::getText() const
{
	return text;
}

template <class T>
inline void KlogicList<T>::setText(const QString &newText)
{	text = newText;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Append(T *new_content)
{
	return Append(new_content, 0, 0);
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Append(T *new_content, int ida)
{
	return Append(new_content, ida, 0);
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Append(T *new_content,  int ida, int idb)
{	KlogicList<T> *akt = this;
	KlogicList<T> *first = this;

	akt = this;
	while(akt->next) akt = akt->next;
	first = this;
	while(first->prev) first = first->prev;

	// das erste Listenelement existiert..
	if (akt == first) {
		if (akt->tContent == NULL)
		{	akt->tContent = new_content;
			akt->id1 = ida;
			akt->id2 = idb;
			text = "";
			return this;
		}
	}

	// ..alle weiteren werden hier erzeugt
	akt->next = new KlogicList<T>;
	akt->next->prev = akt;
	akt->next->next = (KlogicList<T> *)NULL;
	akt->next->tContent = new_content;
	akt->next->id1 = ida;
	akt->next->id2 = idb;
	return akt->next;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Insert(T *new_content, int id, T *next_content)
{	KlogicList<T> *akt = this->With(next_content);
	KlogicList<T> *ins;
	KlogicList<T> *first = this;

	// kein Nachfolger-Elememt gefunden..
	if (akt == NULL) return akt;

	while(first->prev) first = first->prev;

	// das erste Listenelement existiert..
	if ((akt == first) && (akt->tContent == NULL)) {
		akt->tContent = new_content;
		akt->id1 = id; 
		return akt;
	}

	// ..alle weiteren werden hier erzeugt
	ins = new KlogicList<T>;
	if (akt == first) {
		// erstes Element in das neue kopieren
		ins->tContent = akt->tContent;
		ins->prev = akt;
		ins->next = akt->next;
		ins->text = akt->text;
		if (ins->next) ins->next->prev = ins;
		ins->id1 = akt->id1;
		ins->id2 = akt->id2;
		// und das erste Element neu fuellen
		akt->tContent = new_content;
		akt->prev = (KlogicList<T> *)NULL;
		akt->next = ins;
		akt->text = "";
		akt->id1 = id;
		// return the new first element!
		ins = akt;
	} else {
		ins->tContent = new_content;
		ins->next = akt;
		ins->prev = akt->prev;
		ins->id1 = id;
		if (akt->prev) akt->prev->next = ins;
		akt->prev = ins;
	}
	return ins;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Destroy(T *content)
{
	return Destroy(content, 0, 0);
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Destroy(T *content, int ida)
{
	return Destroy(content, ida, 0);
}

// destroy by content, name1 and name2, returning next
template <class T>
inline KlogicList<T> * KlogicList<T>::Destroy(T *content, int ida, int idb)
{	KlogicList<T> *akt;
	KlogicList<T> *ret;
	KlogicList<T> *first = this;

	while(first->prev) first = first->prev;
	akt = first;
	ret = first;

	if (!content) return (KlogicList<T> *)NULL;
	while(akt) {
		if ((akt->tContent == content) && (ida == 0 || ida == akt->id1) && (idb == 0 || idb == akt->id2)) {
			if (akt == first) {
				// If possible, copy the next element to this first one
				akt->tContent = (T *)NULL;
				akt->id1 = 0;
				akt->id2 = 0;
				akt->text = "";
				if (akt->next) {
					ret = akt->next;
					akt->text = akt->next->text;
					akt->next->text = "";
					akt->tContent = akt->next->tContent;
					akt->id1 = akt->next->id1;
					akt->id2 = akt->next->id2;
					if (akt->next->next) akt->next->next->prev = akt;
					akt->next = akt->next->next;
					delete ret;
				}
				return akt;
			} else {
				if (akt->prev) {
					akt->prev->next = akt->next;
					ret = akt->prev;
				}
				if (akt->next) {
					akt->next->prev = akt->prev;
					ret = akt->next;
				}
				delete akt;
				return ret;
			}
		}
		akt = akt->next;
	}
	return (KlogicList<T> *)NULL;
}

// destroy by name, returning next
template <class T>
inline KlogicList<T> * KlogicList<T>::Destroy(int ida)
{	KlogicList<T> *akt;
	KlogicList<T> *ret;
	KlogicList<T> *first = this;

	while(first->prev) first = first->prev;
	akt = first;
	ret = first;

	while(akt)
	{	if (akt->id1 == ida) {
			// all KlogicList-entries but the first are allocated by the first
			if (akt == first) {
				akt->tContent = (T *)NULL;
				akt->id1 = 0;
				akt->id2 = 0;
				if (akt->next) {
					ret = akt->next;
					akt->tContent = akt->next->tContent;
					akt->id1 = akt->next->id1;
					akt->id2 = akt->next->id2;
					akt->text = akt->next->text;
					if (akt->next->next) akt->next->next->prev = akt;
					akt->next = akt->next->next;
					delete ret;
					return akt;
				}
				return akt;
			}
			else {
				if (akt->prev) {
					akt->prev->next = akt->next;
					ret = akt->prev;
				}
				if (akt->next) {
					akt->next->prev = akt->prev;
					ret = akt->next;
				}
				delete akt;
				return ret;
			}
		}
		akt = akt->next;
	}
	return ret;
}

// destroy all elements
template <class T>
inline KlogicList<T> * KlogicList<T>::Destroy()
{	KlogicList<T> *akt;
	KlogicList<T> *nextakt;
	KlogicList<T> *first = this;

	while(first->prev) first = first->prev;
	akt = first;

	while(akt)
	{	nextakt = akt->next;
		if (akt == first) {
			akt->tContent = (T *)NULL;
			akt->id1 = 0;
			akt->id2 = 0;
		}
		else {
			akt->prev->next = akt->next;
			if (akt->next) akt->next->prev = akt->prev;
			delete akt;
		}
		akt = nextakt;
	}
	return first;
}

template <class T>
inline T * KlogicList<T>::Get(int id)
{	KlogicList<T> *akt = this;

	while (akt) {
		if (akt->id1 == id) return akt->tContent;
		akt = akt->next;
	}
	akt = this;
	while (akt->prev) {
                if (akt->prev->id1 == id) return akt->prev->tContent;
                akt = akt->prev;                                                                                                      
	}
	return (T *)NULL;
}

template <class T>
inline T * KlogicList<T>::Get(T *searched)
{	KlogicList<T> *akt = this;

	while (akt) {
		if (akt->tContent == searched)
			return searched;
		akt = akt->next;
	}
	akt = this;
	while (akt->prev) {
		if (akt->prev->tContent == searched) return searched;
		akt = akt->prev;
	}
	return (T *)NULL;
}

template <class T>
inline T * KlogicList<T>::Put(T *new_object)
{
	T *ret = tContent;
	tContent = new_object;
	return ret;
}

template <class T>
inline T * KlogicList<T>::Get(const QString &searched)
{	KlogicList<T> *akt = this;

	while (akt) {
		if (akt->text == searched) 
			return akt->tContent;
		akt = akt->next;
	}
	akt = this;
	while (akt->prev) {
		if (akt->prev->text == searched)
			return akt->prev->tContent;
		akt = akt->prev;
	}
	return (T *)NULL;
}

template <class T>
inline T * KlogicList<T>::Get()
{	return this->tContent;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::With(T *searched)
{	KlogicList<T> *akt = this;

	while (akt) {
		if (akt->tContent == searched) return akt;
		akt = akt->next;
	}
	akt = this;
	while(akt->prev) {
		if (akt->prev->tContent == searched) return akt;
		akt = akt->prev;
	}
	return (KlogicList<T> *)NULL;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::With(const QString &searched)
{	KlogicList<T> *akt = this;

	if (searched == "") return (KlogicList<T> *)NULL;

	while(akt) {
		if (akt->text == searched)
			return akt;
		akt = akt->next;
	}
	akt = this;
	while (akt->prev) {
		if (akt->prev->text == searched)
			return akt->prev;
		akt = akt->prev;
	}
	return (KlogicList<T> *)NULL;
}

template <class T>
KlogicList<T> * KlogicList<T>::With(int id)
{	KlogicList<T> *akt = this;

	while (akt) {
		if (akt->id1 == id) return akt;
		akt = akt->next;
	}
	akt = this;
	while(akt->prev) {
		if (akt->prev->id1 == id) return akt->prev;
		akt = akt->prev;
	}
	return (KlogicList<T> *)NULL;
}

template <class T>
KlogicList<T> * KlogicList<T>::NextWith(int id)
{	KlogicList<T> *akt = this->next;

	while (akt) {
		if (akt->id1 == id) return akt;
		akt = akt->next;
	}
	return (KlogicList<T> *)NULL;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::With(T *searched, int id)
{	KlogicList<T> *akt = this;

	while (akt) {
		if (akt->tContent == searched && akt->id1 == id) return akt;
		akt = akt->next;
	}
	akt = this;
	while(akt->prev) {
		if (akt->prev->tContent == searched && akt->prev->id1 == id) return akt->prev;
		akt = akt->prev;
	}
	return (KlogicList<T> *)NULL;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::With(T *searched, int ida, int idb)
{	KlogicList<T> *akt = this;

	while (akt) {
		if (akt->tContent == searched && akt->id1 == ida && akt->id2 == idb) return akt;
		akt = akt->next;
	}
	akt = this;
	while(akt->prev) {
		if (akt->prev->tContent == searched && akt->prev->id1 == ida && akt->prev->id2 == idb)
			return akt->prev;
		akt = akt->prev;
	}
	return (KlogicList<T> *)NULL;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::First()
{	KlogicList<T> *akt = this;

	while(akt->prev) akt = akt->prev;

	if (tContent) return akt;
	return (KlogicList<T> *)NULL;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Last()
{	KlogicList<T> *akt = this;

	while (akt->next) akt = akt->next;

	if (tContent) return akt;
	return (KlogicList<T> *)NULL;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Prev()
{	if (prev && prev->tContent) return prev;
	return (KlogicList<T> *)NULL;
}

template <class T>
inline KlogicList<T> * KlogicList<T>::Next()
{	if (next && next->tContent) return next;
	return 0;
}

template <class T>
inline int KlogicList<T>::counter()
{	KlogicList<T> *akt = this;
	int ret = 0;

	while (akt) {
		if (akt->tContent) ret++;
		akt = akt->next;
	}
	akt = this;
	while(akt->prev) {
		if (akt->prev->tContent) ret++;
		akt = akt->prev;
	}
	return ret;
}

template <class T>
inline int KlogicList<T>::IsEmpty()
{
	KlogicList<T> *akt = this;
	while (akt) {
		if (akt->tContent) return 0;
		akt = akt->next;
	}
	akt = this;
	while(akt->prev) {
		if (akt->prev->tContent) return 0;
		akt = akt->prev;
	}
	return 1;
}

template <class T>
inline int KlogicList<T>::current()
{	KlogicList<T> *akt = this;
	int ret = 0;

	while (akt->prev) {
		if (akt->tContent) ret++;
		akt = akt->prev;
	}
	return ret;
}

#endif

