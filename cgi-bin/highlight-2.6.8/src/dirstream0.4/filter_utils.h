/////////////////////////////////////////////////////////////////////////////
//	file		:	filter_utils.h
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
//  Definition des Filter-Combiner-Interfaces
/////////////////////////////////////////////////////////////////////////////
#ifndef FILTER_UTILS_H_INCLUDED
#define FILTER_UTILS_H_INCLUDED
/*! \defgroup BoolFilter Expression-Filter und logische Operationen
\par Motivation
H�ufig l�sst sich ein Filterkriterium als Kombination einfacherer Filter
ausdr�cken, z.B. k�nnte ein Anwender alle cpp-Dateien die nicht tempor�re
Dateien sind ausw�hlen wollen. 
Unter der Annahme, dass der Anwender bereits eine NoTemporaries-Filter-Klasse
implementiert hat, k�nnte er nun folgendes schreiben:
\code
class CppsNotTemporary
{
public:
    CppsNotTemporary()
		: cpp_f_("*.cpp")
	{}
	bool operator()(const std::string& e) const
	{
		return cpp_f_(e) && !notTemp_f_(e);
	}
private:
	pattern_f cpp_f_;
	NoTemporaries notTemp_f_;
};
...
dirstream str(".", pred_f(CppsNotTemporary()));
...
\endcode
Die Implementation solcher Funktionsobjekte ist zwar einfach, f�hrt aber schnell
zu einer explosionsartigen Anh�ufung kleiner Pr�dikate, die sich nur schlecht
wiederverwenden lassen. Aus diesem Grund bietet dirstream die M�glichkeit
Filter �ber logische Operationen zu verkn�pfen. Intern wird dann automatisch
ein \ref Filter "Filter-Objekt" erzeugt, das den fertigen Ausdruck repr�sentiert.
\par Benutzung
Die Benutzung des Expression-Filter Frameworks ist denkbar einfach.
Anstelle eines einfachen Filters �bergibt man einen boole'schen Ausdruck an
den Konstruktor von dirstream.
\code
dirstream str(".", op(pattern_f("*.cpp")) && !op(NoTemporaries()));
\endcode
Wie man dem Code entnehmen kann, m�ssen Filter, die �ber logische Operationen
verkn�pft werden sollen, zuvor an die Funktion \ref dirstr::op "op" �bergeben werden.
Dieser kleine Umweg ist n�tig, damit auch Zeiger auf Filter �bergeben werden
k�nnen (Ein �berladener Operator muss mindestens ein Argument vom Typ UDT besitzen).
\code
filter_base* f1 = filterFactory.getFilterA();
filter_base* f2 = filterFactory.getFilterB();
dirstream str(".", op(f1) || op(f2));
delete f2;
delete f1;
\endcode
\par Arbeitsweise
Die Kombination von Filtern geschieht intern �ber <a href="http://osl.iu.edu/~tveldhui/papers/Expression-Templates/exprtmpl.html">Expression-Templates</a>.
Das entstehende Expression-Objekt wird dann in ein \ref Filter "Filter-Objekt"
transformiert. Die Implementation des Expression-Frameworks ist relativ kompliziert 
(im Vergleich zur Komplexit�t der restlichen Bibliothek), f�r den Anwender zum Gl�ck aber
auch irrelevant.\n
\b Anwender des Expression-Frameworks <b>m�ssen lediglich drei Dinge beachten</b>:
-# Klassen im Namespace dirstr::detail d�rfen nicht direkt verwendet werden. Sie
sind Implementationsdetails der Bibliothek.
-# Das Ergebnis eines boole'schen Ausdrucks darf nur in drei Kontexten verwendet werden
	-# als Argument f�r die Funktion \ref dirstr::expr_f "expr_f"
	-# als Argument f�r einen Filter-Parameter eines \ref dirstr::dirstream "dirstream-Konstruktors"
	-# als Argument f�r einen Filter-Parameter einer \ref dirstr::dirstream "dirstream::open"-Funktion
-# Bei der Umwandlung eines Expression-Objekts in ein \ref Filter "Filter-Objekt" werden alle enthaltenen 
Filter kopiert. D.h konkret:
	- \ref Filter "Filter-Objekte", Pr�dikate und Funktionszeiger werden �ber ihren Copy-Konstruktor kopiert.
	- Zeiger auf Objekte einer von \ref dirstr::filter_base "filter_base" abgeleiteten Klasse 
	werden �ber den Aufruf der Methode \ref dirstr::filter_base::clone "clone" kopiert.
	- Der Value-Type eines SharedPtrs wird gem��t der ersten beiden Regeln kopiert.
	.
\attention
Da Filter-Objekte und Pr�dikate �ber ihren Copy-Konstruktor kopiert werden,
kann es hier unter Umst�nden zu Slicing-Problemen kommen, n�mlich genau
dann, wenn der Anwender ein Objekt einer abgeleiteten Klasse �ber eine
Referenz auf eine konkreten Basisklasse an die Funktion \ref dirstr::op "op" �bergibt.
\code
class FilterA {...};
class FilterB : public FilterA {};
FilterB b;
FilterA& a = b;
op(a) // Ups! Aus FilterB wird ein FilterA
\endcode
\par Einschr�nkungen
- Die Unterst�tzung beliebiger Objekt-Pointer ohne die Verwendung von partieller Templatespezialisierung 
l�sst sich nur �ber einen nichttrivialen Umweg implementieren. 
Da dirstream aber m�glichst auch auf Compilern, die keine partielle Templatespezialisierung
erlauben, �bersetzbar sein soll und da der Aufwand der Implementation des Umwegs h�her als
der Nutzen ist, werden vorerst nur filter-base-kompatible Objekt-Pointer unterst�tzt. 
- Die Menge der unterst�tzten Objekte sowie deren Kopiersemantik ist derzeit fest vorgegeben
und kann nicht ver�ndert werden. Hier w�re eine konfigurierbarer Traits-Ansatz w�nschenswert.
Da die Implementation eines solchen Ansatzes ohne die Verwendung von partieller Templatespezialisierung
aber recht aufwendig ist, wird ein solcher Ansatz erst verfolgt, wenn 
sein Nutzen durch einen konkreten Bedarf bewiesen ist.
 */
