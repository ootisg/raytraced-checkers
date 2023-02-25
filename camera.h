#ifndef CAMERA_H
#define CAMERA_H

#define MAX_STEPS 20

extern double hrot;
extern double vrot;
extern double camx;
extern double camy;
extern double camz;

extern int distortion_type;

void render ();
void turn_left ();
void turn_right ();
void forward ();
void backward ();
void lookup ();
void lookdown ();
void up ();
void down ();

extern struct world* world;

void set_attributes (double h_rot, double v_rot, double x, double y, double z);

#endif
