#include "termbox.h"
#include "camera.h"
#include "canvas.h"

#include "3d.h"

int canvas_width = 0;
int canvas_height = 0;
struct tb_cell cell;
int cursorx = 0;
int cursory = 0;

int d3d_window_x = 0;
int d3d_window_y = 0;
int d3d_window_width = 0;
int d3d_window_height = 0;

void d3d_refresh_charmap () {
	tb_select_output_mode (TB_OUTPUT_256);
	tb_utf8_char_to_unicode (&cell.ch, "▀");
	int i, j;
	for (i = d3d_window_x; i < d3d_window_width; i++) {
		for (j = d3d_window_y; j < d3d_window_height / 2; j++) {
			cell.fg = 0; //Black-ish
			cell.bg = 0; //Same
			tb_put_cell (i, j, &cell);
		}
	}
}

void d3d_draw_window () {
	render ();
}

void d3d_setup () {
	d3d_refresh_charmap ();
}

void setPixel (int x, int y, int color) {
	x += d3d_window_x;
	y += d3d_window_y;
	if (x < 0 || y < 0 || x >= d3d_window_width || y >= d3d_window_height * 2) {
		return;
	}
	struct tb_cell* cells = tb_cell_buffer ();
	struct tb_cell* current = cells + (x + (y >> 1) * tb_width ());
	if (y % 2 == 0) {
		current->fg = color;
	} else {
		current->bg = color;
	}
}

int canvasWidth () {
	return d3d_window_width;
}

int canvasHeight () {
	return d3d_window_height;
}