#include "detail/filter_utils_impl.h"
namespace dirstr {
	/**
	 * \ingroup BoolFilter
	 * \brief Markiert einen Filter als Operand f�r eine logische Operation.
	 *
	 * \param f Ein filter-kompatibles Objekt.
	 *
	 * \par 
	 * filter-kompatible ist:
	 * - ein Objekt einer Klasse die von \ref dirstr::filter_base "filter_base" erbt
	 * - ein Zeiger auf ein Objekt einer Klasse die von \ref dirstr::filter_base "filter_base" erbt.
	 * - ein Funktionsobjekt das mit einem std::string-Argument
	 * aufgerufen werden kann und ein bool-kompatiblen R�ckgabewert hat.
	 * - ein Zeiger auf eine Funktion die mit einem std::string-Argument
	 * aufgerufen werden kann und ein bool-kompatiblen R�ckgabewert hat.
	 * - ein SharedPtr-Objekt dessen Value-Type eine der vorherigen Bedingungen erf�llt
	 *
	 * \attention
	 *	Argumente die an op �bergeben werden, gehen \b nicht in den Besitz von op �ber.
	 *	Das gilt insbesondere auch f�r Objekte die per Pointer �bergeben werden.
	 *	D.h. Code wie:
	 *  \code
	 *	op(new pattern_f("*.cpp"));
	 *	\endcode
	 *	l�sst sich zwar problemlos kompilieren, f�hrt zur Laufzeit aber zu
	 *	einem \b Speicherloch.
	 *
	 * \return Ein Objekt einer nicht spezifizierten Klasse das als Argument
	 * f�r einen logische Operator verwendet werden kann.
	 *
	 **/
	template <class F>
	detail::expr<F> op(const F& f)
	{
		return detail::expr<F>(f);
	}

