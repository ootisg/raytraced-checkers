#include <stdio.h>
#include <stdlib.h>

#include "termbox.h"
#include "camera.h"
#include "canvas.h"
#include "world.h"

#include "outfuncs.h"
#include "loadfuncs.h"
#include "charbuffer.h"
#include "movefuncs.h"
#include "globals.h"

int canvas_width = 0;
int canvas_height = 0;
struct tb_cell cell;
int cursorx = 0;
int cursory = 0;

int view_window_x;
int view_window_y;
int view_window_width;
int view_window_height;

int view_mode = 0;
int file_entry = 0;
int current_view = 0;

/*
 * Variables for displaying/storing moves
 */
int selected_move = 0;
int move_column_x;
int move_column_y;
int move_column_height;
int move_column_scroll;
int max_display_length = 6;

int num_moves = 0;
struct charbuffer* movebuffer = 0;
struct gameState** movelist = 0;
struct gameState* global_attributes;
struct procst* prevents_segfaults;

int padding_left = 4;
int padding_center = 8;
int padding_right = 2;
int column_widths[2];
int column_x[2];
int screen_x = 20;
int move_cursor = 0;
int move_cursor_drawpos = 0;

int column_start_y = 10;

int COLOR_VALID = 2;
int COLOR_ERR = 9;
int COLOR_UNKNOWN = 0;

struct charbuffer* canvas_filename;

