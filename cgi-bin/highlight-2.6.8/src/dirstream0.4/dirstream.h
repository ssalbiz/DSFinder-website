/////////////////////////////////////////////////////////////////////////////
//	file		:	dirstream.h
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
//  Definition der Klassen dirstream und dirstream_iterator sowie
//  einiger Filter und Hilfsfunktionen.
//
//  Hinweis:
//	Auf Win32-Platformen verwendet dirstream Wrapper-Funktionen
//	von Kevlin Henney. Dieser Wrapper basieren auf den Funktionen _findfirst,
//	_findnext und _findclose.
//	http://www.two-sdg.demon.co.uk/curbralan/code/dirent/dirent.html
//
//	Die ursprüngliche Idee für diesen dirstream stammt aus dem Artikel
// 	"Promoting Polymorphism" von Kevlin Henney.
// 	http://www.appdevadvisor.co.uk/Downloads/ada5_8/Henney5_8.pdf
//
/////////////////////////////////////////////////////////////////////////////
//
/****************************************************************************
 *                                                                         	*
 *	This program is free software; you can redistribute it and/or modify  	*
 *	it under the terms of the GNU General Public License as published by  	*
 *	the Free Software Foundation; either version 2 of the License, or     	*
 *	(at your option) any later version.                                   	*
 *																			*
 * 	This program is distributed in the hope that it will be useful,			*
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of			*
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			*
 *	GNU General Public License for more details.                           	*
 *																			*
 ****************************************************************************/
#ifndef DIR_STREAM_H_INCLUDED
#define DIR_STREAM_H_INCLUDED
#include <cctype>
#include <string>
#if defined _MSC_VER && _MSC_VER <= 1300
#	define for if(0);else for
#	pragma warning(disable:4786)
# 	define HAS_STD_ITERATOR
namespace std {
	using ::tolower;
	using ::size_t;
}
#endif

#include <stack>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <memory>
#include <utility>

