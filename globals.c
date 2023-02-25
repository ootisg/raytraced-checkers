#include "globals.h"

#include <stdlib.h>

#include "ray.h"

struct global_vars* global_vars;

struct global_vars* globals () {
	if (!global_vars) {
		//Allocate the globals struct
		global_vars = malloc (sizeof (struct global_vars));
		//Initialize the contents
		//Ray globals
		global_vars->lightsource_count = 0;
		global_vars->lightsources = NULL;
		//Camera globals
		global_vars->camx = 4.5;
		global_vars->camy = 4.5;
		global_vars->camz = 4.5;
		global_vars->hrot = 0;
		global_vars->vrot = 0;
		global_vars->fov = 3.1415926 / 2;
		global_vars->plane_dist = 1;
		global_vars->max_steps = 25;
		global_vars->distortion_type = 1;
	}
	return global_vars;
}
