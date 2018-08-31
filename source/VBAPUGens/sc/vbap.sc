/*
VBAP created by Ville Pukki
This version ported from ver 0.99 PD code by Scott Wilson
Development funded in part by the AHRC http://www.ahrc.ac.uk

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


VBAPSpeakerArray {
	classvar <>maxNumSpeakers = 55, minSideLength = 0.01;
	var <dim, <speakers, <numSpeakers, sets;

	*new { |dim, directions|
		^super.newCopyArgs(dim).initFromDirections(directions);
	}

	initFromDirections { |directions|
		if(dim == 2, {directions = directions.collect({|azi| [azi, 0]});});
		numSpeakers = directions.size;
		speakers = directions.collect({|dir| VBAPSpeaker.new(dir[0], dir[1]) });
		this.anglesToCartesian;
	}

	anglesToCartesian {
		var atorad = (2 * 3.1415927 / 360);
		speakers.do({ |spkr|
			var azi, ele;
			azi = spkr.azi;
			ele = spkr.ele;
			spkr.x = cos(azi * atorad) * cos(ele * atorad);
			spkr.y = sin(azi * atorad) * cos(ele * atorad);
			spkr.z = sin(ele * atorad);
		});
	}

	getSetsAndMatrices {
		/* calculate and print out chosen loudspeaker sets and corresponding  matrices */

		if(dim == 3, {
			this.choose_ls_triplets;
			^this.calculate_3x3_matrixes;
		}, {
			if(dim == 2, {^this.choose_ls_tuplets});
		});
		postln("Error in loudspeaker direction data");
		^nil;
	}

	loadToBuffer {|server|
		^Buffer.loadCollection(server, this.getSetsAndMatrices);
	}


	     /* Selects the loudspeaker triplets, and
      calculates the inversion matrices for each selected triplet.
     A line (connection) is drawn between each loudspeaker. The lines
     denote the sides of the triangles. The triangles should not be
     intersecting. All crossing connections are searched and the
     longer connection is erased. This yields non-intesecting triangles,
     which can be used in panning.
     See theory in paper Pulkki, V. Lokki, T. "Creating Auditory Displays
     with Multiple Loudspeakers Using VBAP: A Case Study with
     DIVA Project" in International Conference on
     Auditory Displays -98.*/

	choose_ls_triplets {
		var i1, j1, k1, m, li, table_size;
		var vb1,vb2,tmp_vec; // instances of VBAPSpeaker
		var connections;
		var angles;
		var sorted_angles;
		var distance_table;
		var distance_table_i;
		var distance_table_j;
		var distance;

		// init vars defined above
		connections = Array.fill(maxNumSpeakers, {Array.newClear(maxNumSpeakers)});
		angles = Array.newClear(maxNumSpeakers);
		sorted_angles = Array.newClear(maxNumSpeakers);
		distance_table = Array.newClear((maxNumSpeakers * (maxNumSpeakers - 1)) / 2);
		distance_table_i = Array.newClear((maxNumSpeakers * (maxNumSpeakers - 1)) / 2);
		distance_table_j = Array.newClear((maxNumSpeakers * (maxNumSpeakers - 1)) / 2);

		sets = nil;
		for(0.0, numSpeakers -1, {|i|
			for(i+1.0, numSpeakers -1, {|j|
				for(j+1.0, numSpeakers -1, {|k|
					if(this.vol_p_side_lgth(i,j,k) > minSideLength, {
						connections[i][j]=1;
						connections[j][i]=1;
						connections[i][k]=1;
						connections[k][i]=1;
						connections[j][k]=1;
						connections[k][j]=1;
						//"i: % j: %, k: %\n".postf(i, j, k);
						sets = sets.add(VBAPSpeakerSet([i,j,k]));
					});
				});
			});
		});

		/*calculate distancies between all lss and sorting them*/
		table_size = ((numSpeakers - 1) * (numSpeakers)) / 2;
		for(0, table_size -1, { |i| distance_table[i] = 100000.0 });
		for(0.0, numSpeakers - 1, { |i|
			for(i+1, numSpeakers - 1, {|j|
				var k;
				if(connections[i][j] == 1, {
					distance = abs(this.vec_angle(speakers[i],speakers[j]));
					k=0;
					while({distance_table[k] < distance}, {k = k+1});
					forBy(table_size - 1, k + 1, -1,{ |l|
						distance_table[l] = distance_table[l-1];
						distance_table_i[l] = distance_table_i[l-1];
						distance_table_j[l] = distance_table_j[l-1];
					});
					distance_table[k] = distance;
					distance_table_i[k] = i;
					distance_table_j[k] = j;
			}, {table_size = table_size - 1;});
			});
		});

	  /* disconnecting connections which are crossing shorter ones,
	     starting from shortest one and removing all that cross it,
	     and proceeding to next shortest */
		for(0.0, table_size - 1, {|i|
			var fst_ls, sec_ls;
			fst_ls = distance_table_i[i];
			sec_ls = distance_table_j[i];
			if(connections[fst_ls][sec_ls] == 1, {
				for(0.0, numSpeakers - 1, {|j|
					for(j+1.0, numSpeakers - 1, {|k|
						if( (j!=fst_ls) && (k != sec_ls) && (k!=fst_ls) && (j != sec_ls), {
							if(this.lines_intersect(fst_ls, sec_ls, j,k), {
								connections[j][k] = 0;
								connections[k][j] = 0;
							});
						});
					});
				});
			});
		});

	  /* remove triangles which had crossing sides
	     with smaller triangles or include loudspeakers*/
		//"triplet_amount before stripping: %\n".postf(sets.size);
		sets = sets.reject({|set|
			i1 = set.chanOffsets[0];
			j1 = set.chanOffsets[1];
			k1 = set.chanOffsets[2];
			(connections[i1][j1] == 0) || (connections[i1][k1] == 0) || (connections[j1][k1] == 0)
				|| this.any_ls_inside_triplet(i1,j1,k1);
		});
		//"triplet_amount after stripping: %\n".postf(sets.size);
	}

	lines_intersect { |i, j, k, l|
	     /* checks if two lines intersect on 3D sphere
	       */
		var v1, v2, v3, neg_v3; // VBAPSpeaker
		var angle;
		var dist_ij,dist_kl,dist_iv3,dist_jv3,dist_inv3,dist_jnv3;
		var dist_kv3,dist_lv3,dist_knv3,dist_lnv3;

		v1 = this.unq_cross_prod(speakers[i], speakers[j]);
		v2 = this.unq_cross_prod(speakers[k], speakers[l]);
		v3 = this.unq_cross_prod(v1, v2);

		neg_v3 = VBAPSpeaker.new;
		neg_v3.x= 0.0 - v3.x;
		neg_v3.y= 0.0 - v3.y;
		neg_v3.z= 0.0 - v3.z;

		dist_ij = (this.vec_angle(speakers[i], speakers[j]));
		dist_kl = (this.vec_angle(speakers[k], speakers[l]));
		dist_iv3 = (this.vec_angle(speakers[i], v3));
		dist_jv3 = (this.vec_angle(v3, speakers[j]));
		dist_inv3 = (this.vec_angle(speakers[i], neg_v3));
		dist_jnv3 = (this.vec_angle(neg_v3, speakers[j]));
		dist_kv3 = (this.vec_angle(speakers[k], v3));
		dist_lv3 = (this.vec_angle(v3, speakers[l]));
		dist_knv3 = (this.vec_angle(speakers[k], neg_v3));
		dist_lnv3 = (this.vec_angle(neg_v3, speakers[l]));

		/* if one of loudspeakers is close to crossing point, don't do anything*/
		if((abs(dist_iv3) <= 0.01) || (abs(dist_jv3) <= 0.01) ||
			(abs(dist_kv3) <= 0.01) || (abs(dist_lv3) <= 0.01) ||
			(abs(dist_inv3) <= 0.01) || (abs(dist_jnv3) <= 0.01) ||
			(abs(dist_knv3) <= 0.01) || (abs(dist_lnv3) <= 0.01), {^false});

		/* if crossing point is on line between both loudspeakers return 1 */
		if (((abs(dist_ij - (dist_iv3 + dist_jv3)) <= 0.01 ) &&
	       (abs(dist_kl - (dist_kv3 + dist_lv3))  <= 0.01)) ||
	      ((abs(dist_ij - (dist_inv3 + dist_jnv3)) <= 0.01)  &&
	       (abs(dist_kl - (dist_knv3 + dist_lnv3)) <= 0.01 )), { ^true}, {^false});

	}



  /* calculate volume of the parallelepiped defined by the loudspeaker
     direction vectors and divide it with total length of the triangle sides.
     This is used when removing too narrow triangles. */
	vol_p_side_lgth { |i, j, k|
		var volper, lgth;
		var xprod;

		xprod = this.unq_cross_prod(speakers[i], speakers[j]);
		volper = abs(this.vec_prod(xprod, speakers[k]));
		lgth = (abs(this.vec_angle(speakers[i], speakers[j]))
		+ abs(this.vec_angle(speakers[i], speakers[k]))
		+ abs(this.vec_angle(speakers[j], speakers[k])));
		if(lgth > 0.00001, { ^(volper / lgth)}, { ^0.0 });
	}

	//unq_cross_prod(t_ls v1,t_ls v2, t_ls *res)
	/* vector cross product */
	unq_cross_prod { |v1, v2|
		var length, result;
		result = VBAPSpeaker.new;
		result.x = (v1.y * v2.z ) - (v1.z * v2.y);
		result.y = (v1.z * v2.x ) - (v1.x * v2.z);
		result.z = (v1.x * v2.y ) - (v1.y * v2.x);
		length = this.vec_length(result);
		result.x = result.x / length;
		result.y = result.y / length;
		result.z = result.z / length;
		^result;
	}

	vec_length { |v1|
		/* length of a vector */
		^(sqrt(v1.x.squared + v1.y.squared + v1.z.squared));
	}

	vec_prod {|v1, v2|
		/* vector dot product */
		^((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z));
	}

	vec_angle{ |v1, v2|
		/* angle between two loudspeakers */
		var inner;
		inner = ((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z)) /
			(this.vec_length(v1) * this.vec_length(v2));
		if(inner > 1.0, {inner = 1.0});
		if (inner < -1.0, {inner = -1.0});
		^abs(acos(inner));
	}

	any_ls_inside_triplet { |a, b, c| // speakers, numSpeakers
		/* returns true if there is loudspeaker(s) inside given ls triplet */
		var invdet;
		var lp1, lp2, lp3;
		var invmx;
		var tmp;
		var any_ls_inside, this_inside;

		lp1 =  speakers[a];
		lp2 =  speakers[b];
		lp3 =  speakers[c];

		invmx = Array.newClear(9);

		/* matrix inversion */
		invdet = 1.0 / (  lp1.x * ((lp2.y * lp3.z) - (lp2.z * lp3.y))
                    - (lp1.y * ((lp2.x * lp3.z) - (lp2.z * lp3.x)))
                    + (lp1.z * ((lp2.x * lp3.y) - (lp2.y * lp3.x))));

		invmx[0] = ((lp2.y * lp3.z) - (lp2.z * lp3.y)) * invdet;
		invmx[3] = ((lp1.y * lp3.z) - (lp1.z * lp3.y)) * invdet.neg;
		invmx[6] = ((lp1.y * lp2.z) - (lp1.z * lp2.y)) * invdet;
		invmx[1] = ((lp2.x * lp3.z) - (lp2.z * lp3.x)) * invdet.neg;
		invmx[4] = ((lp1.x * lp3.z) - (lp1.z * lp3.x)) * invdet;
		invmx[7] = ((lp1.x * lp2.z) - (lp1.z * lp2.x)) * invdet.neg;
		invmx[2] = ((lp2.x * lp3.y) - (lp2.y * lp3.x)) * invdet;
		invmx[5] = ((lp1.x * lp3.y) - (lp1.y * lp3.x)) * invdet.neg;
		invmx[8] = ((lp1.x * lp2.y) - (lp1.y * lp2.x)) * invdet;

		any_ls_inside = false;
		for(0, numSpeakers - 1, {|i|
			if((i != a) && (i != b) && (i != c), {
				this_inside = true;
				for(0, 2, {|j|
					tmp = speakers[i].x * invmx[0 + (j*3)];
					tmp = speakers[i].y * invmx[1 + (j*3)] + tmp;
					tmp = speakers[i].z * invmx[2 + (j*3)] + tmp;
					if(tmp < -0.001, {this_inside = false;});
				});
			if(this_inside, {any_ls_inside = true});
			});
		});
	  ^any_ls_inside;
	}

	calculate_3x3_matrixes {
     /* Calculates the inverse matrices for 3D */

		var invdet;
		var lp1, lp2, lp3;
		var invmx;
		var triplet_amount = 0, pointer,list_length=0;
		var result;

		if(sets.isNil, {
			postln("define-loudspeakers: Not valid 3-D configuration");
			^nil;
		});

		triplet_amount = sets.size;
		//"triplet_amount: %\n".postf(triplet_amount);
		list_length = triplet_amount * 21 + 2;
		result = FloatArray.newClear(list_length);

		result[0] = dim;
		result[1] = numSpeakers;
		pointer=2;

		sets.do({|set|
			lp1 = speakers[set.chanOffsets[0]];
			lp2 = speakers[set.chanOffsets[1]];
			lp3 = speakers[set.chanOffsets[2]];

			invmx = FloatArray.newClear(9);

			//"lp1x: % lp1y: % lp1z: %\n".postf(lp1.x, lp1.y, lp1.z);
			//"lp2x: % lp2y: % lp2z: %\n".postf(lp2.x, lp2.y, lp2.z);
			//"lp3x: % lp3y: % lp3z: %\n".postf(lp3.x, lp3.y, lp3.z);
			invdet = 1.0 / (  (lp1.x * ((lp2.y * lp3.z) - (lp2.z * lp3.y)))
                    - (lp1.y * ((lp2.x * lp3.z) - (lp2.z * lp3.x)))
                    + (lp1.z * ((lp2.x * lp3.y) - (lp2.y * lp3.x))));

              //"invdet: %\n".postf(invdet);

			invmx[0] = ((lp2.y * lp3.z) - (lp2.z * lp3.y)) * invdet;
			invmx[3] = ((lp1.y * lp3.z) - (lp1.z * lp3.y)) * invdet.neg;
			invmx[6] = ((lp1.y * lp2.z) - (lp1.z * lp2.y)) * invdet;
			invmx[1] = ((lp2.x * lp3.z) - (lp2.z * lp3.x)) * invdet.neg;
			invmx[4] = ((lp1.x * lp3.z) - (lp1.z * lp3.x)) * invdet;
			invmx[7] = ((lp1.x * lp2.z) - (lp1.z * lp2.x)) * invdet.neg;
			invmx[2] = ((lp2.x * lp3.y) - (lp2.y * lp3.x)) * invdet;
			invmx[5] = ((lp1.x * lp3.y) - (lp1.y * lp3.x)) * invdet.neg;
			invmx[8] = ((lp1.x * lp2.y) - (lp1.y * lp2.x)) * invdet;
			set.inv_mx = invmx;
			3.do({|i|
				result[pointer] = set.chanOffsets[i] + 1;
				pointer = pointer + 1;
			});
			9.do({|i|
				result[pointer] = invmx[i];
				pointer = pointer + 1;
			});
			result[pointer] = lp1.x; pointer = pointer + 1;
			result[pointer] = lp2.x; pointer = pointer + 1;
			result[pointer] = lp3.x; pointer = pointer + 1;
			result[pointer] = lp1.y; pointer = pointer + 1;
			result[pointer] = lp2.y; pointer = pointer + 1;
			result[pointer] = lp3.y; pointer = pointer + 1;
			result[pointer] = lp1.z; pointer = pointer + 1;
			result[pointer] = lp2.z; pointer = pointer + 1;
			result[pointer] = lp3.z; pointer = pointer + 1;

		});
		^result;
	}

	choose_ls_tuplets {
     /* selects the loudspeaker pairs, calculates the inversion
        matrices and stores the data to a global array*/
		var atorad = (2 * 3.1415927 / 360);
		var w1,w2;
		var p1,p2;
		var sorted_lss;
		var exist;
		var amount=0;
		var inv_mat;
		var ls_table;
		var list_length;
		var result;
		var pointer;

		exist = Array.newClear(maxNumSpeakers);
		inv_mat = Array.fill(maxNumSpeakers, {Array.newClear(4)});

		for(0, maxNumSpeakers - 1, {|i|
			exist[i]=0;
		});

		/* sort loudspeakers according their azimuth angle */
		sorted_lss = this.sort_2D_lss;

		/* adjacent loudspeakers are the loudspeaker pairs to be used.*/
		for(0, numSpeakers -2, {|i|
			if((speakers[sorted_lss[i+1]].azi - speakers[sorted_lss[i]].azi) <= (180 - 10), {
				if(this.calc_2D_inv_tmatrix(speakers[sorted_lss[i]].azi,
					speakers[sorted_lss[i+1]].azi, inv_mat[i]),{
					exist[i]=1;
					amount = amount + 1;
				});
			});
		});

		if(((6.283 - speakers[sorted_lss[numSpeakers-1]].azi)
			+ speakers[sorted_lss[0]].azi) <= (180 -  10), {
			if(this.calc_2D_inv_tmatrix(speakers[sorted_lss[numSpeakers-1]].azi,
                           speakers[sorted_lss[0]].azi,
                           inv_mat[numSpeakers-1]), {
				exist[numSpeakers-1]=1;
				amount = amount + 1;
			});
		});

		/* Output */
		list_length= amount * 6 + 2;
		result = Array.newClear(list_length);

		result[0] = dim;
		result[1] = numSpeakers;
		pointer=2;

		for(0, numSpeakers - 2, {|i|
			if(exist[i] == 1, {
				result[pointer] = sorted_lss[i]+1;
				pointer = pointer + 1;
				result[pointer] = sorted_lss[i+1]+1;
				pointer = pointer + 1;
				for(0, 3, {|j|
					result[pointer] = inv_mat[i][j];
					pointer = pointer + 1;
				});
			});
		});
		if(exist[numSpeakers-1] == 1, {
			result[pointer] = sorted_lss[numSpeakers-1]+1;
			pointer = pointer + 1;
			result[pointer] = sorted_lss[0]+1;
			pointer = pointer + 1;
			for(0, 3, {|j|
				result[pointer] = inv_mat[numSpeakers-1][j];
				pointer = pointer + 1;
			});
		});
		^result;
	}

	sort_2D_lss {
		/* sort loudspeakers according to azimuth angle */

		var i,j,index;
		var tmp, tmp_azi;
		var rad2ang = 360.0 / ( 2 * pi );

		var x,y;
		var sorted_lss;

		sorted_lss = Array.newClear(maxNumSpeakers);

		/* Transforming angles between -180 and 180 */
		for (0, numSpeakers - 1, {|i|
			speakers[i].azi = acos( speakers[i].x) * rad2ang;
			if (abs(speakers[i].y) <= 0.001, {
				tmp = 1.0;
			}, {
				tmp = speakers[i].y / abs(speakers[i].y);
			});
			speakers[i].azi = speakers[i].azi * tmp;
		});
		for (0, numSpeakers - 1, {|i|
			tmp = 2000;
			for (0, numSpeakers - 1, {|j|
				if (speakers[j].azi <= tmp, {
					tmp = speakers[j].azi;
					index = j;
				});
			});
			sorted_lss[i]=index;
			tmp_azi = (speakers[index].azi);
			speakers[index].azi = (tmp_azi + 4000.0);
		});
		for (0, numSpeakers - 1, {|i|
			tmp_azi = (speakers[i].azi);
			speakers[i].azi = (tmp_azi - 4000.0);
		});
		^sorted_lss;
	}

	calc_2D_inv_tmatrix { |azi1, azi2, inv_mat|
	/* calculate inverse 2x2 matrix */

		var x1,x2,x3,x4;
		var y1,y2,y3,y4;
		var det;
		var rad2ang = 360.0 / ( 2  * 3.141592 );

		x1 = cos(azi1 / rad2ang);
		x2 = sin(azi1 / rad2ang);
		x3 = cos(azi2 / rad2ang);
		x4 = sin(azi2 / rad2ang);
		det = (x1 * x4) - ( x3 * x2 );
		if(abs(det) <= 0.001, {
			inv_mat[0] = 0.0;
			inv_mat[1] = 0.0;
			inv_mat[2] = 0.0;
			inv_mat[3] = 0.0;
			^false;
		}, {
			inv_mat[0] =   (x4 / det);
			inv_mat[1] =  (x3.neg / det);
			inv_mat[2] =   (x2.neg / det);
			inv_mat[3] =    (x1 / det);
			^true;
		})
	}


}