#include "filter_base.h"
#include "detail/filter_asserter.h"
///////////////////////////////////////////////////////////////////////////////
// Platform-spezifischer Code 
///////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include "dirstreamw32.h"
#else
// Wenn vorhanden, POSIX Funktionen verwenden.
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
namespace dirstr
{
	const char PATH_SEPERATOR = '/';
}
#endif
///////////////////////////////////////////////////////////////////////////////
namespace dirstr
{

/**
 * \ingroup Main
 * \brief Aufzählung der verschiedenen Typen von Verzeichniseinträgen
 **/
enum entry_type{
	dirs = 1,						/*!< Verzeichnisse */
	dirs_not_dot_or_dot_dot = 2,	/*!< Verzeichnisse außer '.' und '..' */
	files = 4,						/*!< Dateien */
	files_and_dirs = dirs | files,  /*!< Dateien und Verzeichnisse */
	files_and_dirs_not_dot_or_dot_dot = dirs_not_dot_or_dot_dot | files /*!< Dateien und Verzeichnisse außer '.' und '..' */
};

/**
 * \ingroup Main
 * \brief Aufzählung der Suchmodi die der dirstream unterstützt.
 **/
enum recurse_mode {
	recursive_yes,	/*!< Rekursive Suche in Unterverzeichnissen */
	recursive_no	/*!< Nicht-rekursive Suche. Unterverzeichnisse werden nicht berücksichtigt */
};

/**
 * \ingroup Main
 * \brief Exception die von der Klasse dirstream geworfen wird, falls sich ein angegebenes 
 * Verzeichnis nicht öffnen lässt.
 **/
class open_error : public std::runtime_error
{
public:
	open_error(const char* dir)
		: std::runtime_error(dir)
	{}
};

/**
 * \ingroup Filter
 * \brief no_f steht für "kein Filter". Es werden alle Einträge ausgewählt.
 **/
struct no_f : public filter_base
{
public:
	bool operator()(const std::string&) const {return true;}
	filter_base* clone() const {return new no_f();}
};

template <class F> struct expr;
///////////////////////////////////////////////////////////////////////////////
// class dirstream
///////////////////////////////////////////////////////////////////////////////
/**
 * \ingroup Main
 * \author Benjamin Kaufmann
 * \date 2002-2004
 * \version 0.4
 * \brief dirstream ermöglicht die Traversierung eines Verzeichnisses über ein
 * istream-ähnliches Interface.
 *
 * Die Traversierung eines Verzeichnisses kann über zwei \ref Filter "Filter-Objekte"
 * beeinflusst werden. 
 * Der Entry-Filter wird für jeden Verzeichniseintrag aufgerufen.
 * Konkrete Entry-Filter müssen für jeden Eintrag entscheiden, ob dieser
 * ausgewählt (Filter liefert true) oder ignoriert (Filter liefert false)
 * werden soll.
 * Der SubDir-Filter wird nur bei der rekursiven Verzeichnissuche verwendet.
 * Er wird für jedes Unterverzeichnis aufgerufen. Liefert der Filter true,
 * wird das Unterverzeichnis berücksichtigt andernfalls wird es ignoriert.
 * Beide Filter-Typen müssen das Interface filter_base implementieren.
 * \note 
 * An den dirstream übergebene Filter werden niemals direkt verwendet. Stattdessen
 * erzeugt und verwendet ein dirstream-Objekt immer ein \ref dirstr::filter_base::clone "Klon"
 * 
 * \see filter_base
 * \see pred_f
 * \see mem_fun_f
 * \see ptr_fun_f
 */
class dirstream
{
typedef filter_base* (std::auto_ptr<filter_base>::*unspecified_bool_type)() const;
public:
	/**
	 * Erstellt ein neues dirstream-Objekt für die Durchwanderung des Verzeichnisses dirName.
	 * @pre dirName != 0
	 * @param dirName		Name des zu durchwandernden Verzeichnisses
	 * @param entryFilter   Filter der für die Auswahl von Verzeichniseinträgen verwendet
	 *						werden soll.
	 *						Der Default-Wert \ref no_f steht für "kein Filter", es werden
	 *						alle Verzeichniseinträge ausgewählt.
	 * @param recMode		Zu verwendender Suchmodus.
	 * @param subDirFilter	Filter der bei der Auswahl von Unterverzeichnissen 
	 *						angewendet werden soll. Wird nur berücksichtigt falls
	 *						recurse_mode::recursive_yes angegeben wurde.
	 *						Der Default-Wert \ref no_f steht für "kein Filter", es werden
	 *						alle Unterverzeichnisse ausgewählt.
	 * @exception			open_error Eine \ref open_error "open_error-Exception" wird geworfen, 
	 *						falls dirName kein gültiges Verzeichnis ist bzw.
	 *						dieses Verzeichnis nicht geöffnet werden kann.
	 * @post is_open() liefert true
	 * 
	 **/
	dirstream		(const char* dirName, const filter_base& entryFilter = no_f(),
					 recurse_mode recMode = recursive_no, const filter_base& subDirFilter = no_f());
		
	/**
	 * \brief Schließt den Strom und gibt alle Ressourcen frei.
	 * 
	 * Der Destruktor ruft implizit die close-Funktion auf.
	 * close muss also in der Regel nicht explizit aufgerufen werden.
	 **/
	~dirstream		();

	/**
	 * Ermöglicht die Verwendung eines Stream-Objekts innerhalb eines bool'schen Ausdrucks.
	 * Erlaubt also Ausdrücke wie if (stream)
	 * @see dirstream::is_open() const
	 **/
	operator unspecified_bool_type() const
	{
		return m_DirHandle ? &std::auto_ptr<filter_base>::get : 0;
	}
    
	/**
	 * Über diesen Operator wird der nächste Eintrag des Verzeichnisses
	 * eingelesen.
	 * @pre		is_open() liefert true
	 * @param	name String in dem der Name des aktuellen Verzeichniseintrags
	 *			gespeichert werden soll.
	 * @post	name enthält den aktuellen Verzeichniseintrag relativ zum
	 *			Startverzeichnis.
	 *			Existiert kein weiterer Eintrag, liefert der Vergleich *this == 0
	 *			true und der Wert von name ist undefiniert.
	 * @exception open_error Eine \ref open_error "open_error-Exception" wird
	 *	geworfen, falls bei der rekursiven Traversierung eines Verzeichnisses
	 *	ein Unterverzeichnis nicht geöffnet werden kann.
	 *						
	 **/
	dirstream& operator>>(std::string& name);

