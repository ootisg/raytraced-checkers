#ifndef CANVAS_H
#define CANVAS_H

#include "loadfuncs.h"
#include "termbox.h"

void setPixel (int x, int y, int color);
int canvasWidth ();
int canvasHeight ();
int canvasX ();
int canvasY ();
struct tb_cell* getCell (int x, int y);
void draw_str (char* str, int x, int y, int fg, int bg);
void draw_char (char c, int x, int y, int fg, int bg);
void draw_move (struct charbuffer* move, int x, int y, int fg, int bg, int cursor);
void copy_state (struct gameState* src, struct gameState* dest);
void add_move (char* move);
void load_file (char* filename);
void import_file (struct charbuffer* in);
void resize_columns ();
void refresh_moves ();
void draw_filename ();
void draw_keys ();
void update_board ();
void evaluate_moves ();
void canvas_save_file ();

#endif
