#ifndef WORLD_H
#define WORLD_H

#include "voxel.h"
#include "movefuncs.h"

struct world {
	int sizeX;
	int sizeZ;
	int sizeY;
	struct voxel* map;
};

struct world* makeWorld (int sizeX, int sizeY, int sizeZ);
struct voxel* getVoxel (struct world* w, int x, int y, int z);
void update_world (struct world* w, struct gameState* gstate);

#endif