	/**
	 * Liest maximal n-Einträge aus dem Verzeichnis-Strom und speichert diese
	 * im Array r.
	 * \pre is_open() liefert true
	 * \pre n >= 0
	 * \param r String-Array der Größe n oder größer.
	 * \param n Anzahl der zu lesenden Einträge
	 * \return Anzahl der tatsächlich gelesenen Einträge.
	 * \exception open_error Eine \ref open_error "open_error-Exception" wird
	 *	geworfen, falls bei der rekursiven Traversierung eines Verzeichnisses
	 *	ein Unterverzeichnis nicht geöffnet werden kann.
	 **/
	std::streamsize readsome(std::string* r, std::streamsize n);

	/**
	 * @see dirstream(const char*, const filter_base&, recurse_mode, const filter_base&)
	 * Sollte dieses Objekt bereits mit einem Verzeichnis verbunden sein, ruft
	 * open zuerst close auf bevor es das neue Verzeichnis öffnet.
	 **/
	bool open(const char* dirName, const filter_base& entryFilter,
			  recurse_mode recMode = recursive_no, const filter_base& subDirFilter = no_f());
	
	/** 
	 * \brief Öffnet das Verzeichnis dirName unter Verwendung der im Konstruktor
	 * angegebenen Filter.
	 * 
	 * Sollte dieses Objekt bereits mit einem Verzeichnis verbunden sein, ruft
	 * open zuerst close auf bevor es das neue Verzeichnis öffnet.
	 **/
	bool open(const char* dirName, recurse_mode recMode = recursive_no);
	
	/**
	 * \brief Liefert true, falls ein Verzeichnis erfolgreich geöffnet wurde.
	 */
	bool is_open		()	const;

	/**
	 * \brief "Spult" ein Verzeichnisstrom zurück.
	 * 
	 * Ein solcher Strom kann also wieder von vorne durchlaufen werden.
	 * Der Aufruf von rewind invalidiert alle Dirstream-Iteratoren
	 * die mit diesem Strom verbunden sind.
	 * \pre is_open() liefert true
	 **/
	void rewind			();
	
	/**
	 * \brief Schließt den Verzeichnisstrom und gibt das intern verwaltet
	 * Verzeichnis-Handle frei.
	 *
	 * Der Aufruf von close macht alle dirstream_iteratoren die mit diesem
	 * Objekt verbunden sind ungültig.
	 * 
	 * \post is_open() liefert false
	 **/
	void close();
private:
	// Streamobjekte können nicht kopiert werden, da diese
	// Operation keinen Sinn macht.
	dirstream(const dirstream&);
	dirstream& operator=(const dirstream&);
	
	bool isDirectory(const char*) const;
	bool selectSubDir(const std::string& name) const;
	bool selectEntry(const std::string& name) const;
	void closeDir();
    void commonInit();
    bool dotOrDotDot(const std::string& name) const;
    
