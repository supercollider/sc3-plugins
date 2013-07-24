/*
 BEASTmulch UGens
 Copyright (C) 2009 Scott Wilson

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	02110-1301	USA

 http://www.beast.bham.ac.uk/research/mulch.shtml
 beastmulch-info@contacts.bham.ac.uk

 The BEASTmulch project was supported by a grant from the Arts and Humanities Research Council of the UK: http://www.ahrc.ac.uk
*/


/*

This version ported from the PD implementation by Scott Wilson

Copyright

This software is being provided to you, the licensee, by Ville Pulkki,
under the following license. By obtaining, using and/or copying this
software, you agree that you have read, understood, and will comply
with these terms and conditions: Permission to use, copy, modify and
distribute, including the right to grant others rights to distribute
at any tier, this software and its documentation for any purpose and
without fee or royalty is hereby granted, provided that you agree to
comply with the following copyright notice and statements, including
the disclaimer, and that the same appear on ALL copies of the software
and documentation, including modifications that you make for internal
use or for distribution:


 Written by Ville Pulkki 1999
 Helsinki University of Technology
 and
 Unversity of California at Berkeley

*/

/*
 SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
 http://www.audiosynth.com

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "SC_PlugIn.h"
#include <cmath>
#include <limits>
#include <stdio.h>

#ifdef NOVA_SIMD
#include "simd_memory.hpp"
#include "simd_binary_arithmetic.hpp"

using nova::wrap_argument;

#ifdef __GNUC__
#define inline_functions __attribute__ ((flatten))
#else
#define inline_functions
#endif

#endif

#define RES_ID 9171					/* resource ID for assistance (we'll add that later) */
#define MAX_LS_SETS 100				/* maximum number of loudspeaker sets (triplets or pairs) allowed */
#define MAX_LS_AMOUNT 55			/* maximum amount of loudspeakers, can be increased */

static InterfaceTable *ft;

struct VBAP : Unit
{
	float x_azi;				/* panning direction azimuth */
	float x_ele;				/* panning direction elevation */
	float x_set_inv_matx[MAX_LS_SETS][9];  /* inverse matrice for each loudspeaker set */
	float x_set_matx[MAX_LS_SETS][9];	   /* matrice for each loudspeaker set */
	int x_lsset[MAX_LS_SETS][3];		  /* channel numbers of loudspeakers in each LS set */
	int x_lsset_available;			/* have loudspeaker sets been defined with define_loudspeakers */
	int x_lsset_amount;			/* amount of loudspeaker sets */
	int x_ls_amount;					  /* amount of loudspeakers */
	int x_dimension;					  /* 2 or 3 */
	float x_spread;							/* speading amount of virtual source (0-100) */
	float x_spread_base[3];				   /* used to create uniform spreading */
	float *final_gs;

	float m_chanamp[MAX_LS_AMOUNT]; // for smoothing amp changes max channels 55 at the moment
};

// for circular smoothing

