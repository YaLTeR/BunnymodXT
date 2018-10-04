/*
Copyright (C) 2018 Matherunner
Copyright (C) 2016 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "draw_hulls.hpp"
#include "hlsdk/common/const.h"
#include <GL/gl.h>

namespace DrawHulls
{
	struct rgb {
		double r;       // a fraction between 0 and 1
		double g;       // a fraction between 0 and 1
		double b;       // a fraction between 0 and 1
	};

	struct hsv {
		double h;       // angle in degrees
		double s;       // a fraction between 0 and 1
		double v;       // a fraction between 0 and 1
	};

	struct hullnode_t
	{
		hullnode_t *next;
		hullnode_t *prev;
	};

	struct hull_model_t
	{
		hullnode_t polys;
		unsigned int num_polys;
	};

	struct winding_t
	{
		const mplane_t *plane;
		winding_t *pair;
		hullnode_t chain;
		int numpoints;
		vec3_t p[4]; // variable sized
	};

	inline void Host_Error(const char *format, ...)
	{
		// TODO: print message
		abort();
	}

	// Definitions taken from engine/common/mathlib.h from Xash3D
	#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
	#define CrossProduct(a,b,c) ((c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1],(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2],(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0])
	#define VectorSubtract(a,b,c) ((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
	#define VectorAdd(a,b,c) ((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
	#define VectorCopy(a,b) ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
	#define VectorScale(in, scale, out) ((out)[0] = (in)[0] * (scale),(out)[1] = (in)[1] * (scale),(out)[2] = (in)[2] * (scale))
	#define VectorClear(x) ((x)[0]=(x)[1]=(x)[2]=0)
	#define VectorNormalize( v ) { float ilength = (float)sqrt(DotProduct(v, v));if (ilength) ilength = 1.0f / ilength;v[0] *= ilength;v[1] *= ilength;v[2] *= ilength; }
	#define VectorMA(a, scale, b, c) ((c)[0] = (a)[0] + (scale) * (b)[0],(c)[1] = (a)[1] + (scale) * (b)[1],(c)[2] = (a)[2] + (scale) * (b)[2])

	#define PLANE_X		0	// 0 - 2 are axial planes
	#define PLANE_Y		1	// 3 needs alternate calc
	#define PLANE_Z		2

	#define SIDE_FRONT   0
	#define SIDE_BACK    1
	#define SIDE_ON      2
	#define SIDE_CROSS  -2

	#define BOGUS_RANGE ((vec_t)114032.64) // world.size * 1.74

	constexpr unsigned int MAX_CLIPNODE_DEPTH = 256; // should never exceeds

	#define list_entry( ptr, type, member ) \
		((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

	// iterate over each entry in the list
	#define list_for_each_entry( pos, head, member )			\
		for( pos = list_entry( (head)->next, winding_t, member );	\
			&pos->member != (head);				\
			pos = list_entry( pos->member.next, winding_t, member ))

	// iterate over the list, safe for removal of entries
	#define list_for_each_entry_safe( pos, n, head, member )		\
		for( pos = list_entry( (head)->next, winding_t, member ),	\
			n = list_entry( pos->member.next, winding_t, member );	\
			&pos->member != (head);				\
			pos = n, n = list_entry( n->member.next, winding_t, member ))

	#define LIST_HEAD_INIT( name ) { &(name), &(name) }

	inline void list_add__( hullnode_t *neww, hullnode_t *prev, hullnode_t *next )
	{
		next->prev = neww;
		neww->next = next;
		neww->prev = prev;
		prev->next = neww;
	}

	// add the new entry after the give list entry
	inline void list_add( hullnode_t *newobj, hullnode_t *head )
	{
		list_add__( newobj, head, head->next );
	}

	// add the new entry before the given list entry (list is circular)
	inline void list_add_tail( hullnode_t *newobj, hullnode_t *head )
	{
		list_add__( newobj, head->prev, head );
	}

	inline void list_del( hullnode_t *entry )
	{
		entry->next->prev = entry->prev;
		entry->prev->next = entry->next;
	}

	static winding_t *winding_alloc( unsigned int numpoints )
	{
		// return (winding_t *)malloc( (int)((winding_t *)0)->p[numpoints] );
		return (winding_t *)malloc(sizeof(winding_t) + numpoints * sizeof(vec3_t));
	}

	static void free_winding( winding_t *w )
	{
		// simple sentinel by Carmack
		if( *(unsigned *)w == 0xDEADC0DE )
			Host_Error( "free_winding: freed a freed winding\n" );
		*(unsigned *)w = 0xDEADC0DE;
		free( w );
	}

	static winding_t *winding_copy( winding_t *w )
	{
		winding_t	*neww;

		neww = winding_alloc( w->numpoints );
		memcpy(neww, w, sizeof(winding_t) + w->numpoints * sizeof(vec3_t));

		return neww;
	}

	static void winding_reverse( winding_t *w )
	{
		vec3_t	point;
		int	i;

		for( i = 0; i < w->numpoints / 2; i++ )
		{
			VectorCopy( w->p[i], point );
			VectorCopy( w->p[w->numpoints - i - 1], w->p[i] );
			VectorCopy( point, w->p[w->numpoints - i - 1] );
		}
	}

	/*
	* winding_shrink
	*
	* Takes an over-allocated winding and allocates a new winding with just the
	* required number of points. The input winding is freed.
	*/
	static winding_t *winding_shrink( winding_t *w )
	{
		winding_t	*neww = winding_alloc( w->numpoints );
		const int copysize = sizeof(winding_t) + w->numpoints * sizeof(vec3_t);
		if (copysize > 0)
			memcpy(neww, w, copysize);
		free_winding( w );

		return neww;
	}

	/*
	====================
	winding_for_plane
	====================
	*/
	static winding_t *winding_for_plane( const mplane_t *p )
	{
		vec3_t	org, vright, vup;
		int	i, axis;
		vec_t	max, v;
		winding_t	*w;

		// find the major axis
		max = -BOGUS_RANGE;
		axis = -1;

		for( i = 0; i < 3; i++ )
		{
			v = fabs( p->normal[i] );
			if( v > max )
			{
				axis = i;
				max = v;
			}
		}

		VectorClear( vup );
		switch( axis )
		{
		case 0:
		case 1:
			vup[2] = 1;
			break;
		case 2:
			vup[0] = 1;
			break;
		default:
			Host_Error( "BaseWindingForPlane: no axis found\n" );
			return NULL;
		}

		v = DotProduct( vup, p->normal );
		VectorMA( vup, -v, p->normal, vup );
		VectorNormalize( vup );
		VectorScale( p->normal, p->dist, org );
		CrossProduct( vup, p->normal, vright );
		VectorScale( vup, BOGUS_RANGE, vup );
		VectorScale( vright, BOGUS_RANGE, vright );

		// project a really big axis aligned box onto the plane
		w = winding_alloc( 4 );
		memset( w->p, 0, sizeof( vec3_t ) * 4 );
		w->numpoints = 4;
		w->plane = p;

		VectorSubtract( org, vright, w->p[0] );
		VectorAdd( w->p[0], vup, w->p[0] );
		VectorAdd( org, vright, w->p[1] );
		VectorAdd( w->p[1], vup, w->p[1] );
		VectorAdd( org, vright, w->p[2] );
		VectorSubtract( w->p[2], vup, w->p[2] );
		VectorSubtract( org, vright, w->p[3] );
		VectorSubtract( w->p[3], vup, w->p[3] );

		return w;
	}

	/*
	* ===========================
	* Helper for for the clipping functions
	*  (winding_clip, winding_split)
	* ===========================
	*/
	static void CalcSides( const winding_t *in, const mplane_t *split, int *sides, vec_t *dists, int counts[3], vec_t epsilon )
	{
		const vec_t	*p;
		int		i;

		counts[0] = counts[1] = counts[2] = 0;

		switch( split->type )
		{
		case PLANE_X:
		case PLANE_Y:
		case PLANE_Z:
			p = in->p[0] + split->type;
			for( i = 0; i < in->numpoints; i++, p += 3 )
			{
				const vec_t dot = *p - split->dist;

				dists[i] = dot;
				if( dot > epsilon )
					sides[i] = SIDE_FRONT;
				else if( dot < -epsilon )
					sides[i] = SIDE_BACK;
				else sides[i] = SIDE_ON;
				counts[sides[i]]++;
			}
			break;
		default:
			p = in->p[0];
			for( i = 0; i < in->numpoints; i++, p += 3 )
			{
				const vec_t dot = DotProduct( split->normal, p ) - split->dist;

				dists[i] = dot;
				if( dot > epsilon )
					sides[i] = SIDE_FRONT;
				else if( dot < -epsilon )
					sides[i] = SIDE_BACK;
				else sides[i] = SIDE_ON;
				counts[sides[i]]++;
			}
			break;
		}

		sides[i] = sides[0];
		dists[i] = dists[0];
	}

	static void PushToPlaneAxis( vec_t *v, const mplane_t *p )
	{
		const int	t = p->type % 3;

		v[t] = (p->dist - p->normal[(t + 1) % 3] * v[(t + 1) % 3] - p->normal[(t + 2) % 3] * v[(t + 2) % 3]) / p->normal[t];
	}

	/*
	==================
	winding_clip

	Clips the winding to the plane, returning the new winding on 'side'.
	Frees the input winding.
	If keepon is true, an exactly on-plane winding will be saved, otherwise
	it will be clipped away.
	==================
	*/
	static winding_t *winding_clip( winding_t *in, const mplane_t *split, qboolean keepon, int side, vec_t epsilon )
	{
		vec_t	*dists;
		int	*sides;
		int	counts[3];
		vec_t	dot;
		int	i, j;
		winding_t *neww;
		vec_t	*p1, *p2, *mid;
		int	maxpts;

		dists = (vec_t *)malloc(( in->numpoints + 1 ) * sizeof( vec_t ));
		sides = (int *)malloc(( in->numpoints + 1 ) * sizeof( int ));
		CalcSides( in, split, sides, dists, counts, epsilon );

		if( keepon && !counts[SIDE_FRONT] && !counts[SIDE_BACK] )
		{
			neww = in;
			goto out_free;
		}

		if( !counts[side] )
		{
			free_winding( in );
			neww = NULL;
			goto out_free;
		}

		if( !counts[side ^ 1] )
		{
			neww = in;
			goto out_free;
		}

		maxpts = in->numpoints + 4;
		neww = winding_alloc( maxpts );
		neww->numpoints = 0;
		neww->plane = in->plane;

		for( i = 0; i < in->numpoints; i++ )
		{
			p1 = in->p[i];

			if( sides[i] == SIDE_ON )
			{
				VectorCopy( p1, neww->p[neww->numpoints] );
				neww->numpoints++;
				continue;
			}

			if( sides[i] == side )
			{
				VectorCopy( p1, neww->p[neww->numpoints] );
				neww->numpoints++;
			}

			if( sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i] )
				continue;

			// generate a split point
			p2 = in->p[(i + 1) % in->numpoints];
			mid = neww->p[neww->numpoints++];

			dot = dists[i] / (dists[i] - dists[i + 1]);
			for( j = 0; j < 3; j++ )
			{
				// avoid round off error when possible
				if( in->plane->normal[j] == 1.0 )
					mid[j] = in->plane->dist;
				else if( in->plane->normal[j] == -1.0 )
					mid[j] = -in->plane->dist;
				else if( split->normal[j] == 1.0 )
					mid[j] = split->dist;
				else if( split->normal[j] == -1.0 )
					mid[j] = -split->dist;
				else mid[j] = p1[j] + dot * (p2[j] - p1[j]);
			}

			if( in->plane->type < 3 )
				PushToPlaneAxis( mid, in->plane );
		}

		// free the original winding
		free_winding( in );

		// Shrink the winding back to just what it needs...
		neww = winding_shrink(neww);
	out_free:
		free( dists );
		free( sides );

		return neww;
	}

	/*
	==================
	winding_split

	Splits a winding by a plane, producing one or two windings.  The
	original winding is not damaged or freed.  If only on one side, the
	returned winding will be the input winding.  If on both sides, two
	new windings will be created.
	==================
	*/
	static void winding_split( winding_t *in, const mplane_t *split, winding_t **pfront, winding_t **pback )
	{
		vec_t	*dists;
		int	*sides;
		int	counts[3];
		vec_t	dot;
		int	i, j;
		winding_t	*front, *back;
		vec_t	*p1, *p2, *mid;
		int	maxpts;

		dists = (vec_t *)malloc(( in->numpoints + 1 ) * sizeof( vec_t ));
		sides = (int *)malloc(( in->numpoints + 1 ) * sizeof( int ));
		CalcSides(in, split, sides, dists, counts, 0.04f );

		if( !counts[0] && !counts[1] )
		{
			// winding on the split plane - return copies on both sides
			*pfront = winding_copy( in );
			*pback = winding_copy( in );
			goto out_free;
		}

		if( !counts[0] )
		{
			*pfront = NULL;
			*pback = in;
			goto out_free;
		}

		if( !counts[1] )
		{
			*pfront = in;
			*pback = NULL;
			goto out_free;
		}

		maxpts = in->numpoints + 4;
		front = winding_alloc( maxpts );
		front->numpoints = 0;
		front->plane = in->plane;
		back = winding_alloc( maxpts );
		back->numpoints = 0;
		back->plane = in->plane;

		for( i = 0; i < in->numpoints; i++ )
		{
			p1 = in->p[i];

			if( sides[i] == SIDE_ON )
			{
				VectorCopy( p1, front->p[front->numpoints] );
				VectorCopy( p1, back->p[back->numpoints] );
				front->numpoints++;
				back->numpoints++;
				continue;
			}

			if( sides[i] == SIDE_FRONT )
			{
				VectorCopy( p1, front->p[front->numpoints] );
				front->numpoints++;
			}
			else if( sides[i] == SIDE_BACK )
			{
				VectorCopy( p1, back->p[back->numpoints] );
				back->numpoints++;
			}

			if( sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i] )
				continue;

			// generate a split point
			p2 = in->p[(i + 1) % in->numpoints];
			mid = front->p[front->numpoints++];

			dot = dists[i] / (dists[i] - dists[i + 1]);
			for( j = 0; j < 3; j++ )
			{
				// avoid round off error when possible
				if( in->plane->normal[j] == 1.0 )
					mid[j] = in->plane->dist;
				else if( in->plane->normal[j] == -1.0 )
					mid[j] = -in->plane->dist;
				else if( split->normal[j] == 1.0 )
					mid[j] = split->dist;
				else if( split->normal[j] == -1.0 )
					mid[j] = -split->dist;
				else mid[j] = p1[j] + dot * (p2[j] - p1[j]);
			}

			if( in->plane->type < 3 )
				PushToPlaneAxis( mid, in->plane );
			VectorCopy( mid, back->p[back->numpoints] );
			back->numpoints++;
		}

		*pfront = winding_shrink( front );
		*pback = winding_shrink( back );
	out_free:
		free( dists );
		free( sides );
	}

	/* ------------------------------------------------------------------------- */

	/*
	* This is a stack of the clipnodes we have traversed
	* "sides" indicates which side we went down each time
	*/
	static dclipnode_t	*node_stack[MAX_CLIPNODE_DEPTH];
	static int	side_stack[MAX_CLIPNODE_DEPTH];
	static unsigned int	node_stack_depth;

	static void push_node( dclipnode_t *node, int side )
	{
		if( node_stack_depth == MAX_CLIPNODE_DEPTH )
			Host_Error( "node stack overflow\n" );

		node_stack[node_stack_depth] = node;
		side_stack[node_stack_depth] = side;
		node_stack_depth++;
	}

	static void pop_node( void )
	{
		if( !node_stack_depth )
			Host_Error( "node stack underflow\n" );
		node_stack_depth--;
	}

	static void free_hull_polys( hullnode_t *hull_polys )
	{
		winding_t	*w, *next;

		list_for_each_entry_safe( w, next, hull_polys, chain )
		{
			list_del( &w->chain );
			free_winding( w );
		}
	}

	static void hull_windings_r( hull_t *hull, dclipnode_t *node, hullnode_t *polys, hull_model_t *model );

	static void do_hull_recursion( hull_t *hull, dclipnode_t *node, int side, hullnode_t *polys, hull_model_t *model )
	{
		winding_t	*w, *next;

		if( node->children[side] >= 0 )
		{
			dclipnode_t *child = hull->clipnodes + node->children[side];
			push_node( node, side );
			hull_windings_r( hull, child, polys, model );
			pop_node();
		}
		else
		{
			switch( node->children[side] )
			{
			case CONTENTS_EMPTY:
			case CONTENTS_WATER:
			case CONTENTS_SLIME:
			case CONTENTS_LAVA:
				list_for_each_entry_safe( w, next, polys, chain )
				{
					list_del( &w->chain );
					list_add( &w->chain, &model->polys );
				}
				break;
			case CONTENTS_SOLID:
			case CONTENTS_SKY:
				// throw away polys...
				list_for_each_entry_safe( w, next, polys, chain )
				{
					if( w->pair )
						w->pair->pair = NULL;
					list_del( &w->chain );
					free_winding( w );
					model->num_polys--;
				}
				break;
			default:
				Host_Error( "bad contents: %i\n" /* , node->children[side] */ );
				break;
			}
		}
	}

	static void hull_windings_r( hull_t *hull, dclipnode_t *node, hullnode_t *polys, hull_model_t *model )
	{
		mplane_t		*plane = hull->planes + node->planenum;
		hullnode_t	frontlist = LIST_HEAD_INIT( frontlist );
		hullnode_t	backlist = LIST_HEAD_INIT( backlist );
		winding_t		*w, *next, *front, *back;
		unsigned int	i;

		list_for_each_entry_safe( w, next, polys, chain )
		{
			// PARANIOA - PAIR CHECK
			assert( !w->pair || w->pair->pair == w );

			list_del( &w->chain );
			winding_split( w, plane, &front, &back );
			if( front ) list_add( &front->chain, &frontlist );
			if( back ) list_add( &back->chain, &backlist );

			if( front && back )
			{
				if( w->pair )
				{
					// split the paired poly, preserve pairing
					winding_t	*front2, *back2;

					winding_split( w->pair, plane, &front2, &back2 );

					front2->pair = front;
					front->pair = front2;
					back2->pair = back;
					back->pair = back2;

					list_add( &front2->chain, &w->pair->chain );
					list_add( &back2->chain, &w->pair->chain );
					list_del( &w->pair->chain );
					free_winding( w->pair );
					model->num_polys++;
				}
				else
				{
					front->pair = NULL;
					back->pair = NULL;
				}

				model->num_polys++;
				free_winding( w );
			}
		}

		w = winding_for_plane(plane);

		for( i = 0; w && i < node_stack_depth; i++ )
		{
			mplane_t *p = hull->planes + node_stack[i]->planenum;
			w = winding_clip( w, p, false, side_stack[i], 0.00001f );
		}

		if( w )
		{
			winding_t *tmp = winding_copy( w );
			winding_reverse( tmp );

			w->pair = tmp;
			tmp->pair = w;

			list_add( &w->chain, &frontlist );
			list_add( &tmp->chain, &backlist );

			// PARANIOA - PAIR CHECK
			assert( !w->pair || w->pair->pair == w );
			model->num_polys += 2;
		}
		else
		{
			// printf("new winding was clipped away!\n");
		}

		do_hull_recursion( hull, node, 0, &frontlist, model );
		do_hull_recursion( hull, node, 1, &backlist, model );
	}

	static void remove_paired_polys( hull_model_t *model )
	{
		winding_t	*w, *next;

		list_for_each_entry_safe( w, next, &model->polys, chain )
		{
			if( w->pair )
			{
				list_del( &w->chain );
				free_winding( w );
				model->num_polys--;
			}
		}
	}

	static void make_hull_windings( hull_t *hull, hull_model_t *model )
	{
		hullnode_t head = LIST_HEAD_INIT( head );

		node_stack_depth = 0;
		model->num_polys = 0;

		if( hull->planes != NULL )
		{
			hull_windings_r( hull, hull->clipnodes + hull->firstclipnode, &head, model );
			remove_paired_polys( model );
		}
	}

	static void make_polygons_for_hull(model_t *(*Mod_FindName)(int, const char *), int hullnum, model_t *worldmod, hull_model_t *hull_models, int num_hull_models)
	{
		model_t	*mod = worldmod;
		char	name[8];
		int	i;

		if( hullnum < 1 || hullnum > 3 )
			return;

		// rebuild hulls list
		for( i = 0; i < num_hull_models; i++ )
		{
			hull_model_t *model = &hull_models[i];
			free_hull_polys( &model->polys );
			make_hull_windings( &mod->hulls[hullnum], model );
			snprintf( name, sizeof( name ), "*%i", i + 1 );
			mod = Mod_FindName( 0, name );
		}
	}

	static rgb hsv2rgb(hsv in)
	{
		double      hh, p, q, t, ff;
		long        i;
		rgb         out;

		if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
			out.r = in.v;
			out.g = in.v;
			out.b = in.v;
			return out;
		}
		hh = in.h;
		if (hh >= 360.0) hh = 0.0;
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = in.v * (1.0 - in.s);
		q = in.v * (1.0 - (in.s * ff));
		t = in.v * (1.0 - (in.s * (1.0 - ff)));

		switch (i) {
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
		}
		return out;
	}

	static void draw_poly(const winding_t *poly, float alpha)
	{
		float pitch = asin(poly->plane->normal[2]);
		float yaw = atan2(poly->plane->normal[1], poly->plane->normal[0]);
		if (yaw < 0) {
			yaw += (float)M_2_PI;
		}

		float hue = yaw * 180.f / (float)M_PI;
		float sat = ((float)M_PI_2 - fabsf(pitch)) / (float)M_PI_2;
		float value = 0.5f + (((unsigned long)poly->plane->dist % 53) / 53.0f) * 0.6f - 0.3f;
		rgb out = hsv2rgb({ hue, sat, value });

		glColor4f((float)out.r, (float)out.g, (float)out.b, alpha);
		glBegin(GL_POLYGON);
		for( int i = 0; i < poly->numpoints; i++ )
			glVertex3fv(poly->p[i]);
		glEnd();
	}

	void HullInfo::lazy_init_hull(model_t *(*Mod_FindName)(int, const char *), int hullnum)
	{
		if (current_worldmod && !hull_models) {
			hull_models = (hull_model_t *)malloc(sizeof(hull_model_t) * current_worldmod->numsubmodels);
			num_hull_models = current_worldmod->numsubmodels;

			// initialize list
			for (int i = 0; i < current_worldmod->numsubmodels; i++)
			{
				hullnode_t *poly = &hull_models[i].polys;
				poly->next = poly;
				poly->prev = poly;
			}

			current_hullnum = 0;
		}

		if (hull_models && num_hull_models && hullnum != current_hullnum) {
			current_hullnum = hullnum;
			make_polygons_for_hull(Mod_FindName, hullnum, current_worldmod, hull_models, num_hull_models);
		}
	}

	void HullInfo::draw_world_hull(float alpha) const
	{
		if (!hull_models || !num_hull_models)
			return;

		hull_model_t	*hull = &hull_models[0];
		winding_t		*poly;

		glDisable( GL_TEXTURE_2D );
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		list_for_each_entry( poly, &hull->polys, chain )
		{
			draw_poly(poly, alpha);
		}

		glDisable(GL_BLEND);
		glEnable( GL_TEXTURE_2D );
	}

	void HullInfo::draw_model_hull(const model_s *mod, float alpha) const
	{
		if (!hull_models || !num_hull_models || !in_draw_brush)
			return;

		if (!mod || mod->name[0] != '*')
			return;

		int i = atoi(mod->name + 1);
		if (i < 1 || i >= num_hull_models)
			return;

		hull_model_t *hull = &hull_models[i];

		glPolygonOffset(1.0f, 2.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_TEXTURE_2D );
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		winding_t *poly;
		list_for_each_entry( poly, &hull->polys, chain )
		{
			draw_poly(poly, alpha);
		}

		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D );
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	void HullInfo::clean()
	{
		if (hull_models) {
			// release ploygons
			for(int i = 0; i < num_hull_models; i++)
			{
				hull_model_t *model = &hull_models[i];
				free_hull_polys( &model->polys );
			}
			free(hull_models);
			hull_models = nullptr;
		}

		num_hull_models = 0;
		current_hullnum = 0;
	}
}
