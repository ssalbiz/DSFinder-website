/////////////////////////////////////////////////////////////////////////////
//	file		:	filter_utils_imp.h
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
//  Implementation des Filter-Combiner Frameworks
//	Achtung: S�mtliche Klassen die in dieser Datei definiert werden sind
//  als Implementationsdetails anzusehen. Sie sollten niemals explizit
//	verwendet werden.
/////////////////////////////////////////////////////////////////////////////
#ifndef FILTER_UTILS_H_INCLUDED
#error dirstream.filterutils - Do not include this file directly! Include fiter_utils.h instead.
#endif
#ifndef FILTER_UTILS_IMPL_H_INCLUDED
#define FILTER_UTILS_IMPL_H_INCLUDED
#include "../smartpointer.h"
#include "../filter_base.h"
#include "filter_asserter.h"
#include <memory>
namespace dirstr { namespace detail {
///////////////////////////////////////////////////////////////////////////////
// Implementation der konkreten bin�ren Operationen
///////////////////////////////////////////////////////////////////////////////
	struct and_op
	{
		template <class F1, class F2>
		bool operator()(const F1& f1, const F2& f2, const std::string& str) const
		{
			return (f1(str) != 0) && (f2(str) != 0);
		}
	};
	struct or_op
	{
		template <class F1, class F2>
		bool operator()(const F1& f1, const F2& f2, const std::string& str) const
		{
			return (f1(str) != 0) || (f2(str) != 0);
		}
	};
	struct xor_op
	{
		template <class F1, class F2>
		bool operator()(const F1& f1, const F2& f2, const std::string& str) const
		{
			return (f1(str) != 0) ^ (f2(str) != 0);
		}
	};

///////////////////////////////////////////////////////////////////////////////
// Implementation der expr-Traits
///////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) && _MSC_VER <= 1200
	template <class Op1, class Op2>
	struct assert_equal_op
	{
		template <bool b> struct operation_types_not_equal;
		template <> struct operation_types_not_equal<true> {typedef int type;};
		static char test(const Op1&);
		static char (&test(...))[2];
		enum {result = sizeof(test(Op2()))};
		typedef operation_types_not_equal<result == 1>::type type;
	};
#endif
	template <class F1, class F2, class Op> struct expr_binary_v;
	template <class F1, class F2, class Op> struct expr_binary_s;
	template <class F> struct expr_not_v;
	template <class F> struct expr_not_s;


	struct ptr_checker
	{
	public:
		static char (&test(const volatile dirstr::filter_base*))[2];
		static char (&test(const volatile void*))[6];
	};

	template <int> struct expr_traits_impl;
	struct expr_traits_types
	{
		enum {
			filter					= sizeof(char),		// filter_base-Objekte, Pr�dikate, Funktionszeiger
			ptr_filter_base			= sizeof(char[2]),	// filter_base*-kompatible Pointer
			shared_ptr_filter_base	= sizeof(char[3]),	// SharedPtr<T> mit T = filter_base*-kompatibel
			shared_ptr_other		= sizeof(char[4]),	// SharedPtr<T>
			expression				= sizeof(char[5]),	// and, or, xor, not
			ptr_other				= sizeof(char[6]),	// allgemeine Pointer 
			shared_ptr_select		= sizeof(char[99]),	
			ptr_select				= sizeof(char[100])
		};
	};

	// Traits f�r Filter-Pr�dikate oder Filter-Funktionen, z.B. op(EinFilter())
	// Filter werden immer im top-level-Ausdruck gebunden, k�nnen
	// deshalb per Referenz gehalten werden.
	// Im endg�ltigen Filter-Objekt werden solche Filter "by-value" gehalten
	// und �ber ihren Copy-Ctor kopiert.
	// ACHTUNG: Im Falle von Filter-Hierarchien droht hier slicing!
	// Z.B.
	// class FilterA {...};
	// class FilterB : public FilterA {};
	// FilterB b;
	// FilterA& a = b;
	// op(a)	-> Slicing. Ein FilterB-Objekt wird zu einem FilterA!
	template <>
	struct expr_traits_impl<expr_traits_types::filter>
	{
		template <class T>
		struct in : assert_is_filter<T>
		{
			typedef T copy_type;
			typedef const T& ref_t;
			static const T& transform(const T& t) {return t;}
			static const T& filter_obj(const T& t)
			{
				return t;
			}
		};
	};

