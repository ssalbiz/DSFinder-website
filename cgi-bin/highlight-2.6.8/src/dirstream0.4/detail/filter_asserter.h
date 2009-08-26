/////////////////////////////////////////////////////////////////////////////
//	file		:	filter_asserter.h
//  copyright	:	(C) 2002-2004 Benjamin Kaufmann
//  email		:	hume@c-plusplus.de
//	internet	:	http://bens.c-plusplus.info/
//
//  assert_is_filter prüft ob T ein Typ ist, der mit einem Argument vom
//  Type std::string aufgerufen werden kann und ein bool-kompatiblen
// 	Rückgabewert liefert.
/////////////////////////////////////////////////////////////////////////////
#ifndef FILTER_ASSERTER_H_INCLUDED
#define FILTER_ASSERTER_H_INCLUDED
namespace dirstr { class filter_base; }
namespace dirstr {	namespace detail {
	template <class T>
	struct maker
	{
		static T make_t();
	};

	char check_result_type(bool);
	template <class T>
	struct assert_is_filter
	{
		enum {T_IS_NOT_A_VALID_FILTER =
			sizeof( check_result_type( maker<T>::make_t()(std::string()) ) ) == sizeof(char)
		};
	};
	template <>
	struct assert_is_filter<dirstr::filter_base*>
	{};
}}

#endif
