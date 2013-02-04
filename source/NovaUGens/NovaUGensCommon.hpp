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

#include "SC_PlugIn.hpp"

#ifndef NOVAUGENSCOMMON_HPP
#define NOVAUGENSCOMMON_HPP

struct NovaUnit:
	public SCUnit
{
	template <typename FloatType, typename Functor>
	struct ScalarSignal2
	{
		ScalarSignal2(FloatType value, Functor const & f):
			value(f(value))
		{}

		FloatType consume() const
		{
			return value;
		}

		FloatType value;
	};

	template <typename FloatType, typename Functor>
	struct SlopeSignal2:
		Functor
	{
		SlopeSignal2(FloatType value, FloatType slope, Functor const & f):
			Functor(f), value(value), slope(slope)
		{}

		FloatType consume()
		{
			FloatType ret = value;
			value += slope;
			return Functor::operator()(ret);
		}

		FloatType value, slope;
	};

	template <typename FloatType, typename Functor>
	struct AudioSignal2:
		Functor
	{
		AudioSignal2(const FloatType * pointer, Functor const & f):
			Functor(f), pointer(pointer)
		{}

		FloatType consume()
		{
			return Functor::operator()(*pointer++);
		}

		const FloatType * pointer;
	};

	template <typename FloatType, typename Functor>
	inline ScalarSignal2<FloatType, Functor> makeScalar(FloatType value, Functor const & f) const
	{
		return ScalarSignal2<FloatType, Functor>(value, f);
	}

	template <typename FloatType>
	inline ScalarSignal<FloatType> makeScalar(FloatType value) const
	{
		return SCUnit::makeScalar(value);
	}

	template <typename FloatType, typename Functor>
	inline SlopeSignal2<FloatType, Functor> makeSlope(FloatType next, FloatType last, Functor const & f) const
	{
		return SlopeSignal2<FloatType, Functor>(last, calcSlope(next, last), f);
	}

	template <typename FloatType>
	inline SlopeSignal<FloatType> makeSlope(FloatType next, FloatType last) const
	{
		return SCUnit::makeSlope(next, last);
	}

	template <typename Functor>
	inline AudioSignal2<float, Functor> makeSignal(int index, Functor const & f) const
	{
		const float * input = in(index);
		return AudioSignal2<float, Functor>(input, f);
	}

	template <typename Functor>
	inline AudioSignal<float> makeSignal(int index) const
	{
		return SCUnit::makeSignal(index);
	}
};


#endif // NOVAUGENSCOMMON_HPP
