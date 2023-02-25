#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "voxel.h"
#include "ray.h"
#include "globals.h"

//SHADING TABLES
int SHADING_TABLE_BLUE[] = {5, 21, 20, 19, 18, 17};
int SHADING_TABLE_WHITE[] = {17, 231, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240};
int SHADING_TABLE_RED[] = {4, 196, 160, 124, 88};
int SHADING_TABLE_BLACK[] = {3, 236, 233, 16};
int SHADING_TABLE_GOLD[] = {4, 220, 178, 136, 3};
int SHADING_TABLE_NONE[] = {0};

void DEFAULT_RAY_SHADER (struct ray* r) {

}

void DEFAULT_FRAG_SHADER (struct rayinfo* r, double x, double y) {

}

void NO_LIGHTING (struct ray* r) {

}

void DEFAULT_LIGHTING_SHADER (struct ray* r) {
	int* shading_table;
	switch (r->info.color) {
		case 12:
			shading_table = SHADING_TABLE_BLUE;
			break;
		case 15:
			shading_table = SHADING_TABLE_WHITE;
			break;
		case 9:
			shading_table = SHADING_TABLE_RED;
			break;
		case 16:
			shading_table = SHADING_TABLE_BLACK;
			break;
		case 11:
			shading_table = SHADING_TABLE_GOLD;
			break;
		default:
			shading_table = SHADING_TABLE_NONE;
			break;
	}
	if (shading_table == SHADING_TABLE_NONE) {
		return;
	}
	//Shadows are a bit of a work in progress
	
	struct ray* shadow_ray = malloc (sizeof (struct ray));
	makeRay (shadow_ray, globals()->lightsources[0].x, globals()->lightsources[0].y, globals()->lightsources[0].z, r->cx, r->cy, r->cz);
	(&(shadow_ray->info))->do_lighting = 0;
	int i = 0;
	while (i < 30 && !((&(shadow_ray->info))->finished)) {
		//printf ("%f,%f,%f\n", shadow_ray->cx, shadow_ray->cy, shadow_ray->cz);
		cast (r->world, shadow_ray);
		i++;
	}
	if (shadow_ray->cvx == r->cvx && shadow_ray->cvy == r->cvy && shadow_ray->cvz == r->cvz) {
		int len = shading_table[0];
		double diffx = globals()->lightsources[0].x - r->cx;
		double diffy = globals()->lightsources[0].y - r->cy;
		double diffz = globals()->lightsources[0].z - r->cz;
		double dist = sqrt (diffx * diffx + diffy * diffy + diffz * diffz);
		int used = (int)(len * (dist / globals()->lightsources[0].range));
		//printf ("%f\n", len * (dist / globals()->lightsources[0].range));
		if (used >= len) {
			used = len - 1;
		}
		(&(r->info))->color = shading_table[used + 1];
	} else {
		//printf ("%d,%d,%d; %d,%d,%d\n", shadow_ray->cvx, shadow_ray->cvy, shadow_ray->cvz, r->cvx, r->cvy, r->cvz);
		(&(r->info))->color = shading_table[shading_table[0]];
	}
}

struct voxel* makeEmptyVoxel (struct voxel* v) {
	if (v) {
		v->solid = 0;
		v->vid = 0;
		v->ray_shader = DEFAULT_RAY_SHADER;
		v->frag_shader = DEFAULT_FRAG_SHADER;
		v->lighting_shader = DEFAULT_LIGHTING_SHADER;
	} else {
		return NULL;
	}
}
