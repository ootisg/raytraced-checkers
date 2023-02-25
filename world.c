#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "world.h"
#include "voxel.h"
#include "outfuncs.h"
#include "ray.h"
#include "board_constants.h"
#include "globals.h"

double dist_squared_2d (double x1, double y1, double x2, double y2) {
	return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

void red_shader (struct rayinfo* r, double x, double y) {
	r->color = 15;
}

void nothing_shader (struct rayinfo* r, double x, double y) {

}

void other_shader (struct rayinfo* r, double x, double y) {
	r->color = 12;
}

void other_other_shader (struct rayinfo* r, double x, double y) {
	r->color = 8;
}

void other_other_other_shader (struct rayinfo* r, double x, double y) {
	r->color = 2;
}

void cylinder_shader (struct ray* r) {
	double circle_x_offset = r->cvx + .5;
	double circle_z_offset = r->cvz + .5;
	int king_color = 0;
	if (r->dx) {

		double radius = .33; //Circle radius
		double height = .25; //Checker height

		int do_render = 0;

		double m = r->mzx; //Slope
		double k = r->sz - m * (r->sx); //Intercept

		//Quadratic terms derived from intersecting a line with a circle
		double a = m * m + 1;
		double b = 2 * (m * k - m * circle_z_offset - circle_x_offset);
		double c = k * k - 2 * k * circle_z_offset + circle_z_offset * circle_z_offset + circle_x_offset * circle_x_offset - radius * radius;

		double mid_term = b * b - 4 * a * c;
		
		//printf ("%f,%f,%f\n", a, b, c);
		//printf ("%f,%f,%f,%f\n", m, k, r->sx, r->sz);
		//printf ("%f\n", (-b + sqrt (mid_term))/(2 * a));
		//printf ("%f\n", (-b - sqrt (mid_term))/(2 * a));

		if (mid_term >= 0) {
			//Collision exists
			double checkx;
			double checky;
			checkx = (-b + sqrt(mid_term))/(2*a);
			checky = r->myx * (checkx - r->sx) + r->sy;
			if (checky > r->cvy + 1 - height && checky < r->cvy + 1) {
				do_render = 1;
			}
			checkx = (-b - sqrt(mid_term))/(2*a);
			checky = r->myx * (checkx - r->sx) + r->sy;
			if (checky > r->cvy + 1 - height && checky < r->cvy + 1) {
				do_render = 1;
			}
		}

		double planex;
		double planey = r->cvy + 1 - height;
		double planez;	
		
		planex = r->mxy * (planey - r->sy) + r->sx;
		planez = r->mzy * (planey - r->sy) + r->sz;
		
		double dist = dist_squared_2d (r->cvx + .5, r->cvz + .5, planex, planez);

		if (dist <= radius * radius) {
			do_render = 1;
		}
		
		if (dist >= (radius * radius) * (.6 * .6) && dist <= radius * radius) {
			do_render = 1;
			king_color = 1;
		}

		(&(r->info))->finished = do_render;
		if (do_render) {
			int color;
			switch (getVoxel (r->world, r->cvx, r->cvy, r->cvz)->vid) {
				case 0:
					color = 0;
					break;
				case 1:
					color = 9;
					break;
				case 2:
					color = 16;
					break;
				case 3:
					if (king_color) {
						color = 11;
					} else {
						color = 9;
					}
					break;
				case 4:
					if (king_color) {
						color = 11;
					} else {
						color = 16;
					}
					break;
			}
			(&(r->info))->color = color;
		}
	}
}

struct world* makeWorld (int sizeX, int sizeY, int sizeZ) {
	struct world* w = malloc (sizeof (struct world));
	w->sizeX = sizeX;
	w->sizeY = sizeY;
	w->sizeZ = sizeZ;
	w->map = malloc (sizeof (struct voxel) * sizeX * sizeY * sizeZ);
	int wx, wy, wz;
	for (wx = 0; wx < sizeX; wx++) {
		for (wy = 0; wy < sizeY; wy++) {
			for (wz = 0; wz < sizeZ; wz++) {
				makeEmptyVoxel (w->map + (wx * sizeY * sizeZ + wy * sizeZ + wz));
			}
		}
	}
	for (wx = 0; wx < sizeX; wx++) {
		for (wz = 0; wz < sizeZ; wz++) {
			struct voxel* v = getVoxel (w, wx, 7, wz);
			v->solid = 1;
			if ((wx + wz) % 2 == 0) {
				v->frag_shader = red_shader;
			} else {
				v->frag_shader = other_shader;
			}
			/*v = getVoxel (w, wx, 6, wz);
			v->ray_shader = cylinder_shader;
			v->solid = 1;
			v->frag_shader = other_other_shader;*/
			/*v = getVoxel (w, wx, 6, wz);
			v->solid = 1;
			if ((wx + wz) % 2 == 1) {
				v->frag_shader = red_shader;
			} else {
				v->frag_shader = other_shader;
			}*/
			/*if (wx == 0 || wz == 0 || wx == sizeX - 1 || wz == sizeZ - 1) {
				int i;
				for (i = 5; i < 7; i++) {
					v = getVoxel (w, wx, i, wz);
					v->solid = 1;
					v->frag_shader = red_shader;
				}
			}*/
		}
	}
	/*struct voxel* v = getVoxel (w, 7, 6, 5); v->frag_shader = other_other_shader;
	v->ray_shader = cylinder_shader;
	v->solid = 1;*/
	globals()->lightsources = malloc (sizeof (struct lightsource));
	globals()->lightsources->x = 4.01;
	globals()->lightsources->y = 5.5;
	globals()->lightsources->z = 4.01;
	globals()->lightsources->range = 8.0;
	globals()->lightsource_count = 1;
	return w;
}

void update_world (struct world* w, struct gameState* gstate) {
	struct board* board = gstate->boardState;
	struct square* current;
	int wx;
	int wy;
	for (wy = 0; wy < 8; wy++) {
		for (wx = 0; wx < 8; wx++) {
			current = getSquare (board, wx, wy);
			struct voxel* v = getVoxel (w, wx, 6, wy);
			if (current->empty) {
				v->solid = 0;
				v->ray_shader = DEFAULT_RAY_SHADER;
				v->frag_shader = nothing_shader;
			} else {
				v->solid = 1;
				v->ray_shader = cylinder_shader;
				v->frag_shader = nothing_shader;
				if (current->piece_color == SQUARE_COLOR_RED) {
					if (current->piece_type == PIECE_TYPE_PAWN) {
						v->vid = 1;
					} else {
						v->vid = 3;
					}
				} else {
					if (current->piece_type == PIECE_TYPE_PAWN) {
						v->vid = 2;
					} else {
						v->vid = 4;
					}
				}
			}
		}
	}
}

struct voxel* getVoxel (struct world* w, int x, int y, int z) {
	return &(w->map[x * w->sizeY * w->sizeZ + y * w->sizeZ + z]);
}