	typedef std::auto_ptr<filter_base> Filter;
	Filter					m_EntryFilter;
	Filter					m_SubDirFilter;
	recurse_mode			m_RecurseMode;
	std::stack<std::string>	m_Dirs;
	std::string				m_DirName;
    std::string             m_CurrentPath;
	DIR*					m_DirHandle;
	dirent*					m_CurrentEntry;
};

/**
 * \ingroup Main
 * @brief	Iterator-Klasse für dirstream-Objekte.
 *
 * Dieser Iterator der Kategorie Input-Iterator ermöglicht z.B. die
 * Verbindung von dirstream-Objekten mit den Algorithmen der
 * Standardbibliothek.
 * 
 * \code
 * dirstream str(".");
 * std::copy(dirstream_iterator(str), dirstream_iterator(), 
 *     ostream_iterator<string>(cout, "\n"));
 * \endcode
 **/
class dirstream_iterator
#ifdef HAS_STD_ITERATOR
	: public std::iterator<std::input_iterator_tag, std::string>
#endif
{
public:
// Wer std::iterator nicht hat, muss selbst für die benötigten
// typedefs sorgen.
#ifndef HAS_STD_ITERATOR
typedef std::input_iterator_tag 	iterator_category;
typedef std::string					value_type;
typedef ptrdiff_t 					difference_type;
typedef std::string*   				pointer;
typedef std::string& 				reference;
#endif
public:
	typedef dirstream stream_type;
	/**
	 * Der Standard-Ctor erstellt einen Ende-Iterator.
	 */
	dirstream_iterator() 
		: dstr_(0)
	{}
	
	/**
	 * Dieser Ctor verbindet den neuen Iterator mit dem Stream dstr und
	 * versucht dann einen ersten Eintrag zu lesen.
	 * Gelingt dies, kann der Iterator dereferenziert und damit der Wert
	 * extrahiert werden. Andernfalls wird der Iterator zu einem Ende-Iterator,
	 * der nicht dereferenziert werden darf.
	 **/
	explicit dirstream_iterator(dirstream& dstr)
		: dstr_(&dstr)
	{
		readOne();
	}
	
	/**
	 * Liefert den aktuellen Verzeichniseintrag.
	 * \pre *this != dirstream_iterator()
	 */
	const std::string& operator*() const
	{
		return currVal_;
	}

	/**
	 * Liefert &**this;
	 * \see operator*()
	 **/
	const std::string* operator->() const
	{
		return &**this;	
	}

	/**
	 * Liest und speichert den nächsten Verzeichniseintrag, falls dies
	 * möglich ist.
	 * 
     * \see dirstream::operator>>
     */
	dirstream_iterator& operator++()
	{
		readOne();
		return *this;	
	}

	/**
	 * Erstellt eine Kopie des Iterator-Objekts, liest den nächsten Eintrag
	 * und liefert die Kopie zurück.
	 **/
	dirstream_iterator operator++(int)
	{
		dirstream_iterator t(*this);
		++*this;
		return t;	
	}
	friend bool operator==(const dirstream_iterator& lhs, const dirstream_iterator& rhs);
	friend bool operator!=(const dirstream_iterator& lhs, const dirstream_iterator& rhs);
private:
	void readOne()
	{
		assert(dstr_);
		if ( !(*dstr_ >> currVal_) )
			dstr_ = 0;
	}
	dirstream* dstr_;
	std::string currVal_;
};

/**
 * Liefert true, falls lhs und rhs mit dem selben Stream verbunden sind oder
 * aber beide Iteratoren Ende-Iteratoren sind.
 **/
inline bool operator==(const dirstream_iterator& lhs, const dirstream_iterator& rhs)
{
	return lhs.dstr_ == rhs.dstr_;
}

/**
 * Liefert !(lhs == rhs)
 * \see operator==(const dirstream_iterator&, const dirstream_iterator&)
 * \par Beispielhafte Benutzung:
 * \code
 * dirstream str(".");
 * for(dirstream_iterator it(str), end; it != end; ++it)
 * {...}
 * \endcode
 **/
inline bool operator!=(const dirstream_iterator& lhs, const dirstream_iterator& rhs)
{
	return !(lhs == rhs);
}


///////////////////////////////////////////////////////////////////////////////
// Ein paar nützliche Funktionen
///////////////////////////////////////////////////////////////////////////////
/*! \defgroup Utilities Zusätzliche Funktionen
 */

/** 
 * \ingroup Utilities
 * \brief Prüft ob es sich bei entry um ein Verzeichnis handelt.
 * 
 * @pre path.empty() == false
 * @return liefert true, falls entry ein Verzeichnis ist. false sonst.
 **/
bool is_directory(const std::string& entry);

/**
 * \ingroup Utilities
 * \brief Liefert den absoluten Pfad des Verzeichniseintrags entry.
 * 
 * @pre path.empty() == false
 * @return liefert den absoluten Pfad für entry. Liefert einen leeren String
 * für den Fall, dass die Auflösung fehl schlägt.
 **/
std::string full_path(const std::string& entry);

/**
 * \ingroup Utilities
 * \brief Erzeugt das Verzeichnis path
 * 
 * @pre path.empty() == false
 * @return liefert true, falls das Verzeichnis path erzeugt werden konnte. false sonst
 **/
bool create_directory(const std::string& path);

/**
 * \ingroup Utilities
 * \brief Prüft ob das Verzeichnis path existiert.
 * 
 * @pre path.empty() == false
 * @return liefert true, falls das Verzeichnis path existiert.
 **/
bool directory_exists(const std::string& path);

///////////////////////////////////////////////////////////////////////////////
// Standard-Filter
///////////////////////////////////////////////////////////////////////////////
/*! \defgroup Filter Vordefinierte Filter- und Filter-Adapter-Klassen
\par Einführung
Bei der Auflisting von Verzeichnissen will man häufig nur solche Einträge
berücksichtigen, die einem bestimmten Kriterium genügen. 
Da Filter-Kriterien in der Regel aber anwendungsspezifisch sind, 
ist eine Auswahl einer Menge fest vorgegebener Filter sehr schwierig.
Aus diesem Grund bietet dirstream neben einer Menge von Standardfiltern
dem Benutzer auch die Möglichkeit, beliebige benutzer-definierte Filter 
zu verwenden.
\par dirstream Filter
Aus Sicht des dirstreams ist ein Filter ein Objekt einer von der \ref dirstr::filter_base "Filter-Basisklasse filter_base"
abgeleiteten Klasse. Jedes <b>Filter-Objekt f</b> muss für einen gegebenen <b>Verzeichniseintrag e</b>
entscheiden, ob dieser Eintrag \b ausgewählt ('f(e)' liefert true) oder \b ignoriert ('f(e)' liefert false) 
werden soll.
Die Traversierung eines Verzeichnisses mittels eines \ref dirstr::dirstream "dirstream-Objekts"
wird durch zwei Filer-Objekte beeinflusst:
- Ein Entry-Filter der für jeden Verzeichniseintrag aufgerufen wird, sowie
- Ein SubDir-Filter der, <b>im Fall der rekursiven Suche</b>, für jedes Unterverzeichnis 
aufgerufen wird.

\par Standardfilter
dirstream bietet zur Zeit drei mehr oder weniger nützliche Standardfilter:
- \ref dirstr::no_f "no_f": Ein Filter der immer true lierfert, also alle Einträge auswählt.
- \ref dirstr::type_f "type_f": Ein Filter der nur solche Einträge auswählt, die einen bestimmten Typ haben.
- \ref dirstr::pattern_f "pattern_f": Ein Filter der Einträge auswählt, die einem bestimmten Muster genügen.

\par Eigene Filter schreiben
Das Verhalten eines dirstreams kann zusätzlich durch benutzer-definierte Filter beliebig
beeinflusst werden. 
Benutzer-definierte Filter können auf vier verschiedene Arten erzeugt werden.
- Durch Ableitung einer Klasse von \ref dirstr::filter_base "filter_base"
- Durch \ref dirstr::ptr_fun_f "Adaption einer benutzer-definierten Funktion"
- Durch \ref dirstr::pred_f "Adaption eines benutzer-definierten Funktionsobjekts"
- Durch \ref dirstr::mem_fun_f "Adaption einer Memberfunktion einer benutzer-definierten Klasse"

Die genauen Bedingungen für die Adaption von Funktionen, Funktionsobjekten und
Memberfunktionen sowie Beispiele werden in der Dokumentation der entsprechenden Adapter
beschrieben.
\see dirstr::adapter_f_t
\see dirstr::mem_fun_f_t

\par Filter kombinieren
 Filter können über boole'sche Operationen zu neuen Filtern verknüpft werden.
 Wie das genau geht und was es dabei zu beachten gilt, wird im Abschnitt
 \ref BoolFilter beschrieben.
 */


#define ADD_CLONE_IMPL(Name) \
filter_base* clone() const {return new Name(*this);}


/**
 * \ingroup Filter
 * \brief type_f wählt Einträge eines bestimmten Typs.
 * \see entry_type
 */
struct type_f : public filter_base {
	/**
	 * @param t	Typ der Verzeichniseinträge die ausgewählt werden sollen.
	 * @see entry_type
	 **/
	type_f(entry_type t)
		: t_(t)
	{}
	