	// Traits f�r Pointer die nach filter_base* konvertiert werden k�nnen.
	// Im endg�ltigen Filter-Objekt wird ein Klon des Objekts per SharedPtr<filter_base>
	// gehalten.
	template <>
	struct expr_traits_impl<expr_traits_types::ptr_filter_base>
	{
		template <class T>	// filter_base*
		struct in
		{
			typedef T ref_t;
			typedef SharedPtr<dirstr::filter_base> copy_type;
			static copy_type transform(const T& t)
			{
				return SharedPtr<dirstr::filter_base>(t->clone());
			}
			static const filter_base& filter_obj(filter_base* t)
			{
				return *t;
			}
		};
		enum {is_filter = 1};
	};

	// Traits f�r Objekt-Pointer die nicht nach filter_base* konvertiert werden k�nnen.
	// Da die Unterst�tzung solcher Pointer auf Compilern ohne partielle Template-
	// Spezialisierung einen extra Trick erfordert, dessen Implementation
	// einen hohen Aufwand bedeutet, werden allgemeine Zeiger zur Zeit nicht unterst�tzt.
	template <>
	struct expr_traits_impl<expr_traits_types::ptr_other>
	{
		template <class T>	// T*
		struct in : assert_is_filter<typename T::PLAIN_POINTER_NOT_SUPPORTED>
		{

		};
	};

	// Traits f�r SharedPtr-Objekte die ein filter_base-Objekt enthalten.
	// Kopieren bedeutet klonen des enthaltenen Filter-Objekts.
	template <>
	struct expr_traits_impl<expr_traits_types::shared_ptr_filter_base>
	{
		template <class T>
		struct in
		{
			typedef T ref_t;
			typedef T copy_type;
			static copy_type transform(const T& t)
			{
				return copy_type(t->clone());
			}
		};
	};

	// Traits f�r alle anderen SharedPtr<T>-Objekte.
	// Kopieren bedeutet Allokation eines neuen Heap-Objekts das �ber den
	// Copy-Ctor von T initialisiert wird.
	// ACHTUNG: Im Falle von Filter-Hierarchien die nicht filter_base
	// als ihre Basisklasse haben, droht hier slicing!
	// Z.B.
	// class FilterA {...};
	// class FilterB : public FilterA {};
	// SharedPtr<FilterA> af( new FilterB() );
	// op(af)	-> Slicing. Ein FilterB-Objekt wird zu einem FilterA!
	template <>
	struct expr_traits_impl<expr_traits_types::shared_ptr_other>
	{
		template <class T>
		struct in
		{
			typedef T ref_t;
			typedef typename T::VT VT;
			typedef T copy_type;
			static copy_type transform(const T& t)
			{
				enum {r = assert_is_filter<VT>::T_IS_NOT_A_VALID_FILTER};
				return copy_type(new VT(*t));
			}
		};
	};

	// Selektor f�r SharedPtr<T>-Objekte.
	// Abh�ngig von T wird entweder nach shared_ptr_filter_base (T ist ein
	// von filter_base abgeleiteter Typ) oder shared_ptr_other
	// verzweigt.
	template <>
	struct expr_traits_impl<expr_traits_types::shared_ptr_select>
	{
		template <class T>
		struct in
		{
		private:
			typedef typename T::PT PT;
			typedef typename T::VT VT;
            enum {temp = sizeof( ptr_checker::test( (PT)0 ) ) };
			enum {result = temp == (unsigned) expr_traits_types::ptr_filter_base ?
					expr_traits_types::shared_ptr_filter_base
					: expr_traits_types::shared_ptr_other
			};
		public:
			typedef T copy_type;
			typedef T ref_t;
			static copy_type transform(const T& t)
			{
				return expr_traits_impl<result>::template in<T>::transform(t);
			}
			static const VT& filter_obj(const T& t)
			{
				return *t;
			}
		};
	};

