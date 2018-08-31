/*
 *
 *    Copyright (C) 2013 Tim Blechmann
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef NOVAUTILS_HPP
#define NOVAUTILS_HPP

#include <boost/type_traits/is_integral.hpp>
#include <boost/static_assert.hpp>

namespace nova {

namespace loop_detail {

template<typename T, typename IntType>
inline void run_object(T const & t, void (T::*func)() const, IntType)
{
	(t.*func)();
}

template<typename T, typename IntType>
inline void run_object(T & t, void (T::*func)(), IntType)
{
	(t.*func)();
}

template<typename T, typename IntType>
inline void run_object(T const & t, void (T::*func)(IntType) const, IntType x)
{
	(t.*func)(x);
}


template<typename T, typename IntType>
inline void run_object(T & t, void (T::*func)(IntType), IntType x)
{
	(t.*func)(x);
}

template<typename T, typename IntType>
inline void run_object(T & t, IntType x)
{
	run_object(t, &T::operator(), x);
}

template<typename IntType>
inline void run_object(void (*t)(IntType), IntType i)
{
	(*t)(i);
}

template<typename IntType>
inline void run_object(void (*t)(), IntType)
{
	(*t)();
}

} /* namespace loop_detail */

template <typename IntType, typename Functor>
inline void loop(IntType loop_count, Functor & f)
{
	BOOST_STATIC_ASSERT(boost::is_integral<IntType>::value);

	for (IntType i = 0; i != loop_count; ++i)
		loop_detail::run_object(f, i);
}

template <typename IntType, typename Functor>
inline void loop(IntType loop_count, Functor const & f)
{
	BOOST_STATIC_ASSERT(boost::is_integral<IntType>::value);

	for (IntType i = 0; i != loop_count; ++i)
		loop_detail::run_object(f, i);
}

}

#endif // NOVAUTILS_HPP