VBAPSpeaker {

	// setters are 'private'

	var <>x, <>y, <>z;

	// spherical coords, angles (in degrees) from a central point
	var <>azi; 	// from median plane +/- 180 deg
	var <>ele; 	// above azimuthal plane

	*new {|azi, ele|
		^super.new.init(azi, ele);
	}

	init{|azimuth, elevation, radius|
		azi = azimuth;
		ele = elevation;
	}
}

VBAPSpeakerSet { // triplet or pair
	var <chanOffsets;
	var <>inv_mx;

	*new {|chanOffsets|
		^super.newCopyArgs(chanOffsets);
	}
}

VBAP : MultiOutUGen {
	// spread 0 - 100
	*ar { arg numChans, in, bufnum, azimuth = 0.0, elevation = 1.0, spread = 0.0;
		^this.multiNew('audio', numChans, in, bufnum, azimuth, elevation, spread )
	}
	*kr { arg numChans, in, bufnum, azimuth = 0.0, elevation = 1.0, spread = 0.0;
		^this.multiNew('control', numChans, in, bufnum, azimuth, elevation, spread )
	}
	init { arg numChans ... theInputs;
		inputs = theInputs;
		channels = Array.fill(numChans, { arg i; OutputProxy(rate,this, i) });
		^channels
	}
}
