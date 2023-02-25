#ifndef RAYSTRUCT_H
#define RAYSTRUCT_H

struct rayinfo {
	int color;
	int finished;
	int steps;
	int do_lighting;
};

struct ray {
	int mode;
	//Start coordinates
	double sx;
	double sy;
	double sz;
	//Current coordinates
	double cx;
	double cy;
	double cz;
	//Current voxel
	int cvx;
	int cvy;
	int cvz;
	//Ray directions
	int dx;
	int dy;
	int dz;
	//Slopes
	double mxy;
	double mxz;
	double myx;
	double myz;
	double mzx;
	double mzy;
	//Other ray info
	struct rayinfo info;
	//Za warudo
	struct world* world;
};

struct lightsource {
	double x;
	double y;
	double z;
	double range;
};

#endif