	// Selektor f�r Objekt-Ptr.
	// Abh�ngig von T wird entweder nach ptr_filter_base (T ist ein
	// von filter_base*-kompatibler Pointer) oder ptr_other
	// verzweigt.
	template <>
	struct expr_traits_impl<expr_traits_types::ptr_select>
	{
		template <class T>	// T*
		struct in
		{
		public:
			enum {result = sizeof( ptr_checker::test( (T)0 ) )};
			enum {PLAIN_POINTER_NOT_SUPPORTED = expr_traits_impl<result>::is_filter};
			typedef SharedPtr<dirstr::filter_base> copy_type;
			typedef T ref_t;
			static copy_type transform(const T& t)
			{
				return expr_traits_impl<result>::template in<T>::transform(t);
			}
			static const filter_base& filter_obj(const T& t)
			{
				return expr_traits_impl<result>::template in<T>::filter_obj(t);
			}
		};
	};

	// Traits f�r fl�chtige Expression-Ausdr�cke wie expr_and_v oder expr_or_v.
	// Da diese Objekte zum Teil erst in den operator-Funktionen erzeugt werden,
	// k�nnen sie nicht per Referenz sondern m�ssen "by value" gehalten werden.
	template <>
	struct expr_traits_impl<expr_traits_types::expression>
	{
		template <class T>	// e.g. exp_and_v
		struct in
		{
			typedef T ref_t;
			typedef typename T::copy_type copy_type;
			static const T& transform(const T& t)
			{
				return t;
			}
		};
	};


	template <class T>
	struct impl_checker
	{
		struct A {};
		struct B : public A {};
		template <class E1, class E2, class Op>
		static char (&test(const expr_binary_v<E1, E2, Op>&, B))[5];
		template <class E>
		static char (&test(const expr_not_v<E>&, B))[5];
		template <class V>
		static char (&test(const SharedPtr<V>&, B))[99];

		template <class Z>
		static char (&test(Z*, B))[100];
		template <class Z>
		static char test(Z z, A);
		template <class R, class A1>
		static char test( R (*)(A1), B);
	};
	template <class T>
	struct expr_traits_selector
	{
	public:
		enum {result = sizeof( impl_checker<T>::test( maker<T>::make_t(), impl_checker<T>::B() ) )};
		typedef typename expr_traits_impl<result>::template in<T> type;
	};
	template <class T>
	struct expr_traits : expr_traits_selector<T>::type
	{
		typedef typename dirstr::detail::expr_traits_selector<T>::type base_t;
		typedef typename base_t::copy_type copy_type;
	};

///////////////////////////////////////////////////////////////////////////////
//	Expression Klassen
///////////////////////////////////////////////////////////////////////////////
// Bei der Konstruktion von Filter-Ausdr�cken werden zwei Phasen unterschieden.
// Die erste Phase umfasst die Zusammensetzung des endg�ltigen Ausdrucks.
// In dieser Phase werden Objekte der Klassen expr und *_v verwendet.
// Diese Klassen halten ihre Filter per Referenz. In dieser Phase
// werden also keinerlei Kopien erzeugt. Das hei�t aber auch, dass solche
// Objekte nur innerhalb der vollst�ndigen Ausdrucks g�ltig sind
// (das _v steht f�r 'volatile' und soll darauf hinweisen, dass die Objekte 
// "fl�chtig", also nur f�r einen kurzen Zeitraum g�ltig sind).
//
// In der zweiten Phase wird ein fertiger Ausdruck transformiert und dabei
// in ein Objekt einer von filter_base abgeleiteten Klasse umgewandelt.
// Bei der Transformation werden aus den *_v-Klassen *_s-Klassen. Diese Klassen
// halten ihre Filter "by value". Bei der Transformation von *_v-Objekten in
// *_s-Objekte werden referenzierte Filter kopiert (das _s steht f�r 'stable' und
// soll darauf hinweisen, dass Objekte dieser Klassen auch au�erhalb des erzeugenden
// Ausdrucks g�ltig sind).
// 
// Beispielablauf:
//
//	A sei ein Filterpr�dikat, B eine von filter_base abgeleitete Klasse
//
//	B* b = new B;
//  filter_base* f = expr_f( op(A()) && op(b) );
//	delete b;
//
//  Phase 1: Erzeugung des boole'schen Ausdrucks:
//	zu beachten: das tempor�re Objekte das durch A() erzeugt wird
//	lebt nur bis zum Ende der "full-expression" (bis zum Semikolon ;)
//	* op(A()) -> expr<A>: referenziert das tempor�re A-Objekt
//  * op(b)   -> expr<B*>: kopiert den Pointer b
//	* expr<A> && expr<B*> -> expr<exr_binary_v<A, B*, and_op> >
//	
//  Phase 2: Transformation
//	Die zweite Phase wird durch die �bergabe des endg�ltigen expr-Objekts
//	an die Funktion expr_f bzw. an einen dirstream-Ctor eingeleitet.
//	* (expr<exr_binary_v<A, B*, and_op> >).clone() -> new expr_f_t< expr_binary_s<A, SharedPtr<B>, and_op> >
//	* exr_binary_v<A, B*, and_op> -> expr_binary_s<A, SharedPtr<B>, and_op>
//		* Das A-Objekt wird �ber seinen Copy-Ctor kopiert.
//		* Das SharedPtr<B>-Objekt wird �ber b->clone() initialisiert
//	


