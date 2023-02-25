#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "camera.h"
#include "ray.h"
#include "world.h"
#include "canvas.h"
#include "globals.h"

#include "3d.h"

/*int main () {
	struct world* w = makeWorld (20, 20, 20);
	struct ray* r = malloc (sizeof (struct ray));
	makeRay (r, 5.5, 5.5, 5.5, 4.5, 6.5, 5.5);
	printf ("%f, %f, %f\n", r->cx, r->cy, r->cz);
	cast (w, r);
	printf ("%f, %f, %f\n", r->cx, r->cy, r->cz);
}*/

void project_sphere (double* res, double x, double y, double z, double phi, double theta, double radius) {
	res[0] = radius * cos (phi) * cos (theta) + x;
	res[1] = radius * sin (theta) + y;
	res[2] = radius * sin (phi) * cos (theta) + z;
} 

void set_attributes (double h_rot, double v_rot, double x, double y, double z) {
	globals()->hrot = h_rot;
	globals()->vrot = v_rot;
	globals()->camx = x;
	globals()->camy = y;
	globals()->camz = z;
}

void render () {

	if (!globals()->world) {
		globals()->world = makeWorld (8, 8, 8);
	}
	/*struct ray* r = malloc (sizeof (struct ray));
	makeRay (r, 5.5, 5.5, 5.5, 8.5, 8.2, 5.56);
	cast (world, r);
	cast (world, r);
	printf ("%d,%d,%d,%d\n", r->cvx, r->cvy, r->cvz, r->info.color);*/
	struct ray* r = malloc (sizeof (struct ray));
	int width = canvasWidth ();
	int height = canvasHeight ();
	if (width == 0 || height == 0) {
		width = 20;
		height = 20;
	}
	double aspect_ratio = ((double)(height)) / width;
	double hfov = globals()->fov;
	double vfov = hfov * aspect_ratio;
	vfov = hfov;
	double tlhang = globals()->hrot - hfov / 2;
	double tlvang = globals()->vrot - vfov / 2;
	double brhang = globals()->hrot + hfov / 2;
	double brvang = globals()->vrot + vfov / 2;
	globals()->plane_dist = sin (globals()->fov) / 2;
	double coords_buffer[12];
	double* cc = coords_buffer;
	project_sphere (cc, globals()->camx, globals()->camy, globals()->camz, tlhang, tlvang, globals()->plane_dist);
	project_sphere (cc + 3, globals()->camx, globals()->camy, globals()->camz, brhang, tlvang, globals()->plane_dist);
	//*throws up hands* Swapping out the 6 for the 9 and vice versa lets you look straight down without distortions. Don't ask me why.
	
	if (globals()->distortion_type) {
		project_sphere (cc + 9, globals()->camx, globals()->camy, globals()->camz, tlhang, brvang, globals()->plane_dist);
		project_sphere (cc + 6, globals()->camx, globals()->camy, globals()->camz, brhang, brvang, globals()->plane_dist);
	} else {
		project_sphere (cc + 6, globals()->camx, globals()->camy, globals()->camz, tlhang, brvang, globals()->plane_dist);
		project_sphere (cc + 9, globals()->camx, globals()->camy, globals()->camz, brhang, brvang, globals()->plane_dist);
	}
	//printf ("%f, %f, %f, %f\n", tlhang, tlvang, brhang, brvang);
	//4.5, 4.5, 5.5
	//cc[1] = 5;
	//cc[4] = 5;
	//cc[7] = 5;
	//cc[10] = 5;
	/*cc[0] = 4.25;
	cc[1] = 5;
	cc[2] = 4.25;
	cc[3] = 4.25;
	cc[4] = 5;
	cc[5] = 4.75;
	cc[6] = 4.75;
	cc[7] = 5;
	cc[8] = 4.25;
	cc[9] = 4.75;
	cc[10] = 5;
	cc[11] = 4.75;*/
	//printf ("%f,%f,%f;%f,%f,%f;%f,%f,%f;%f,%f,%f\n", cc[0], cc[1], cc[2], cc[3], cc[4], cc[5], cc[6], cc[7], cc[8], cc[9], cc[10], cc[11]);
	int wx, wy;
	double xsteph = (cc[3] - cc[0]) / width;
	double ysteph = (cc[4] - cc[1]) / width;
	double zsteph = (cc[5] - cc[2]) / width;
	double xstepv = (cc[6] - cc[0]) / height;
	double ystepv = (cc[7] - cc[1]) / height;
	double zstepv = (cc[8] - cc[2]) / height;
	double cx = cc[0];
	double cy = cc[1];
	double cz = cc[2];
	for (wy = 0; wy < height; wy++) {
		cx = cc[0] + xstepv * wy;
		cy = cc[1] + ystepv * wy;
		cz = cc[2] + zstepv * wy;
		for (wx = 0; wx < width; wx++) {
			makeRay (r, globals()->camx, globals()->camy, globals()->camz, cx, cy, cz);
			cx += xsteph;
			cy += ysteph;
			cz += zsteph;
			//printf ("%f, %f, %f, %f, %f\n", cx, cy, cz, zsteph, zstepv);
			//Do ray tracing here
			while (!(&(r->info))->finished && (&(r->info))->steps < globals()->max_steps) {
				cast (globals()->world, r);
			}
			setPixel (wx + canvasX (), wy + canvasY (), (&(r->info))->color);
		}
	}
}

void turn_left () {
	globals()->hrot -= M_PI / 20;
}

void turn_right () {
	globals()->hrot += M_PI / 20;
}

void forward () {
	globals()->camx += cos (globals()->hrot) * .5;
	globals()->camz += sin (globals()->hrot) * .5;
}

void up () {
	globals()->camy += .5;
}

void down () {
	globals()->camy -= .5;
}

void backward () {
	globals()->camx -= cos (globals()->hrot) * .5;
	globals()->camz -= sin (globals()->hrot) * .5;
}

void lookup () {
	globals()->vrot += M_PI / 60;
}

void lookdown () {
	globals()->vrot -= M_PI / 60;
}
