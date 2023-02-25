#ifndef RAY_H
#define RAY_H

#define OVER_X 0
#define OVER_Y 1;
#define OVER_Z 2;

#include "raystruct.h"
#include "world.h"

extern int lightsource_count;
extern struct lightsource* lightsources;

struct ray* makeRay (struct ray* location, double xStart, double yStart, double zStart, double xThrough, double yThrough, double zThrough);
int cast (struct world* w, struct ray* r);
void BORDER_RAY_SHADER (struct ray* r);
void BORDER_FRAG_SHADER (struct rayinfo* r, double x, double y);
void SIDE_SHADER (struct rayinfo* r, double x, double y);

#endif