	// Repr�sentiert eine bin�re-Operation im tempor�ren Ausdruck.
	// Element-Filter werden in der Regel nur referenziert!
	template <class F1, class F2, class Op>
	struct expr_binary_v
	{
		expr_binary_v(const F1& f1, const F2& f2)
        : f1_(f1)
        , f2_(f2)
		{}

		typedef F1 left_type;
		typedef F2 right_type;
		typedef Op op_type;
		typedef typename expr_traits<F1>::copy_type copy_type_e1;
		typedef typename expr_traits<F2>::copy_type copy_type_e2;
		typedef expr_binary_s<copy_type_e1, copy_type_e2, Op> copy_type;
		typename expr_traits<F1>::ref_t f1_;
		typename expr_traits<F2>::ref_t f2_;
	};

	// Repr�sentiert eine not-Operation im tempor�ren Ausdruck
	template <class F>
	struct expr_not_v
	{
		expr_not_v(const F& f1)
        : f_(f1)
		{}
		typedef F inner_type;
		typedef typename expr_traits<F>::copy_type copy_type_e;
		typedef expr_not_s<copy_type_e> copy_type;
		typename expr_traits<F>::ref_t f_;
	};

	// Repr�sentiert eine bin�re Operation im endg�ltigen Filter
	// expr_binary_s besitzt nur einen Copy-Ctor (wird f�r das Klonen von
	// Filtern ben�tigt) und einen Initialisierungskonstruktor der ein
	// expr_binary_s aus einem expr_binary_v-Objekt erzeugt.
	// Bei der Konstruktion eines expr_binary_s-Objekts aus einem 
	// expr_binary_v-Objekt werden beide Teilausdr�cke des
	// expr_binary_v-Objekts zuvor transformiert.
	// Transformation bedeutet kopieren der Element-Filter 
	// siehe expr_traits
	template <class F1, class F2, class Op>
	struct expr_binary_s
	{
#if defined(_MSC_VER) && _MSC_VER <= 1200
		
		// Workaround f�r den VC 6.0
		// Dieser Workaround ist n�tig, da der VC 6.0 in Anwesenheit eines
		// Template-Ctors der Form expr_binary_s(const expr_binary_v<OF1, OF2, Op>& o)
		// es vers�umt Code f�r den Copy-Ctor zu generieren.
		template <class Exp>
		expr_binary_s(const Exp& e)
			: f1_(expr_traits<Exp::left_type>::transform(e.f1_))
			, f2_(expr_traits<Exp::right_type>::transform(e.f2_))
		{
			assert_equal_op<Op, Exp::op_type>::result;
		}
#else
		template <class OF1, class OF2>
		expr_binary_s(const expr_binary_v<OF1, OF2, Op>& o)
			: f1_(expr_traits<OF1>::transform(o.f1_))
			, f2_(expr_traits<OF2>::transform(o.f2_))
		{}
#endif
		expr_binary_s(const expr_binary_s& o)
			: f1_(expr_traits<F1>::transform(o.f1_))
			, f2_(expr_traits<F2>::transform(o.f2_))
		{}

