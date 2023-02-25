#ifndef VOXEL_H
#define VOXEL_H

#include "raystruct.h"

//SHADING TABLES


struct voxel {
	int solid;
	int vid;
	void (*ray_shader)(struct ray* r);
	void (*lighting_shader)(struct ray* r);
	void (*frag_shader)(struct rayinfo* r, double x, double y);
};

void DEFAULT_RAY_SHADER (struct ray* r);
void DEFAULT_FRAG_SHADER (struct rayinfo* r, double x, double y);
void DEFAULT_LIGHTING_SHADER (struct ray* r);
void NO_LIGHTING (struct ray* r);
struct voxel* makeEmptyVoxel (struct voxel* v);

#endif