	/**
	 * \brief Liefert true, falls entryName vom passenden Typ ist.
	 **/
	bool operator()(const std::string& entryName) const
	{
		bool isDir = is_directory(entryName.c_str());
		if ((t_ & dirs) && isDir)
			return true;
		if (!isDir && (t_ & files))
			return true;
		if (isDir && (t_ & dirs_not_dot_or_dot_dot))
		{
			std::string::size_type pathEnd = entryName.rfind(PATH_SEPERATOR);
			if (pathEnd == std::string::npos)
				pathEnd = 0;
			else 
				++pathEnd;
			return	entryName.compare(pathEnd, std::string::npos, ".") != 0 &&
					entryName.compare(pathEnd, std::string::npos, "..") != 0;
		}
		return false;
	}		
	ADD_CLONE_IMPL(type_f);
private:
	entry_type t_;
};


/**
 * \ingroup Filter
 * \author Benjamin Kaufmann
 * \date 2002-2004
 * \version 0.4
 * \brief pattern_f wählt Einträge die einem bestimmten Muster genügen. 
 * 
 * Ein Muster kann die bekannten Wildcards '*' und '?' enthalten wobei 
 * - '*' für 0 oder beliebig viele Zeichen und
 * - '?' für genau ein Zeichen steht.
 * .
 * Außerdem unterstützt dieser Filter einfache Zeichenmengen eingeschlossen in [].
 * - [ab] matched genau ein Zeichen, entweder 'a' oder 'b'.
 *
 * \note
 * Soll eine Zeichenmenge das Zeichen ']' enthalten, so muss dieses Zeichen
 * das erste Zeichen der Menge sein (z.B. []ab] -> matched ']' oder 'a' oder 'b')
 *
 * \note
 * pattern_f unterstützt derzeit weder Ranges noch Negation, d.h.
 * [a-z] matched entweder 'a', '-' oder 'z' nicht ein beliebiges Zeichen zwischen
 * a und z. 
 * 
 * \note
 * Auf POSIX.2 konformen Systemen sollte auf die weitaus leistungsfähigeren
 * Funktionen fnmatch bzw. glob zurückgegriffen werden.
 * fnmatch kann man mit dem dirstream z.B. wie folgt nutzen:
 * \code
 * #include <fnmatch.h>
 * struct FnMatcher
 * {
 * 	FnMatcher(const char* pattern, int flags)
 * 		: pattern_(pattern)
 * 		, flags_(flags)
 * 	{}
 * 	bool operator()(const std::string& e) const {
 * 		return ::fnmatch(pattern_, e.c_str(), flags_);
 * 	}
 * private:
 * 	const char* pattern_;
 * 	int flags_;
 * };
 * ...
 * dirstream s(".", pred_f(FnMatcher("*.cpp", 0));
 * \endcode
 **/
struct pattern_f : public filter_base
{
	/**
	 * \brief Exception die geworfen wird, falls ein ungültiges Pattern
	 * an pattern_f übergeben wurde.
	 **/
	class invalid_pattern : public std::logic_error
	{
	public:
		invalid_pattern(const char* msg) : std::logic_error(msg)
		{}
	};
	