int main (int argc, char** argv) {
	canvas_filename = makebuffer ();
	char* filename;
	if (argc > 1) {
		filename = argv[1];
	} else {
		printf ("No filename was specified as an argument. Please enter the name of the file you want to edit.\n");
		char* s = malloc (1024);
		scanf ("%s", s);
		filename = s;
	}
	addstr (canvas_filename, filename);
	add (canvas_filename, 0);
	import_file (getFile (canvas_filename->data));
	//Tb stuff
	set_attributes (-1.884956, 1.308997, 3.700499, .5, 6.814861);
	tb_init ();
	tb_select_output_mode (TB_OUTPUT_256);
	resize_columns ();
	int i, j;
	draw_str (canvas_filename->data, screen_x, 0, 0, 0);
	refresh_moves ();
	render ();
	draw_keys ();
	if (global_attributes) {
		update_world (globals()->world, global_attributes);
	}
	render ();
	for (;;) {
		tb_present ();
		struct tb_event event;
		tb_poll_event (&event);
		if (TB_KEY_ESC == event.key) {
			tb_shutdown ();
			break;
		}
		if (file_entry) {
			/*if (TB_KEY_DELETE == event.key || TB_KEY_BACKSPACE == event.key) {
				if (canvas_filename->length > 0) {
					canvas_filename->length--;
				}
			} else if (TB_KEY_ENTER == event.key) {
				add (canvas_filename, 0);
				//import_file (getFile (canvas_filename->data));
				//resize_columns ();
				//refresh_moves ();
				//render ();
				//update_world (world, global_attributes);
				//render ();
				file_entry = 0;
				selected_move = 0;
			} else {
				add (canvas_filename, event.ch);
			}
			int i;
			for (i = screen_x; i < tb_width (); i++) {
				draw_char (' ', i, 0, 0, 0);
			}
			for (i = 0; i < canvas_filename->length; i++) {
				draw_char (canvas_filename->data[i], screen_x + i, 0, 0, 0);
			}*/
		} else {
			if ('o' == event.ch) {
				//File entry mode always segfaults for some reason
				/*canvas_filename = makebuffer ();
				file_entry = 1;
				int i;
				for (i = screen_x; i < tb_width (); i++) {
					draw_char (' ', i, 0, 0, 0);
				}*/
			}
			if (view_mode) {
				if (TB_KEY_ARROW_LEFT == event.key) {
					turn_left ();
					render ();
				}
				if (TB_KEY_ARROW_RIGHT == event.key) {
					turn_right ();
					render ();
				}
				if (TB_KEY_ARROW_UP == event.key) {
					forward ();
					render ();
				}
				if (TB_KEY_ARROW_DOWN == event.key) {
					backward ();
					render ();
				}
				if ('r' == event.ch) {
					up ();
					render ();
				}
				if ('f' == event.ch) {
					down ();
					render ();
				}
				if ('w' == event.ch) {
					lookup ();
					render ();
				}
				if ('s' == event.ch) {
					lookdown ();
					render ();
				}
				if ('d' == event.ch) {
					/*tb_shutdown ();
					printf ("%f,%f,%f,%f,%f\n", hrot, vrot, camx, camy, camz);
					break;*/
				}
				if ('m' == event.ch) {
					if (current_view == 0) {
						globals()->distortion_type = 0;
						set_attributes (0, 0, 4.5, 5.5, 4.5);
						current_view = 1;
					} else if (current_view == 1) {
						globals()->distortion_type = 1;
						set_attributes (-1.884956, 1.308997, 3.700499, .5, 6.814861);
						current_view = 0;
					}
					render ();
				}
				if ('v' == event.ch) {
					view_mode = 0;
					draw_keys ();
					refresh_moves ();
					render ();
				}
			} else {
				if ((event.ch >= 'a' && event.ch <= 'h' && move_cursor % 2 == 0) || (event.ch >= '1' && event.ch <= '8' && move_cursor % 2 == 1)) {
					if (selected_move > num_moves - 1) {
						add_move ("");
					}
					movelist = (struct gameState**)(movebuffer->data);
					if (movelist[selected_move]->moves->length == move_cursor) {
						//movelist[i]->moves->data[move_cursor - 1] = event.ch;
						add (movelist[selected_move]->moves, event.ch);
					} else {
						movelist[selected_move]->moves->data[move_cursor] = event.ch;
					}
					evaluate_moves ();
					move_cursor++;
					refresh_moves ();
					draw_keys ();
					render ();
				}
				if (TB_KEY_DELETE == event.key) {
					movelist = (struct gameState**)(movebuffer->data);
					if (movelist[selected_move]->moves->length != 0) {
						movelist[selected_move]->moves->length--;
						move_cursor = 1;
					}
					evaluate_moves ();
					move_cursor--;
					refresh_moves ();
					draw_keys ();
					render ();
				}
				if (TB_KEY_ARROW_UP == event.key) {
					move_cursor = 0;
					move_cursor_drawpos = 0;
					selected_move -= 2;
					refresh_moves ();
					update_board ();
					render ();
				}
				if (TB_KEY_ARROW_DOWN == event.key) {
					move_cursor = 0;
					move_cursor_drawpos = 0;
					selected_move += 2;
					refresh_moves ();
					update_board ();
					render ();
				}
				if (TB_KEY_ARROW_LEFT == event.key) {
					move_cursor = 0;
					move_cursor_drawpos = 0;
					selected_move--;
					refresh_moves ();
					update_board ();
					render ();
				}
				if (TB_KEY_ARROW_RIGHT == event.key) {
					move_cursor = 0;
					move_cursor_drawpos = 0;
					selected_move++;
					refresh_moves ();
					update_board ();
					render ();
				}
				if ('v' == event.ch) {
					view_mode = 1;
					draw_keys ();
					refresh_moves ();
					render ();
				}
				if ('s' == event.ch) {
					canvas_save_file ();
				}
			}
		}
	} 
}

void load_file (char* filepath) {
	struct charbuffer* in = getFile (filepath);
	global_attributes = buildGameState (in, NULL, NULL, 0, NULL);
	prevents_segfaults = makeProcessingState ();
}

void import_file (struct charbuffer* in) {
	global_attributes = buildGameState (in, NULL, NULL, 0, NULL);
	add (global_attributes->moves, 0);
	char* move_data = global_attributes->moves->data;
	int i;
	int start_index = 0;
	for (i = 0; i < global_attributes->moves->length; i++) {
		if ((move_data[i] == ' ' || move_data[i] == 0) && i != global_attributes->moves->length - 1) {
			move_data[i] = 0;
			add_move (move_data + start_index);
			start_index = i + 1;
		}
	}
}	

void update_board () {
	struct gameState* gstate;
	if (!global_attributes) {
		return;
	}
	if (selected_move == 0) {
		gstate = global_attributes;
	} else {
		movelist = (struct gameState**)(movebuffer->data);
		gstate = movelist[selected_move - 1];
	}
	update_world (globals()->world, gstate);
}

