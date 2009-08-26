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
Häufig lässt sich ein Filterkriterium als Kombination einfacherer Filter
ausdrücken, z.B. könnte ein Anwender alle cpp-Dateien die nicht temporäre
Dateien sind auswählen wollen. 
Unter der Annahme, dass der Anwender bereits eine NoTemporaries-Filter-Klasse
implementiert hat, könnte er nun folgendes schreiben:
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
Die Implementation solcher Funktionsobjekte ist zwar einfach, führt aber schnell
zu einer explosionsartigen Anhäufung kleiner Prädikate, die sich nur schlecht
wiederverwenden lassen. Aus diesem Grund bietet dirstream die Möglichkeit
Filter über logische Operationen zu verknüpfen. Intern wird dann automatisch
ein \ref Filter "Filter-Objekt" erzeugt, das den fertigen Ausdruck repräsentiert.
\par Benutzung
Die Benutzung des Expression-Filter Frameworks ist denkbar einfach.
Anstelle eines einfachen Filters übergibt man einen boole'schen Ausdruck an
den Konstruktor von dirstream.
\code
dirstream str(".", op(pattern_f("*.cpp")) && !op(NoTemporaries()));
\endcode
Wie man dem Code entnehmen kann, müssen Filter, die über logische Operationen
verknüpft werden sollen, zuvor an die Funktion \ref dirstr::op "op" übergeben werden.
Dieser kleine Umweg ist nötig, damit auch Zeiger auf Filter übergeben werden
können (Ein überladener Operator muss mindestens ein Argument vom Typ UDT besitzen).
\code
filter_base* f1 = filterFactory.getFilterA();
filter_base* f2 = filterFactory.getFilterB();
dirstream str(".", op(f1) || op(f2));
delete f2;
delete f1;
\endcode
\par Arbeitsweise
Die Kombination von Filtern geschieht intern über <a href="http://osl.iu.edu/~tveldhui/papers/Expression-Templates/exprtmpl.html">Expression-Templates</a>.
Das entstehende Expression-Objekt wird dann in ein \ref Filter "Filter-Objekt"
transformiert. Die Implementation des Expression-Frameworks ist relativ kompliziert 
(im Vergleich zur Komplexität der restlichen Bibliothek), für den Anwender zum Glück aber
auch irrelevant.\n
\b Anwender des Expression-Frameworks <b>müssen lediglich drei Dinge beachten</b>:
-# Klassen im Namespace dirstr::detail dürfen nicht direkt verwendet werden. Sie
sind Implementationsdetails der Bibliothek.
-# Das Ergebnis eines boole'schen Ausdrucks darf nur in drei Kontexten verwendet werden
	-# als Argument für die Funktion \ref dirstr::expr_f "expr_f"
	-# als Argument für einen Filter-Parameter eines \ref dirstr::dirstream "dirstream-Konstruktors"
	-# als Argument für einen Filter-Parameter einer \ref dirstr::dirstream "dirstream::open"-Funktion
-# Bei der Umwandlung eines Expression-Objekts in ein \ref Filter "Filter-Objekt" werden alle enthaltenen 
Filter kopiert. D.h konkret:
	- \ref Filter "Filter-Objekte", Prädikate und Funktionszeiger werden über ihren Copy-Konstruktor kopiert.
	- Zeiger auf Objekte einer von \ref dirstr::filter_base "filter_base" abgeleiteten Klasse 
	werden über den Aufruf der Methode \ref dirstr::filter_base::clone "clone" kopiert.
	- Der Value-Type eines SharedPtrs wird gemäßt der ersten beiden Regeln kopiert.
	.
