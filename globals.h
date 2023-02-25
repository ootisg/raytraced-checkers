#ifndef GLOBALS_H
#define GLOBALS_H

struct global_vars {
	int lightsource_count;
	struct lightsource* lightsources;
	struct world* world;
	double camx;
	double camy;
	double camz;
	double hrot;
	double vrot;
	double fov;
	double plane_dist;
	double max_steps;
	int distortion_type;
};

struct global_vars* globals ();

#endif