void setPixel (int x, int y, int color) {
	if (x < 0 || y < 0 || x >= canvas_width || y >= canvas_height * 2) {
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

struct tb_cell* getCell (int x, int y) {
	struct tb_cell* cells = tb_cell_buffer ();
	struct tb_cell* current = cells + (x + (y >> 1) * tb_width ());
	return current;
}

void draw_move (struct charbuffer* move, int x, int y, int fg, int bg, int cursor) {
	int i;
	int cx = x;
	if (move->length == 0) {
		draw_char (' ', x, y, fg, bg);
	}
	for (i = 0; i < move->length; i++) {
		if (i > max_display_length * 2 - 1) {
			draw_str ("...", cx, y, fg, bg);
			return;
		}
		if (i % 2 == 0 && i != 0) {
			draw_char ('-', cx++, y, fg, bg);
			draw_char ('>', cx++, y, fg, bg);
		}
		if (cursor != 0 && i == cursor) {
			draw_char (move->data[i], cx, y, bg, fg);
		} else {
			draw_char (move->data[i], cx, y, fg, bg);
		}
		cx++;
	}
}

void draw_keys () {
	int key_space;
	int spaces[] = {9, 10};
	key_space = spaces[view_mode];
	column_start_y = key_space + 1;
	int wx, wy;
	for (int wx = 0; wx < screen_x; wx++) {
		for (int wy = 1; wy < column_start_y; wy++) {
			draw_char (' ', wx, wy, 0, 0);	
		}
	}
	if (view_mode) {
		draw_str ("CONTROLS:", 2, 2, 0, 0);
		draw_str ("(DON'T HOLD KEYS)", 2, 3, 0, 0);
		draw_str ("ARROW KEYS: MOVE/LOOK", 2, 4, 0, 0);
		draw_str ("R/F: MOVE UP/DOWN", 2, 5, 0, 0);
		draw_str ("W/S: LOOK UP/DOWN", 2, 6, 0, 0);
		draw_str ("M: TOGGLE VIEW", 2, 7, 0, 0);
		draw_str ("V: EDIT MODE", 2, 8, 0, 0);
		draw_str ("ESC TO QUIT", 2, 9, 0, 0);
	} else {
		draw_str ("CONTROLS:", 2, 2, 0, 0);
		draw_str ("(DON'T HOLD KEYS)", 2, 3, 0, 0);
		draw_str ("ARROW KEYS: SELECT", 2, 4, 0, 0);
		draw_str ("A-H,1-8,DEL: EDIT MOVE", 2, 5, 0, 0);
		draw_str ("S: SAVE FILE", 2, 6, 0, 0);
		draw_str ("V: VIEW MODE", 2, 7, 0, 0);
		draw_str ("ESC TO QUIT", 2, 8, 0, 0);
		//draw_str ("O: OPEN FILE", 2, 6, 0, 0);
	}
}

void refresh_moves () {
	resize_columns ();
	draw_filename ();
	if (selected_move < 0) {
		selected_move = 0;
	}
	if (selected_move > num_moves) {
		selected_move = num_moves;
	}

	move_column_height = tb_height () - column_start_y - 1;
	move_column_y = selected_move / 2 - move_column_height + 1;

	if (move_column_y < 0) {
		move_column_y = 0;
	}

	int wx = 0;
	int wy = 0;
	for (wx = 0; wx < screen_x; wx++) {
		for (wy = column_start_y; wy < tb_height (); wy++) {
			draw_char (' ', wx, wy, 0, 0);
		}
	}
	if (!global_attributes || !global_attributes->red_turn) {
		draw_str ("Red:", padding_left, column_start_y, 0, 0);
		draw_str ("Black:", column_x[1], column_start_y, 0, 0);
	} else {
		draw_str ("Black:", padding_left, column_start_y, 0, 0);
		draw_str ("Red:", column_x[1], column_start_y, 0, 0);
	}
	int i;
	int fg = COLOR_VALID;
	int bg = 0;
	int cfg;
	int cbg;
	int cs = 0;
	if (movebuffer) {
		movelist = (struct gameState**)(movebuffer->data);
		for (i = move_column_y * 2; i < move_column_y * 2 + move_column_height * 2 && i < num_moves + 1; i++) {
			wy = (i - move_column_y * 2) / 2 + column_start_y + 1;
			//printf ("%d\n",movelist[i]);
			if (i < num_moves) {
				if (i > 0 && !(i > 0 && movelist[i-1]->moves_legal)) {
					fg = COLOR_UNKNOWN;	
				} else if (!(movelist[i]->moves_legal)) {
					fg = COLOR_ERR;
				}
				if (i == selected_move) {
					cbg = fg;
					cfg = 0;
					if (fg == COLOR_UNKNOWN) {
						cfg = 232;
						cbg = 255;
					}
					cs = move_cursor;
				} else {
					cbg = bg;
					cfg = fg;
					cs = 0;
				}
			}
			if (i == num_moves) {
				if (selected_move == num_moves) {
					draw_char (' ', column_x[i % 2], wy, 0, 232);
				}
			} else {
				draw_move (movelist[i]->moves, column_x[i % 2], wy, cfg, cbg, cs);
			}
		}
	}
	struct charbuffer* progress = makebuffer ();
	int qu, rm;
	qu = num_moves;
	while (qu != 0) {
		rm = qu % 10;
		qu /= 10;
		add (progress, '0' + rm);
	}
	addstr (progress, " / ");
	qu = selected_move + 1;
	while (qu != 0) {
		rm = qu % 10;
		qu /= 10;
		add (progress, '0' + rm);
	}
	char* score_buffer = malloc (sizeof (char) * progress->length + 1);
	for (qu = 0; qu < progress->length; qu++) {
		score_buffer[qu] = progress->data[progress->length - 1 - qu];
	}
	score_buffer[progress->length] = 0;
	for (i = 0; i < screen_x; i++) {
		draw_char (' ', i, 0, 0, 0);
	}
	if (movebuffer) {
		draw_str ("MOVE # ", 0, 0, 0, 0);
		draw_str (score_buffer, 8, 0, 0, 0);
	} else {
		draw_str ("NO FILE LOADED", 0, 0, 0, 0);
	}
}

void resize_columns () {
	column_widths[0] = 2;
	column_widths[1] = 2;
	int i, j;
	if (movebuffer) {
		movelist = (struct gameState**)(movebuffer->data);
		for (i = 0; i < num_moves; i++) {
			int len = movelist[i]->moves->length / 2;
			if (len > max_display_length - 1) {
				len = max_display_length + 1;
			}
			if (len > column_widths[i % 2]) {
				column_widths[i % 2] = len;
			}
		}
	}
	column_x[0] = padding_left;
	column_x[1] = padding_left + (column_widths[0] * 2 - 1) * 2 + padding_center;
	
	//Remove the old 3d window
	//cell.ch = ' ';
	/*for (i = d3d_window_x; i < d3d_window_width; i++) {
		for (j = d3d_window_y; j < d3d_window_height / 2; j++) {
			cell.fg = 0;
			cell.bg = 0;
			tb_put_cell (i, j, &cell);
		}
	}*/

	//Resize the 3d window
	screen_x = column_x[1] + (column_widths[1] * 2 - 1) * 2 + padding_right;
	tb_utf8_char_to_unicode (&cell.ch, "▀");

	canvas_width = tb_width ();
	canvas_height = tb_height () * 2;
	view_window_x = screen_x;
	view_window_y = 2;
	view_window_width = tb_width () - screen_x;
	view_window_height = tb_height () * 2 - 2;
	
	for (i = view_window_x; i < view_window_width; i++) {
		for (j = view_window_y; j < view_window_height / 2; j++) {
			cell.fg = 0; //Black-ish
			cell.bg = 0; //Same
			tb_put_cell (i, j, &cell);
		}
	}
}

int canvasWidth () {
	return view_window_width;
}

int canvasHeight () {
	return view_window_height;
}

int canvasX () {
	return view_window_x;
}

int canvasY () {
	return view_window_y;
}

void draw_str (char* str, int x, int y, int fg, int bg) {
	int i = 0;
	while (str[i]) {
		tb_change_cell (x+i, y, str[i], fg, bg);
		i++;
	}
}

void draw_char (char c, int x, int y, int fg, int bg) {
	tb_change_cell (x, y, c, fg, bg);
}

void draw_filename () {
	int i;
	for (i = screen_x; i < tb_width (); i++) {
		draw_char (' ', i, 0, 0, 0);
	}
	for (i = 0; i < canvas_filename->length; i++) {
		draw_char (canvas_filename->data[i], screen_x + i, 0, 0, 0);
	}
}

void copy_state (struct gameState* src, struct gameState* dest) {
	dest->boardState = copyBoard (src->boardState);
	//Board char array doesn't matter
	dest->flipped = src->flipped;
	dest->capture = src->capture;
	dest->multiple_jumps = src->multiple_jumps;
	dest->red_turn = src->red_turn;
	dest->original_turn = src->original_turn;
	dest->num_allowed_moves = src->num_allowed_moves;
	dest->total_allowed_moves = src->total_allowed_moves;
	dest->remaining_moves = 0;
	dest->moves = 0;
	dest->move_positions = 0;
	dest->moves_legal = src->moves_legal;
}

void add_move (char* move) {
	if (!movebuffer) {
		//Make the buffer
		movebuffer = makebuffer ();
	}
	struct gameState** new_block = (struct gameState**)(add_block (movebuffer, sizeof (struct gameState*)));
	struct gameState* new_board = makeGameState ();
	*new_block = new_board;
	movelist = (struct gameState**)(movebuffer->data);
	if (num_moves) {
		copy_state (movelist[num_moves - 1], new_board);
		new_board->red_turn = !(new_board->red_turn);
	} else {
		copy_state (global_attributes, new_board);
		movelist[0] = new_board;
	}
	if (/*num_moves*/1) {
		//doMove (movelist[num_moves - 1]->moves->data, new_board, prevents_segfaults);
		if (new_board->moves_legal || num_moves == 0) {
			new_board->moves_legal = !doMove (move, new_board, prevents_segfaults);
		}
	}
	num_moves++;
	struct charbuffer* move_buffer = makebuffer ();
	addstr (move_buffer, move);
	//add (move_buffer, 0);
	new_board->moves = move_buffer;
}

void evaluate_moves () {
	int i = 0;
	struct gameState* prev;
	struct gameState* curr;
	for (i = 0; i < num_moves; i++) {
		movelist = (struct gameState**)(movebuffer->data);
		if (i == 0) {
			prev = global_attributes;
		} else {
			prev = movelist[i - 1];
		}
		curr = movelist[i];
		struct charbuffer* temp = curr->moves;
		struct charbuffer* used_move = makebuffer ();
		addstr (used_move, temp->data);
		add (used_move, 0);
		copy_state (prev, curr);
		if (i != 0) {
			curr->red_turn = !(curr->red_turn);
		}
		if (curr->moves_legal || i == 0) {
			curr->moves_legal = !doMove (used_move->data, curr, prevents_segfaults);
		}
		curr->moves = temp;
	}
}

void write_move (struct charbuffer* out, struct charbuffer* in) {
	int i;
	for (i = 0; i < in->length; i++) {
		if (i % 2 == 0 && i != 0) {
			addstr (out, "->");
		}
		add (out, in->data[i]);
	}
}

void canvas_save_file () {
	global_attributes->moves->length = 0;
	int i, j;
	struct charbuffer* curr;
	struct charbuffer* current_move = makebuffer ();
	movelist = (struct gameState**)(movebuffer->data);
	for (i = 0; i < num_moves; i++) {
		curr = movelist[i]->moves;
		for (j = 0; j < curr->length; j++) {
			add (current_move, curr->data[j]);
		}
		write_move (global_attributes->moves, current_move);
		add (global_attributes->moves, ' ');
		current_move->length = 0;
	}
	add (global_attributes->moves, 0);
	struct charbuffer* hfile = makebuffer ();
	struct charbuffer* outname = makebuffer ();
	struct charbuffer* in = getFile (canvas_filename->data);
	addstr (outname, "out_");
	addstr (outname, canvas_filename->data);
	add (outname, 0);
	in->length = global_attributes->move_positions->data;
	addstr (in, global_attributes->moves->data);
	add (in, 0);
	writeToFile (in, outname->data);
}

/*
struct gameState {
	struct board* boardState;
	char board[64];
	int flipped;
	int capture;
	int multiple_jumps;
	int red_turn;
	int original_turn;
	int num_allowed_moves;
	int total_allowed_moves;
	struct charbuffer* remaining_moves;
	struct charbuffer* moves;
	struct node* move_positions;
	int token_positions[4];
	int turn_text_position;
	int moves_legal;
	int num_moves_processed;
};
*/