struct CircleRamp : public Unit
{
	double m_level, m_slope;
	int m_counter;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Functions and globals adapted from PD implementation

// Unused:
//static t_class *vbap_class;
//static void *vbap_new(t_symbol *s, int ac, t_atom *av); /* using A_GIMME - typed message list */

// these are input responses:
//void vbap_bang(VBAP *x);
/* static void vbap_int(t_vbap *x, t_float n); */
//void vbap_matrix(VBAP *x, t_symbol *s, int ac, t_atom *av);

static void angle_to_cart(float azi, float ele, float res[3])
/* converts angular coordinates to cartesian */
{
	float atorad = (2 * 3.1415927 / 360) ;
	res[0] = cos((float) azi * atorad) * cos((float) ele * atorad);
	res[1] = sin((float) azi * atorad) * cos((float) ele * atorad);
	res[2] = sin((float) ele * atorad);
}

static void cart_to_angle(float cvec[3], float avec[3])
/* converts cartesian coordinates to angular */
{
	//	float tmp, tmp2, tmp3, tmp4; /* warning: unused variable */
	float atorad = (2 * 3.1415927 / 360) ;
	float pi =	3.1415927;
	//	float power; /* warning: unused variable */
	float dist, atan_y_per_x, atan_x_pl_y_per_z;
	float azi, ele;

	if(cvec[0]==0.0)
		atan_y_per_x = pi / 2;
	else
		atan_y_per_x = atan(cvec[1] / cvec[0]);
	azi = atan_y_per_x / atorad;
	if(cvec[0]<0.0)
		azi +=180;
	dist = sqrt(cvec[0]*cvec[0] + cvec[1]*cvec[1]);
	if(cvec[2]==0.0)
		atan_x_pl_y_per_z = 0.0;
	else
		atan_x_pl_y_per_z = atan(cvec[2] / dist);
	if(dist == 0.0)
	{
		if(cvec[2]<0.0)
			atan_x_pl_y_per_z = -pi/2.0;
		else
			atan_x_pl_y_per_z = pi/2.0;
	}
	ele = atan_x_pl_y_per_z / atorad;
	dist = sqrtf(cvec[0] * cvec[0] +cvec[1] * cvec[1] +cvec[2]*cvec[2]);
	avec[0]=azi;
	avec[1]=ele;
	avec[2]=dist;
}

static void new_spread_dir(VBAP *x, float spreaddir[3], float vscartdir[3], float spread_base[3])
/* subroutine for spreading */
{
	float beta,gamma;
	float a,b;
	float pi = 3.1415927;
	float power;

	gamma = acos(vscartdir[0] * spread_base[0] +
				 vscartdir[1] * spread_base[1] +
				 vscartdir[2] * spread_base[2])/pi*180;
	if(fabs(gamma) < 1){
		angle_to_cart(x->x_azi+90, 0, spread_base);
		gamma = acos(vscartdir[0] * spread_base[0] +
					 vscartdir[1] * spread_base[1] +
					 vscartdir[2] * spread_base[2])/pi*180;
	}
	beta = 180 - gamma;
	b=sin(x->x_spread * pi / 180) / sin(beta * pi / 180);
	a=sin((180- x->x_spread - beta) * pi / 180) / sin (beta * pi / 180);
	spreaddir[0] = a * vscartdir[0] + b * spread_base[0];
	spreaddir[1] = a * vscartdir[1] + b * spread_base[1];
	spreaddir[2] = a * vscartdir[2] + b * spread_base[2];

	power=sqrt(spreaddir[0]*spreaddir[0] + spreaddir[1]*spreaddir[1]
			   + spreaddir[2]*spreaddir[2]);
	spreaddir[0] /= power;
	spreaddir[1] /= power;
	spreaddir[2] /= power;
}

static void new_spread_base(VBAP *x, float spreaddir[3], float vscartdir[3])
/* subroutine for spreading */
{
	float d;
	float pi = 3.1415927;
	float power;

	d = cos(x->x_spread/180*pi);
	x->x_spread_base[0] = spreaddir[0] - d * vscartdir[0];
	x->x_spread_base[1] = spreaddir[1] - d * vscartdir[1];
	x->x_spread_base[2] = spreaddir[2] - d * vscartdir[2];
	power=sqrt(x->x_spread_base[0]*x->x_spread_base[0] + x->x_spread_base[1]*x->x_spread_base[1]
			   + x->x_spread_base[2]*x->x_spread_base[2]);
	x->x_spread_base[0] /= power;
	x->x_spread_base[1] /= power;
	x->x_spread_base[2] /= power;
}

static void cross_prod(float v1[3], float v2[3], float v3[3])
/* vector cross product */
{
	float length;
	v3[0] = (v1[1] * v2[2] ) - (v1[2] * v2[1]);
	v3[1] = (v1[2] * v2[0] ) - (v1[0] * v2[2]);
	v3[2] = (v1[0] * v2[1] ) - (v1[1] * v2[0]);

	length= sqrt(v3[0]*v3[0] + v3[1]*v3[1] + v3[2]*v3[2]);
	v3[0] /= length;
	v3[1] /= length;
	v3[2] /= length;
}

static void additive_vbap(float *final_gs, float cartdir[3], VBAP *x)
/* calculates gains to be added to previous gains, used in
// multiple direction panning (source spreading) */
{
	float power;
	int i,j,k, gains_modified;
	float small_g;
	float big_sm_g, gtmp[3];
	int winner_set;
	//		float new_cartdir[3];	/* warning: unused variable */
	//		float new_angle_dir[3];		/* warning: unused variable */
	int dim = x->x_dimension;
	int neg_g_am, best_neg_g_am;
	float g[3];
	int ls[3] = { 0, 0, 0 };

	big_sm_g = -100000.0;
	best_neg_g_am=3;

	/* BUG: there is a bug that sometimes causes x->x_lsset_amount to be a massive
		* number.  I haven't tracked it down yet, but its probably an init
		* bug. 2006-08-13 <hans@at.or.at>
		*/
	//	post("x_lsset_amount: %li", x->x_lsset_amount);
	for(i=0;i<x->x_lsset_amount;i++){
		small_g = 10000000.0;
		neg_g_am = 3;
		for(j=0;j<dim;j++){
			gtmp[j]=0.0;
			for(k=0;k<dim;k++)
				gtmp[j]+=cartdir[k]* x->x_set_inv_matx[i][k+j*dim];
			if(gtmp[j] < small_g)
				small_g = gtmp[j];
			if(gtmp[j]>= -0.01)
				neg_g_am--;
		}
		if(small_g > big_sm_g && neg_g_am <= best_neg_g_am){
			big_sm_g = small_g;
			best_neg_g_am = neg_g_am;
			winner_set=i;
			g[0]=gtmp[0]; g[1]=gtmp[1];
			ls[0]= x->x_lsset[i][0]; ls[1]= x->x_lsset[i][1];
			if(dim==3){
				g[2]=gtmp[2];
				ls[2]= x->x_lsset[i][2];
			} else {
				g[2]=0.0;
				ls[2]=0;
			}
		}
	}

	gains_modified=0;
	for(i=0;i<dim;i++)
		if(g[i]<-0.01){
			gains_modified=1;
		}

	if(gains_modified != 1){
		if(dim==3)
			power=sqrt(g[0]*g[0] + g[1]*g[1] + g[2]*g[2]);
		else
			power=sqrt(g[0]*g[0] + g[1]*g[1]);
		g[0] /= power;
		g[1] /= power;
		if(dim==3)
			g[2] /= power;

		final_gs[ls[0]-1] += g[0];
		final_gs[ls[1]-1] += g[1];
		/* BUG FIX: this was causing negative indices with 2 dimensions so I
		 * made it only try when using 3 dimensions.
		 * 2006-08-13 <hans@at.or.at> */
		if(dim==3)
			final_gs[ls[2]-1] += g[2];
	}
}

static void spread_it(VBAP *x, float *final_gs)
/*
 // apply the sound signal to multiple panning directions
 // that causes some spreading.
 // See theory in paper V. Pulkki "Uniform spreading of amplitude panned
 // virtual sources" in WASPAA 99
 */
{
	float vscartdir[3];
	float spreaddir[16][3];
	float spreadbase[16][3];
	int i, spreaddirnum;
	float power;
	if(x->x_dimension == 3){
		spreaddirnum=16;
		angle_to_cart(x->x_azi,x->x_ele,vscartdir);
		new_spread_dir(x, spreaddir[0], vscartdir, x->x_spread_base);
		new_spread_base(x, spreaddir[0], vscartdir);
		cross_prod(x->x_spread_base, vscartdir, spreadbase[1]); /* four orthogonal dirs */
		cross_prod(spreadbase[1], vscartdir, spreadbase[2]);
		cross_prod(spreadbase[2], vscartdir, spreadbase[3]);

		/* four between them */
		for(i=0;i<3;i++) spreadbase[4][i] =	 (x->x_spread_base[i] + spreadbase[1][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[5][i] =	 (spreadbase[1][i] + spreadbase[2][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[6][i] =	 (spreadbase[2][i] + spreadbase[3][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[7][i] =	 (spreadbase[3][i] + x->x_spread_base[i]) / 2.0;

		/* four at half spreadangle */
		for(i=0;i<3;i++) spreadbase[8][i] =	 (vscartdir[i] + x->x_spread_base[i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[9][i] =	 (vscartdir[i] + spreadbase[1][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[10][i] =  (vscartdir[i] + spreadbase[2][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[11][i] =  (vscartdir[i] + spreadbase[3][i]) / 2.0;

		/* four at quarter spreadangle */
		for(i=0;i<3;i++) spreadbase[12][i] =  (vscartdir[i] + spreadbase[8][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[13][i] =  (vscartdir[i] + spreadbase[9][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[14][i] =  (vscartdir[i] + spreadbase[10][i]) / 2.0;
		for(i=0;i<3;i++) spreadbase[15][i] =  (vscartdir[i] + spreadbase[11][i]) / 2.0;

		additive_vbap(final_gs,spreaddir[0],x);
		for(i=1;i<spreaddirnum;i++){
			new_spread_dir(x, spreaddir[i], vscartdir, spreadbase[i]);
			additive_vbap(final_gs,spreaddir[i],x);
		}
	} else if (x->x_dimension == 2) {
		spreaddirnum=6;

		angle_to_cart(x->x_azi - x->x_spread, 0, spreaddir[0]);
		angle_to_cart(x->x_azi - x->x_spread/2, 0, spreaddir[1]);
		angle_to_cart(x->x_azi - x->x_spread/4, 0, spreaddir[2]);
		angle_to_cart(x->x_azi + x->x_spread/4, 0, spreaddir[3]);
		angle_to_cart(x->x_azi + x->x_spread/2, 0, spreaddir[4]);
		angle_to_cart(x->x_azi + x->x_spread, 0, spreaddir[5]);

		for(i=0;i<spreaddirnum;i++)
			additive_vbap(final_gs,spreaddir[i],x);
	} else
		return;

	if(x->x_spread > 70)
		for(i=0;i<x->x_ls_amount;i++){
			final_gs[i] += (x->x_spread - 70) / 30.0 * (x->x_spread - 70) / 30.0 * 10.0;
		}

	for(i=0,power=0.0;i<x->x_ls_amount;i++){
		power += final_gs[i] * final_gs[i];
	}

    power = sqrt(power);
	for(i=0;i<x->x_ls_amount;i++){
		final_gs[i] /= power;
	}
}



static void vbap(float g[3], int ls[3], VBAP *x)
{
	/* calculates gain factors using loudspeaker setup and given direction */
	float power;
	int i,j,k, gains_modified;
	float small_g;
	float big_sm_g, gtmp[3];
	int winner_set=0;
	float cartdir[3];
	float new_cartdir[3];
	float new_angle_dir[3];
	int dim = x->x_dimension;
	int neg_g_am, best_neg_g_am;

	/* transfering the azimuth angle to a decent value */
	while(x->x_azi > 180)
		x->x_azi -= 360;
	while(x->x_azi < -179)
		x->x_azi += 360;

	/* transferring the elevation to a decent value */
	if(dim == 3){
		while(x->x_ele > 180)
			x->x_ele -= 360;
		while(x->x_ele < -179)
			x->x_ele += 360;
	} else
		x->x_ele = 0;


	/* go through all defined loudspeaker sets and find the set which
		// has all positive values. If such is not found, set with largest
		// minimum value is chosen. If at least one of gain factors of one LS set is negative
		// it means that the virtual source does not lie in that LS set. */

	angle_to_cart(x->x_azi,x->x_ele,cartdir);
	big_sm_g = -100000.0;	/* initial value for largest minimum gain value */
	best_neg_g_am=3;		  /* how many negative values in this set */


	for(i=0;i<x->x_lsset_amount;i++){
		small_g = 10000000.0;
		neg_g_am = 3;
		for(j=0;j<dim;j++){
			gtmp[j]=0.0;
			for(k=0;k<dim;k++)
				gtmp[j]+=cartdir[k]* x->x_set_inv_matx[i][k+j*dim];
			if(gtmp[j] < small_g)
				small_g = gtmp[j];
			if(gtmp[j]>= -0.01)
				neg_g_am--;
		}
		if(small_g > big_sm_g && neg_g_am <= best_neg_g_am){
			big_sm_g = small_g;
			best_neg_g_am = neg_g_am;
			winner_set=i;
			g[0]=gtmp[0]; g[1]=gtmp[1];
			ls[0]= x->x_lsset[i][0]; ls[1]= x->x_lsset[i][1];
			if(dim==3){
				g[2]=gtmp[2];
				ls[2]= x->x_lsset[i][2];
			} else {
				g[2]=0.0;
				ls[2]=0;
			}
		}
	}

	/* If chosen set produced a negative value, make it zero and
		// calculate direction that corresponds	 to these new
		// gain values. This happens when the virtual source is outside of
		// all loudspeaker sets. */

	if(dim==3){
		gains_modified=0;
		for(i=0;i<dim;i++)
			if(g[i]<-0.01){
				g[i]=0.0001;
				gains_modified=1;
			}
		if(gains_modified==1){
			new_cartdir[0] =  x->x_set_matx[winner_set][0] * g[0]
				+ x->x_set_matx[winner_set][1] * g[1]
				+ x->x_set_matx[winner_set][2] * g[2];
			new_cartdir[1] =  x->x_set_matx[winner_set][3] * g[0]
				+ x->x_set_matx[winner_set][4] * g[1]
				+ x->x_set_matx[winner_set][5] * g[2];
			new_cartdir[2] =  x->x_set_matx[winner_set][6] * g[0]
				+ x->x_set_matx[winner_set][7] * g[1]
				+ x->x_set_matx[winner_set][8] * g[2];
			cart_to_angle(new_cartdir,new_angle_dir);
			x->x_azi = (float) (new_angle_dir[0] + 0.5);
			x->x_ele = (float) (new_angle_dir[1] + 0.5);
		}
	}

		power=sqrt(g[0]*g[0] + g[1]*g[1] + g[2]*g[2]);
		g[0] /= power;
		g[1] /= power;
		g[2] /= power;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

static inline void VBAP_calc_gain_factors(VBAP * unit)
{
	// adapted from vbap_bang
	float azimuth = ZIN0(2);
	float elevation = ZIN0(3);
	float spread = ZIN0(4);

	float *final_gs = unit->final_gs;

	// only recalculate gain factors if inputs have changed
	if((azimuth != unit->x_azi) || (elevation != unit->x_ele) || (spread != unit->x_spread)){

		float g[3];
		int ls[3];
		int i;

		unit->x_azi = azimuth;
		unit->x_ele = elevation;
		unit->x_spread = spread;

		if(unit->x_lsset_available ==1){
			vbap(g,ls, unit);
			for(i=0;i<unit->x_ls_amount;i++)
				final_gs[i]=0.0;
			for(i=0;i<unit->x_dimension;i++){
				final_gs[ls[i]-1]=g[i];
			}
			if(unit->x_spread != 0){
				spread_it(unit,final_gs);
			}
//			for(i=0; i < unit->mNumOutputs; i++){
//				printf("chan %i: %f\n", i, final_gs[i] );
//			}
		}
	}
}

static void VBAP_next(VBAP *unit, int inNumSamples)
{
	VBAP_calc_gain_factors(unit);

	float *zin0 = ZIN(0);
	float *final_gs = unit->final_gs;

	// now scale the outputs
	for (int i=0; i<(unit->mNumOutputs); ++i) {
		float *out = ZOUT(i);
		float chanamp = unit->m_chanamp[i];
		float nextchanamp = final_gs[i];
		if (nextchanamp == chanamp) {
			if (nextchanamp == 0.f) {
				ZClear(inNumSamples, out);
			} else {
				float *in = zin0;
				LOOP1(inNumSamples,
					 ZXP(out) = ZXP(in) * nextchanamp;
					 )
			}
		} else {
			float chanampslope = CALCSLOPE(nextchanamp, chanamp);
			float *in = zin0;
			LOOP1(inNumSamples,
				 ZXP(out) = ZXP(in) * chanamp;
				 chanamp += chanampslope;
				 )
			unit->m_chanamp[i] = nextchanamp;
		}
	}
}

#ifdef NOVA_SIMD
static inline_functions void VBAP_next_simd(VBAP *unit, int inNumSamples)
{
	VBAP_calc_gain_factors(unit);

	float *in = IN(0);
	float *final_gs = unit->final_gs;

	// now scale the outputs
	for (int i=0; i<(unit->mNumOutputs); ++i) {
		float *out = OUT(i);
		float chanamp = unit->m_chanamp[i];
		float nextchanamp = final_gs[i];
		if (nextchanamp == chanamp) {
			if (nextchanamp == 0.f)
				nova::zerovec_simd(out, inNumSamples);
			else
				nova::times_vec_simd(out, in, nextchanamp, inNumSamples);
		} else {
			float chanampslope = CALCSLOPE(nextchanamp, chanamp);
			nova::times_vec_simd(out, in, nextchanamp, inNumSamples);
			unit->m_chanamp[i] = nextchanamp;
		}
	}
}
#endif

static void VBAP_Ctor(VBAP* unit)
{
	int numOutputs = unit->mNumOutputs, counter = 0, datapointer=0, setpointer=0, i;

	// initialise interpolation levels and outputs
	for (int i=0; i<numOutputs; ++i) {
		unit->m_chanamp[i] = 0;
		ZOUT0(i) = 0.f;
	}

	// [dim, numSpeakers, [chanOffsets 0-2, invmx 0-8, [lp1, lp2, lp2].x, sim.y, sim.z] * sets.size].flat
	float fbufnum = ZIN0(1);
	uint32 ibufnum = (uint32)fbufnum;
	World *world = unit->mWorld;

	SndBuf *buf;
	if (ibufnum >= world->mNumSndBufs) {
		int localBufNum = ibufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			buf = world->mSndBufs;
		}
	} else {
		buf = world->mSndBufs + ibufnum;
	}

	int numvals = buf->samples;
	unit->x_dimension = (int)(buf->data[datapointer++]);
	unit->x_ls_amount = (int)(buf->data[datapointer++]);

	unit->x_azi = unit->x_ele = unit->x_spread = std::numeric_limits<float>::quiet_NaN();

	unit->final_gs = (float*)RTAlloc(unit->mWorld, numOutputs * sizeof(float));
	unit->x_lsset_available = 1;

	if(((unit->x_dimension != 2) && (unit->x_dimension != 3)) || (unit->x_ls_amount < 2)) {
		printf("vbap: Error in loudspeaker data. Bufnum: %i\n", (int)fbufnum);
		unit->x_lsset_available = 0;
		// do something else here
	}

	if(unit->x_dimension == 3)
		counter = (numvals - 2) / ((unit->x_dimension * unit->x_dimension*2) + unit->x_dimension);
	if(unit->x_dimension == 2)
		counter = (numvals - 2) / ((unit->x_dimension * unit->x_dimension) + unit->x_dimension);
	unit->x_lsset_amount=counter;

	if(counter<=0){
		printf("vbap: Error in loudspeaker data. Bufnum: %i\n", (int)fbufnum);
		unit->x_lsset_available=0;
//		return;
	}

	// probably sets should be created with rtalloc
	while(counter-- > 0){
		for(i=0; i < unit->x_dimension; i++){
			unit->x_lsset[setpointer][i]=(int)buf->data[datapointer++];
		}

		for(i=0; i < unit->x_dimension*unit->x_dimension; i++){
			unit->x_set_inv_matx[setpointer][i]=buf->data[datapointer++];
				/*				post("%d",deb++); */
		}
		if(unit->x_dimension == 3){
			for(i=0; i < unit->x_dimension*unit->x_dimension; i++){
				unit->x_set_matx[setpointer][i]=buf->data[datapointer++];

			}
		}

		setpointer++;
	}
	//printf("vbap: Loudspeaker setup configured!\n");

#ifdef NOVA_SIMD
	if (!(BUFLENGTH & 15))
		SETCALC(VBAP_next_simd);
	else
#endif
		SETCALC(VBAP_next);

    if (unit->x_lsset_available == 1) {
        unit->x_spread_base[0] = 0.0;
        unit->x_spread_base[1] = 1.0;
        unit->x_spread_base[2] = 0.0;
        VBAP_next(unit, 1); // calculate initial gain factors && compute initial sample
    } else {
        ZOUT0(0) = 0;
		// if the ls data was bad, just set every gain to 0 and bail
		for(i=0;i<unit->x_ls_amount;i++)
			unit->final_gs[i]=0.f;
    }
}


static void VBAP_Dtor(VBAP* unit)
{
	RTFree(unit->mWorld, unit->final_gs);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// for circular smoothing of input signals
static void CircleRamp_next(CircleRamp *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = IN(0);
	float period = ZIN0(1);
	double circmin = (double)ZIN0(2);
	double circmax = (double)ZIN0(3);

	double circrange = circmax - circmin;
	double slope = unit->m_slope;
	double level = unit->m_level;
	int counter = unit->m_counter;
	int remain = inNumSamples;
	while (remain) {
		int nsmps = sc_min(remain, counter);
		LOOP1(nsmps,
			 ZXP(out) = level;
			 level = sc_wrap(level + slope, circmin, circmax);
			 );
		in += nsmps;
		counter -= nsmps;
		remain -= nsmps;
		if (counter <= 0) {
			counter = (int)(period * SAMPLERATE);
			counter = sc_max(1, counter);
			double diff = sc_wrap((double)(*in), circmin, circmax) - level;
			// go the shortest way around...
			if(fabs(diff) > (circrange * 0.5)){
				int invsign = diff < 0.0 ? 1 : -1;
				diff = (circrange - fabs(diff)) * invsign;
			}
			slope = diff / counter;
		}
	}
	unit->m_level = level;
	unit->m_slope = slope;
	unit->m_counter = counter;

}

static void CircleRamp_next_1(CircleRamp *unit, int inNumSamples)
{
	float *out = OUT(0);

	double circmin = (double)ZIN0(2);
	double circmax = (double)ZIN0(3);
	double circrange = circmax - circmin;

	*out = unit->m_level;
	unit->m_level = sc_wrap(unit->m_level + unit->m_slope, circmin, circmax);
	if (--unit->m_counter <= 0) {
		float in = ZIN0(0);
		float period = ZIN0(1);
		int counter = (int)(period * SAMPLERATE);
		unit->m_counter = counter = sc_max(1, counter);

		double diff = sc_wrap((double)in, circmin, circmax) - unit->m_level;
		// go the shortest way around...
		if(fabs(diff) > (circrange * 0.5)){
			int invsign = diff < 0.0 ? 1 : -1;
			diff = (circrange - fabs(diff)) * invsign;
		}
		unit->m_slope = diff / counter;
	}

}

static void CircleRamp_Ctor(CircleRamp* unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(CircleRamp_next_1);
		//printf("next1\n");
	} else {
		SETCALC(CircleRamp_next);
		//printf("next\n");
	}

	unit->m_counter = 1;
	unit->m_level = sc_wrap(ZIN0(0), ZIN0(2), ZIN0(3));
	unit->m_slope = 0.f;
	ZOUT0(0) = unit->m_level;

}


//////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(VBAP)
{
	ft = inTable;
	DefineDtorCantAliasUnit(VBAP);
	DefineSimpleUnit(CircleRamp);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