	/**
	 * @pre pattern != 0
	 * @param pattern Muster dem auszuwählende Einträge genügen müssen.
	 * @param types Eintragstypen die untersucht werden sollen.
	 * @param checkPath false, falls nur der Name des Eintrags, nicht aber
	 * sein Pfad beim pattern matching berücksichtigt werden soll.
	 * false ist der Default-Wert. 
	 * @note
	 * Die Berücksichtigung des Pfades beim pattern matching kann zu unerwarteten 
	 * Ergebnissen führen, da der Path-Seperator nicht besonders behandelt
	 * wird. D.h. insbesondere, dass er auch durch * oder ? gematched werden
	 * kann. Das Muster "Foo*" matched in diesem Fall also auch einen Eintrag
	 * "Foo/bar.cpp"
	 * @param ignoreCase true, falls case-insensitiv verglichen werden soll.
	 * @exception pattern_f::invalid_pattern \ref pattern_f::invalid_pattern
	 */
	explicit pattern_f(const char* pattern, 
						   entry_type types = files_and_dirs_not_dot_or_dot_dot, 
						   bool checkPath = false, 
						   bool ignoreCase = false
						   )
		: pattern_(simplify(pattern))
		, filter_(types)
		, ignoreCase_(ignoreCase)
		, checkPath_(checkPath)
	{}
	