\attention
Da Filter-Objekte und Prädikate über ihren Copy-Konstruktor kopiert werden,
kann es hier unter Umständen zu Slicing-Problemen kommen, nämlich genau
dann, wenn der Anwender ein Objekt einer abgeleiteten Klasse über eine
Referenz auf eine konkreten Basisklasse an die Funktion \ref dirstr::op "op" übergibt.
\code
class FilterA {...};
class FilterB : public FilterA {};
FilterB b;
FilterA& a = b;
op(a) // Ups! Aus FilterB wird ein FilterA
\endcode
\par Einschränkungen
- Die Unterstützung beliebiger Objekt-Pointer ohne die Verwendung von partieller Templatespezialisierung 
lässt sich nur über einen nichttrivialen Umweg implementieren. 
Da dirstream aber möglichst auch auf Compilern, die keine partielle Templatespezialisierung
erlauben, übersetzbar sein soll und da der Aufwand der Implementation des Umwegs höher als
der Nutzen ist, werden vorerst nur filter-base-kompatible Objekt-Pointer unterstützt. 
- Die Menge der unterstützten Objekte sowie deren Kopiersemantik ist derzeit fest vorgegeben
und kann nicht verändert werden. Hier wäre eine konfigurierbarer Traits-Ansatz wünschenswert.
Da die Implementation eines solchen Ansatzes ohne die Verwendung von partieller Templatespezialisierung
aber recht aufwendig ist, wird ein solcher Ansatz erst verfolgt, wenn 
sein Nutzen durch einen konkreten Bedarf bewiesen ist.
 */
#include "detail/filter_utils_impl.h"
namespace dirstr {
	/**
	 * \ingroup BoolFilter
	 * \brief Markiert einen Filter als Operand für eine logische Operation.
	 *
	 * \param f Ein filter-kompatibles Objekt.
	 *
	 * \par 
	 * filter-kompatible ist:
	 * - ein Objekt einer Klasse die von \ref dirstr::filter_base "filter_base" erbt
	 * - ein Zeiger auf ein Objekt einer Klasse die von \ref dirstr::filter_base "filter_base" erbt.
	 * - ein Funktionsobjekt das mit einem std::string-Argument
	 * aufgerufen werden kann und ein bool-kompatiblen Rückgabewert hat.
	 * - ein Zeiger auf eine Funktion die mit einem std::string-Argument
	 * aufgerufen werden kann und ein bool-kompatiblen Rückgabewert hat.
	 * - ein SharedPtr-Objekt dessen Value-Type eine der vorherigen Bedingungen erfüllt
	 *
	 * \attention
	 *	Argumente die an op übergeben werden, gehen \b nicht in den Besitz von op über.
	 *	Das gilt insbesondere auch für Objekte die per Pointer übergeben werden.
	 *	D.h. Code wie:
	 *  \code
	 *	op(new pattern_f("*.cpp"));
	 *	\endcode
	 *	lässt sich zwar problemlos kompilieren, führt zur Laufzeit aber zu
	 *	einem \b Speicherloch.
	 *
	 * \return Ein Objekt einer nicht spezifizierten Klasse das als Argument
	 * für einen logische Operator verwendet werden kann.
	 *
	 **/
	template <class F>
	detail::expr<F> op(const F& f)
	{
		return detail::expr<F>(f);
	}

	/**
	 * \ingroup BoolFilter
	 * \brief UND-verknüpft die Ausdrücke e1 und e2.
	 *
	 * Der resultierende Filter liefert für einen Eintrag e true, 
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
	 * \brief ODER-verknüpft die Ausdrücke e1 und e2.
	 *
	 * Der resultierende Filter liefert für einen Eintrag e true, 
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
	 * \brief XOR-verknüpft die Ausdrücke e1 und e2.
	 *
	 * Der resultierende Filter liefert für einen Eintrag e true, 
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
	 * \brief Negiert den Filter der durch den Ausdruck e repräsentiert wird
	 *
	 * Der resultierende Filter liefert für einen Eintrag entry true, 
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
	 * \note Der Aufrufer ist für die Zerstörung des Objekts verantwortlich.
	 **/
	template <class T>
	filter_base* expr_f(const dirstr::detail::expr<T>& e)
	{
		return e.clone();
	}

}	// namespace dirstr

#endif

