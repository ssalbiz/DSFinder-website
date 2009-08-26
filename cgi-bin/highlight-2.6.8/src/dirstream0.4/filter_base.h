/////////////////////////////////////////////////////////////////////////////
//	file		:	filter_base.h
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
//  Definition des Filter-Interfaces
/////////////////////////////////////////////////////////////////////////////

#ifndef FILTER_BASE_H_INCLUDED
#define FILTER_BASE_H_INCLUDED
#include <string>
namespace dirstr
{
	/**
	 * \ingroup Filter
	 * \brief
	 * Filter-Interface das von der dirstream-Klasse für die Filterung von
	 * Verzeichnis-Einträgen und Unterverzeichnissen verwendet wird.
	 **/
	class filter_base
	{
	public:
		virtual ~filter_base();
		/**
		 * Filterfunktion die entscheidet ob ein Eintrag ausgewählt werden soll
		 * oder nicht.
		 * @return true, falls der Eintrag ausgewählt werden soll. false sonst.
		 */
		virtual bool operator()(
								const std::string& //!< Name des aktuellen Verzeichniseintrags.
								) const = 0;
		
		/**
		 * "virtueller" Copy-Konstruktor.
		 * Muss eine dynamisch allozierte Kopie von *this liefern.
		 **/
		virtual filter_base* clone() const = 0;
	};

}

#endif

