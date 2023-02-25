#ifndef D3D_H
#define D3D_H

#include "raystruct.h"
#include "voxel.h"
#include "world.h"
#include "camera.h"

int d3d_window_x;
int d3d_window_y;
int d3d_window_width;
int d3d_window_height;

void d3d_draw_window ();
void d3d_refresh_charmap ();
void d3d_setup ();
void d3d_render ();

#endif