		bool operator()(const std::string& str) const
		{
			return Op()(expr_traits<F1>::filter_obj(f1_),
						expr_traits<F2>::filter_obj(f2_),
						str);
		}
	private:
		F1 f1_;	// im endg�ltigen Filter-Objekt werden die beiden Teilausdr�cke
		F2 f2_;	// immer "by value" gehalten.
	};

	// Repr�sentiert eine not-Operation im endg�ltigen Filter.
	// siehe expr_binary_s
	template <class F>
	struct expr_not_s
	{
#if defined(_MSC_VER) && _MSC_VER <= 1200
		template <class Exp>
		expr_not_s(const Exp& e)
			: f_(expr_traits<Exp::inner_type>::transform(e.f_))
		{}
#else
		template <class OF>
		expr_not_s(const expr_not_v<OF>& o)
			: f_(expr_traits<OF>::transform(o.f_))
		{}
#endif
		expr_not_s(const expr_not_s& o)
			: f_(expr_traits<F>::transform(o.f_))
		{}

		bool operator()(const std::string& str) const
		{
			return	!expr_traits<F>::filter_obj(f_)(str);
		}
	private:
		F f_;
	};

	template <class F> 
	struct expr;

	// Endg�ltige Filter-Klasse f�r Filter-Ausdr�cke.
	// Objekte dieser Klassen halten *Kopien* der Element-Filter.
	template <class Exp>
	class expr_f_t : public filter_base
	{
			// template <class T> friend class expr;
	public:	// sollte private: sein, aber zuviele Compiler haben Probleme mit Template-Friends
		template <class OExp>
		expr_f_t(const detail::expr<OExp>& other)
			: exp_(detail::expr_traits<OExp>::transform(other.f_))
		{}
	public:
		expr_f_t(const expr_f_t& other)
			: exp_(detail::expr_traits<Exp>::transform(other.exp_))
		{}
		
		bool operator()(const std::string& str) const
		{
			return detail::expr_traits<Exp>::filter_obj(exp_)(str);
		}
		filter_base* clone() const
		{
			return new expr_f_t(*this);
		}
	private:
		Exp exp_;
	};

	
	// Objekte dieser Klasse werden beim Zusammenbau von boole'schen Ausdr�cken
	// erzeugt. Das Endergebnis eines boole'schen Ausdrucks ist
	// ebenfalls ein (tempor�res) Objekt dieser Klasse.
	// Achtung: 
	//	Element-Filter werden von expr<T>-Objekten nur per Referenz gehalten.
	//	Diese Referenzen sind in der Regel nur bis zum Ende des
	//	vollst�ndigen Ausdrucks in dem das expr<T>-Objekt erzeugt wurde g�ltig.
	//	expr<T>-Objekte sollten *niemals* explizit verwendet oder kopiert werden.
	//	expr<T>-Objekte sollten nur als anonyme tempor�re Objekte verwendet werden und
	//	das auch nur in drei Situationen:
	//	a) Als Operand f�r einen logischen Operator
	//	Bsp: op(a) && op(b) && op(c) 
	//
	//	b) Als Argument f�r einen dirstream-Ctor bzw. einer dirstream::open-Funktion
	//	Bsp: dirstream str(".", op(a) && op(b));
	//
	//	c) Als Argument f�r die Funktion expr_f
	//
	// Insbesondere ist die m�gliche Konvertierung nach filter_base nur 
	// ein Implementationsdetail von der niemals explizit gebrauch gemacht
	// werden soll (Ausnahme siehe b) ).
	// Code wie:
	// const filter_base& f = op(a) && op(b);
	// ...
	// f�hrt garantiert fr�her oder sp�ter zu einer Katastrophe.
	template <class F> 
	struct expr : public filter_base
	{
	public:
		expr(const F& f)
			: f_(f)
		{}
		// Transformiert den Filter-Ausdruck in ein regul�res filter_base-Objekt
		dirstr::filter_base* clone() const
		{
			typedef typename detail::expr_traits<F>::copy_type copy_type;
			return new dirstr::detail::expr_f_t<copy_type>(*this);            	
		}
		
		// Sollte *niemals* aufgerufen werden.
		bool operator()(const std::string& str) const
		{
			std::auto_ptr<filter_base> f(this->clone());
			return (*f)(str);
		}
		typename detail::expr_traits<F>::ref_t f_;
	};
	

}	} // namespace dirstr::detail

#endif