	/**
	 * \ingroup BoolFilter
	 * \brief UND-verkn�pft die Ausdr�cke e1 und e2.
	 *
	 * Der resultierende Filter liefert f�r einen Eintrag e true, 
	 * gdw: (e1(e) != 0) && (e2(e) != 0)
	 *
	 * \par Beispiel-Benutzung:
	 * \code
	 * bool filter1(const std::string&); 
	 * bool filter2(const std::string&);
	 * ...
	 * filter_base* f = expr_f( op(&filter1) && op(&filter2) );
	 * ...
	 * delete f;
	 * \endcode
	 **/
	template <class E1, class E2>	
	detail::expr<detail::expr_binary_v<E1, E2, detail::and_op> >
	operator && (const detail::expr<E1>& e1, const detail::expr<E2>& e2) 
	{	
		return detail::expr<detail::expr_binary_v<E1, E2, detail::and_op> >( 
			detail::expr_binary_v<E1, E2, detail::and_op>(e1.f_, e2.f_) 
		); 
	} 
	
	/**
	 * \ingroup BoolFilter
	 * \brief ODER-verkn�pft die Ausdr�cke e1 und e2.
	 *
	 * Der resultierende Filter liefert f�r einen Eintrag e true, 
	 * gdw: (e1(e) != 0) || (e2(e) != 0)
	 *
	 * \see operator && (const detail::expr<E1>& e1, const detail::expr<E2>& e2) 
	 **/
	template <class E1, class E2>
	detail::expr<detail::expr_binary_v<E1, E2, detail::or_op> >
	operator || (const detail::expr<E1>& e1, const detail::expr<E2>& e2)
	{
		return detail::expr<detail::expr_binary_v<E1, E2, detail::or_op> >(
			detail::expr_binary_v<E1, E2, detail::or_op>(e1.f_, e2.f_)
		);
	}	

	/**
	 * \ingroup BoolFilter
	 * \brief XOR-verkn�pft die Ausdr�cke e1 und e2.
	 *
	 * Der resultierende Filter liefert f�r einen Eintrag e true, 
	 * gdw: (e1(e) != 0) ^ (e2(e) != 0)
	 *
	 * \see operator && (const detail::expr<E1>& e1, const detail::expr<E2>& e2) 
	 **/
	template <class E1, class E2>
	detail::expr<detail::expr_binary_v<E1, E2, detail::xor_op> >
	operator ^ (const detail::expr<E1>& e1, const detail::expr<E2>& e2)
	{
		return detail::expr<detail::expr_binary_v<E1, E2, detail::xor_op> >(
			detail::expr_binary_v<E1, E2, detail::xor_op>(e1.f_, e2.f_)
		);
	}	

	/**
	 * \ingroup BoolFilter
	 * \brief Negiert den Filter der durch den Ausdruck e repr�sentiert wird
	 *
	 * Der resultierende Filter liefert f�r einen Eintrag entry true, 
	 * gdw: e(entry) == false
	 *
	 * \par Benutzung:
	 * \code
	 * bool filter1(const std::string&); 
	 * ...
	 * filter_base* f = expr_f( !op(&filter1) );
	 * delete f;
	 * \endcode
	 **/
	template <class E>
	detail::expr<detail::expr_not_v<E> >
	operator!(const detail::expr<E>& e)
	{
		return detail::expr<detail::expr_not_v<E> >(
			detail::expr_not_v<E>(e.f_)
		);
	}

	/**
	 * \ingroup BoolFilter
	 * \brief Transformiert einen boole'schen-Filterausdruck in ein filter_base-Objekt.
	 *
	 * \return Liefert einen \ref dirstr::filter_base "filter_base"-Pointer auf ein dynamisch alloziertes 
	 * Objekt einer nicht spezifizierten Klasse.
	 * 
	 * \note Der Aufrufer ist f�r die Zerst�rung des Objekts verantwortlich.
	 **/
	template <class T>
	filter_base* expr_f(const dirstr::detail::expr<T>& e)
	{
		return e.clone();
	}

}	// namespace dirstr

#endif