	/**
	 * \brief Liefert true, falls entryName dem im Ctor übergebenen Pattern genügt und
	 * vom richtigen Typ ist.
	 **/
	bool operator()(const std::string& entryName) const
	{
		const char* fname = getName(entryName);
		if (!filter_(entryName)) return false;
		if (entryName.empty()) return pattern_ == "*" || pattern_.empty();
		return match(fname, pattern_.c_str());
	}
	ADD_CLONE_IMPL(pattern_f);
private:
	// @pre pattern does not contain consecutive '*'s.
	bool match(const char* str, const char* pattern) const
	{
		const char* strRun = 0;
		const char* patternRun = 0;
		bool starSeen = false;
		bool end = false;
		do {
			for (strRun = str, patternRun = pattern; !(end = *strRun == 0); ++strRun, ++patternRun) 
			{
				if (*patternRun == '*')
				{	// conceptually a star starts a new segment
					// we now have to match the new segment against the
					// rest of the string...
					starSeen = true;
					str = strRun;
					pattern = patternRun;
					// ...unless the '*' is the last char of pattern because
					// this means "match everything"
					if (!*++pattern) return true;
					break;
				}
				else if (*patternRun == '[')
				{	// match set
					++patternRun;
					bool first = true;		// allow ] as first char of set - e.g []abc]
					bool found = false;
					while (*patternRun != ']' || first)
					{
						if (!found && *patternRun == *strRun)
							found = true;
						first = false;
						++patternRun;
					}
					if (!found)
					{
						if (!starSeen) return false;
						++str;
						break;
					}
				}
				else if (!equal(*patternRun, *strRun) && *patternRun != '?')
				{	// mismatch...
					if (!starSeen) return false;
					// ...but since the current segment started with a star
					// we match this char against the star and then continue our search 
					// 
					++str;	
					break;
				}
			}
			if (end)
			{
				if (*patternRun == '*') ++patternRun;
				return (!*patternRun);
			}
		} while(!end);
		return false;
	}
	bool equal(char c1, char c2) const 
	{
		return ignoreCase_ ? 
			std::tolower(static_cast<unsigned char>(c1)) == std::tolower(static_cast<unsigned char>(c2))
			: c1 == c2;
	}
	
	// simplifies a pattern containing the wild cards * and ?
	// by replacing '**' with '*' and '*?' with '?*'.
	// Example: "a****?b*?*?c*?***" -> "a?*b??*c?*"
	std::string simplify(const char* pattern)
	{
		if (!pattern || !*pattern) return "";
		checkPattern(pattern);
		const char* cur = pattern;
		const char* next = cur + 1;
		bool starPending = false;
		std::string ret;
		ret.reserve(16);
		while (*cur)
		{
			if (*cur == '*' || starPending)
			{
				while (*next == '*') ++next;	// treat consecutive '*' as one
				if (*next == '?')				// handle '*?'
				{
					++cur;
					++next;
					ret += '?';					// we don't write the * here to
					starPending = true;			// simplify handling of consecutive '*?'
				}
				else
				{
					ret += starPending ? '*' : *cur;
					starPending = false;
					cur = next++;
				}
			}
			else
			{
				ret += *cur;
				cur = next++;
			}
		}
		return ret;
	}
	
	void checkPattern(const char* pattern) const
	{
		for(;*pattern;++pattern)
		{
			if (*pattern == '[')
			{
				pattern += 2;
				while (*pattern && *pattern != ']')
					++pattern;
				if (!*pattern)
					throw invalid_pattern("missing ']'");
			}
		}
	}
	const char* getName(const std::string& entryName) const
	{
		if (checkPath_)
			return entryName.c_str();
		std::string::size_type p = entryName.rfind(PATH_SEPERATOR);
		if (p == std::string::npos)
			return entryName.c_str();
		else
			return entryName.c_str() + p + 1;
	}	
	std::string pattern_;
	type_f filter_;
	bool ignoreCase_;
	bool checkPath_;
};

///////////////////////////////////////////////////////////////////////////////
// Filter-Adapter
///////////////////////////////////////////////////////////////////////////////
/**
 * \ingroup Filter
 * \brief Diese Klasse macht aus Funktionen bzw. Prädikaten \ref dirstr::filter_base "filter_base-kompatiblen" Filter.
 * \par Anforderungen:
 * - F muss kopierbar sein
 * - der Ausruck f(e) muss gültig sein, wobei f ein Objekt vom Typ F und
 * e ein Objekt vom Typ std::string ist.
 * - Der Typ von f(e) muss nach bool konvertierbar sein.
 * 
 * \see filter_base
 * \see ptr_fun_f
 * \see pred_f
 **/
template <class F>
struct adapter_f_t : public filter_base, private detail::assert_is_filter<F>
{
public:
	/**
	 * \brief Erzeugt einen Adapter für die Funktion bzw. das Prädikat f.
	 **/
	adapter_f_t(const F& t)
		: f_(t)
	{}
	
