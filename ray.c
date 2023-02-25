#include <stdio.h>
#include <math.h>

#include "ray.h"
#include "world.h"
#include "camera.h"

/*

Just for reference
   
struct rayinfo {
	int color;
	int finished;
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
	double cvx;
	double cvy;
	double cvz;
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
};
*/

double sortyboi[3];

//Yay for helper functions!
double distSqr (double ax, double ay, double az, double bx, double by, double bz) {
	//Three-dimensional pythagorean theorem (yes, I'm aware it's slow)
	double dx, dy, dz;
	dx = bx - ax;
	dy = by - ay;
	dz = bz - az;
	return (dx * dx + dy * dy + dz * dz);
}

struct ray* makeRay (struct ray* location, double xStart, double yStart, double zStart, double xThrough, double yThrough, double zThrough) {
	struct ray* r = location;
	//Set start coordinates
	r->sx = xStart;
	r->sy = yStart;
	r->sz = zStart;
	r->cx = r->sx;
	r->cy = r->sy;
	r->cz = r->sz;
	r->cvx = (int)xStart;
	r->cvy = (int)yStart;
	r->cvz = (int)zStart;
	//Finds some directions n stuff.
	if (xThrough > xStart) {
		r->dx = 1;
	} else if (xThrough < xStart) {
		r->dx = -1;
	} else {
		r->dx = 0;
	}
	if (yThrough > yStart) {
		r->dy = 1;
	} else if (yThrough < yStart) {
		r->dy = -1;
	} else {
		r->dy = 0;
	}
	if (zThrough > zStart) {
		r->dz = 1;
	} else if (zThrough < zStart) {
		r->dz = -1;
	} else {
		r->dz = 0;
	}
	//Calculate slopes
	double xdiff = xThrough - xStart;
	double ydiff = yThrough - yStart;
	double zdiff = zThrough - zStart;
	if (r->dx) {
		r->myx = ydiff / xdiff;
		r->mzx = zdiff / xdiff;
	}
	if (r->dy) {
		r->mxy = xdiff / ydiff;
		r->mzy = zdiff / ydiff;
	}
	if (r->dz) {
		r->mxz = xdiff / zdiff;
		r->myz = ydiff / zdiff;
	}
	r->info.color = 0;
	r->info.finished = 0;
	r->info.steps = 0;
	r->info.do_lighting = 1;
	//printf ("Dirs: %d, %d, %d\n", r->dx, r->dy, r->dz);
	//printf ("Slopes: %f, %f, %f, %f, %f, %f\n", r->myx, r->mzx, r->mxy, r->mzy, r->mxz, r->myz);
}

int cast (struct world* w, struct ray* r) {
	
	r->world = w;

	//Calculate next 'grid snap' coordinates
	double snapx, snapy, snapz;
	if (r->dx == 1) {
		snapx = floor (r->cx) + r->dx;
	} else {
		snapx = ceil (r->cx) + r->dx;
	}
	if (r->dy == 1) {
		snapy = floor (r->cy) + r->dy;
	} else {
		snapy = ceil (r->cy) + r->dy;
	}
	if (r->dz == 1) {
		snapz = floor (r->cz) + r->dz;
	} else {
		snapz = ceil (r->cz) + r->dz;
	}
	//snapx = floor (r->cx) + (r->dx == 1);
	//snapy = floor (r->cy) + (r->dy == 1);
	//snapz = floor (r->cz) + (r->dz == 1);

	//printf ("Snap: %f, %f, %f\n", snapx, snapy, snapz);
	//Calculate the coordinates and find distances (OOF, QUITE A PERFORMANCE HIT)
	double xx, xy, xz, yx, yy, yz, zx, zy, zz; //First letter is the direction, second is the coordinate
	double xdist, ydist, zdist;
	if (r->dx) {
		xx = snapx;
		xy = r->myx * (snapx - r->sx) + r->sy;
		xz = r->mzx * (snapx - r->sx) + r->sz;
		sortyboi[0] = distSqr (r->cx, r->cy, r->cz, xx, xy, xz);
	} else {
		sortyboi[0] = 420;
	}
	if (r->dy) {
		yy = snapy;
		yx = r->mxy * (snapy - r->sy) + r->sx;
		yz = r->mzy * (snapy - r->sy) + r->sz;
		sortyboi[1] = distSqr (r->cx, r->cy, r->cz, yx, yy, yz);
	} else {
		sortyboi[1] = 420;
	}
	if (r->dz) {
		zz = snapz;
		zx = r->mxz * (snapz - r->sz) + r->sx;
		zy = r->myz * (snapz - r->sz) + r->sy;
		sortyboi[2] = distSqr (r->cx, r->cy, r->cz, zx, zy, zz);
	} else {
		sortyboi[2] = 420;
	}

	//printf ("%f, %f, %f\n", yx, yy, yz);

	//Find the shortest path to the next grid cell
	
	int closest = -1; //0 is x, 1 is y, 2 is z
	double min = 420;
	int ci;
	for (ci = 0; ci < 3; ci++) {
		if (sortyboi[ci] < min) {
			min = sortyboi[ci];
			closest = ci;
		}
	}
	
	//Update ray accordingly and apply shaders
	double surfaceX;
	double surfaceY;
	int side;
	if (closest == 0) {
		//x was closest
		r->cx = xx;
		r->cy = xy;
		r->cz = xz;
		//r->cvx = (int)(r->cx) - (r->dx == -1);
		r->cvx += r->dx;
		surfaceX = r->cy;
		surfaceY = r->cz;
	} else if (closest == 1) {
		//y was closest
		r->cx = yx;
		r->cy = yy;
		r->cz = yz;
		//r->cvy = (int)(r->cy) - (r->dy == -1);
		r->cvy += r->dy;
		surfaceX = r->cx;
		surfaceY = r->cz;
	} else if (closest == 2) {
		//z was closest
		r->cx = zx;
		r->cy = zy;
		r->cz = zz;
		//r->cvz = (int)(r->cz) - (r->dz == -1);
		r->cvz += r->dz;
		surfaceX = r->cx;
		surfaceY = r->cy;
	}
	struct voxel* v;
	if (r->cvx < 0 || r->cvy < 0 || r->cvz < 0 || r->cvx >= w->sizeX || r->cvy >= w->sizeY || r->cvz >= w->sizeZ) {
		//(&(r->info))->finished = 1;
		//BORDER_RAY_SHADER (r);
		//BORDER_FRAG_SHADER (&(r->info), surfaceX, surfaceY);
	} else {
		v = getVoxel (w, r->cvx, r->cvy, r->cvz);
		(&(r->info))->finished = v->solid;
		v->ray_shader (r);
		if (v->solid) {
			v->frag_shader (&(r->info), surfaceX, surfaceY);
			/*if (closest == 1) {
				SIDE_SHADER (&(r->info), surfaceX, surfaceY);
			}*/
			//printf ("%f\n", r->cy);
		}
		if (r->info.do_lighting) {
			v->lighting_shader (r);
		}
	}
	(&(r->info))->steps++;
	if ((&(r->info))->steps == MAX_STEPS) {
		BORDER_RAY_SHADER (r);
		BORDER_FRAG_SHADER (&(r->info), surfaceX, surfaceY);
	}
}

void BORDER_RAY_SHADER (struct ray* r) {

}

void BORDER_FRAG_SHADER (struct rayinfo* r, double x, double y) {
	r->color = 117;
}

void SIDE_SHADER (struct rayinfo* r, double x, double y) {
	r->color = 8;
}
