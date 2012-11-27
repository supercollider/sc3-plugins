/*
 *    Copyright (C) 2012 Tim Blechmann
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

#ifndef INTERPOLATORS_HPP
#define INTERPOLATORS_HPP

// interpolators adapted from:
// Olli Niemitalo: Polynomial Interpolators for High-Quality Resampling of Oversampled Audio
// we are not oversampling, but the quality is still really good!

// 4-point, 5th-order 2nd-order-osculating (z-form)
template <typename Float>
inline Float interp_4_5_2nd(Float phase, Float ym1, Float y0, Float y1, Float y2)
{
	Float z = phase - (Float)(1/2.0);
	Float even1 = ym1+y2, odd1 = ym1-y2;
	Float even2 = y0+y1, odd2 = y0-y1;
	Float c0 = (Float)(9/16.0)*even2 - (Float)(1/16.0)*even1;
	Float c1 = (Float)(3/16.0)*odd1 - (Float)(25/16.0)*odd2;
	Float c2 = (Float)(1/4.0)*(even1-even2);
	Float c5 = odd1 - 3*odd2;
	return (((c5*z*z-c5)*z+c2)*z+c1)*z+c0;
}

// Optimal 2x (4-point, 4th-order) (z-form)
template <typename Float>
inline Float opt_2x_4p_4o(Float phase, Float ym1, Float y0, Float y1, Float y2)
{
	Float z = phase - (Float)1/2.0;
	Float even1 = y1+y0, odd1 = y1-y0;
	Float even2 = y2+ym1, odd2 = y2-ym1;
	Float c0 = even1*(Float)0.45645918406487612 + even2*(Float)0.04354173901996461;
	Float c1 = odd1*(Float)0.47236675362442071 + odd2*(Float)0.17686613581136501;
	Float c2 = even1*(Float)-0.253674794204558521 + even2*(Float)0.25371918651882464;
	Float c3 = odd1*(Float)-0.37917091811631082 + odd2*(Float)0.11952965967158000;
	Float c4 = even1*(Float)0.04252164479749607 + even2*-(Float)0.04289144034653719;
	return (((c4*z+c3)*z+c2)*z+c1)*z+c0;
}

// Optimal 2x (6-point, 5th-order) (z-form)
template <typename Float>
inline Float opt_2x_6p_5o(Float phase, Float ym2, Float ym1, Float y0, Float y1, Float y2, Float y3)
{
	// Optimal 2x (6-point, 5th-order) (z-form)
	Float z = phase - 1/2.0;
	Float even1 = y1+y0, odd1 = y1-y0;
	Float even2 = y2+ym1, odd2 = y2-ym1;
	Float even3 = y3+ym2, odd3 = y3-ym2;
	Float c0 = even1*(Float)0.40513396007145713 + even2*(Float)0.09251794438424393
	+ even3*(Float)0.00234806603570670;
	Float c1 = odd1*(Float)0.28342806338906690 + odd2*(Float)0.21703277024054901
	+ odd3*(Float)0.01309294748731515;
	Float c2 = even1*(Float)-0.191337682540351941 + even2*(Float)0.16187844487943592
	+ even3*(Float)0.02946017143111912;
	Float c3 = odd1*(Float)-0.16471626190554542 + odd2*(Float)-0.00154547203542499
	+ odd3*(Float)0.03399271444851909;
	Float c4 = even1*(Float)0.03845798729588149 + even2*(Float)-0.05712936104242644
	+ even3*(Float)0.01866750929921070;
	Float c5 = odd1*(Float)0.04317950185225609 + odd2*(Float)-0.01802814255926417
	+ odd3*(Float)0.00152170021558204;
	return ((((c5*z+c4)*z+c3)*z+c2)*z+c1)*z+c0;
}

template <typename Float>
inline Float bspline_4p_3o(Float phase, Float ym1, Float y0, Float y1, Float y2)
{
	Float ym1py1 = ym1+y1;
	Float c0 = (Float)(1/6.0)*ym1py1 + (Float)(2/3.0)*y0;
	Float c1 = (Float)(1/2.0)*(y1-ym1);
	Float c2 = (Float)(1/2.0)*ym1py1 - y0;
	Float c3 = (Float)(1/2.0)*(y0-y1) + (Float)(1/6.0)*(y2-ym1);
	return ((c3*phase+c2)*phase+c1)*phase+c0;
}

template <typename Float>
inline Float bspline_6p_5o(Float phase, Float ym2, Float ym1, Float y0, Float y1, Float y2, Float y3)
{
	Float ym2py2 = ym2+y2, ym1py1 = ym1+y1;
	Float y2mym2 = y2-ym2, y1mym1 = y1-ym1;
	Float sixthym1py1 = (Float)(1/6.0)*ym1py1;
	Float c0 = (Float)(1/120.0)*ym2py2 + (Float)(13/60.0)*ym1py1 + (Float)(11/20.0)*y0;
	Float c1 = (Float)(1/24.0)*y2mym2 + (Float)(5/12.0)*y1mym1;
	Float c2 = (Float)(1/12.0)*ym2py2 + sixthym1py1 - (Float)(1/2.0)*y0;
	Float c3 = (Float)(1/12.0)*y2mym2 - (Float)(1/6.0)*y1mym1;
	Float c4 = (Float)(1/24.0)*ym2py2 - sixthym1py1 + (Float)(1/4.0)*y0;
	Float c5 = (Float)(1/120.0)*(y3-ym2) + (Float)(1/24.0)*(ym1-y2) + (Float)(1/12.0)*(y1-y0);
	return ((((c5*phase+c4)*phase+c3)*phase+c2)*phase+c1)*phase+c0;
}

#endif // INTERPOLATORS_HPP