	/**
	 * \brief Ruft die adaptierte Funktion mit e als Argument auf und liefert
	 * true, fallse f(e) != 0
	 **/
	bool operator()(const std::string& e) const
	{
		return f_(e) != 0;
	}
	filter_base* clone() const
	{
		return new adapter_f_t<F>(f_);
	}
private:
	F f_;
};

/**
 * \ingroup Filter
 * \brief Macht aus einem Zeiger auf eine Funktion ein filter_base-Objekt.
 *
 * \see adapter_f_t
 *
 * \code
 * bool filter(const std::string&); // benutzer-definierte Filter-Funktion
 * ...
 * dirstream s(".", ptr_fun_f(&filter));
 * \endcode
 */
template <class R>
adapter_f_t<R (*)(const std::string&)> ptr_fun_f(R (*ptr_fun)(const std::string&))
{
	return adapter_f_t< R (*)(const std::string&) >(ptr_fun);	
}

/**
 * \ingroup Filter
 * \brief Erstellt aus einem Prädikat ein filter_base-Objekt.
 *
 * \see adapter_f_t
 *
 * \code
 * struct Foo { bool operator()(const std::string& e) const; };	// benutzer-defniert
 * ...
 * dirstream s(".", pred_f(&filter));
 * \endcode
 */
template <class T>
adapter_f_t<T> pred_f(const T& f)
{
	return adapter_f_t<T>(f);
}

/**
 * \ingroup Filter
 * \brief Diese Klasse macht aus einer Memberfunktion einen \ref dirstr::filter_base "filter_base-kompatiblen" Filter.
 *
 * \par Anforderungen:
 *  - T ist ein Pointer-kompatibler Typ der einen aufrufbaren operator-> besitzt
 *  - T is kopierbar
 *  - MemFun ist vom Type "Zeiger auf konstante-Memberfunktion"
 *  - der Ausdruck (ptr->*memFun)(e) ist gültig, wobei ptr ein Objekt vom Typ T, memFun
 * ein Objekt vom Typ MemFun und e ein Objekt vom Typ std::string ist.
 *  - Der Typ von (ptr->*memFun)(e) muss nach bool konvertierbar sein.
 *
 * \attention
 * mem_fun_f_t-Objekte arbeiten mit Zeigern, d.h. insbesondere, dass
 * beim Klonen lediglich Zeiger kopiert werden.
 * Es ist Aufgabe des Nutzers dafür zu sorgen, dass das referenzierte Objekt
 * mindestens so lange lebt wie das mem_fun_t-Objekt.
 * Desweiteren wirken sich Zustandsänderungen im referenzierten Objekt direkt
 * auf das Verhalten des mem_fun_f_t-Objekts aus.
 * 
 * \see filter_base
 * \see mem_fun_f
 **/
template <class T, class MemFun>
struct mem_fun_f_t : public filter_base
{
public:
	mem_fun_f_t(T obj, MemFun memFun)
		: obj_(obj)
		, memFun_(memFun)
	{}
	/**
	 * Ruft die im Ctor übergebene Memberfunktion über den im Ctor übergeben
	 * Objektzeiger mit dem Argument entryName auf.
	 **/
	bool operator()(const std::string& entryName) const 
	{return (obj_->*memFun_)(entryName);}
	filter_base* clone() const
	{
		return new mem_fun_f_t<T, MemFun>(obj_, memFun_);
	}
private:
	T obj_;
	MemFun memFun_;
};

/**
 * \ingroup Filter
 * \brief Ermöglicht die Verwendunng von Methoden als Filter für ein
 * dirstream-Objekt.
 * 
 * \see mem_fun_f_t
 * \code
 * class Foo {
 *     public:
 *         bool func(const std::string&);
 * ...
 * };
 * ...
 * Foo f;
 * // dirstream der Foos Methode func als Filter verwendet.
 * dirstream s(".", mem_fun_f(&f, &Foo::func));
 * \endcode
 */
template <class PtrT, class M>
mem_fun_f_t<PtrT, M> mem_fun_f(PtrT o, M memFun)
{
	return mem_fun_f_t<PtrT, M>(o, memFun);
}

 
}

#undef ADD_CLONE_IMPL
#endif
