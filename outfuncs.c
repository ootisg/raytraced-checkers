#include <stdio.h>

#include "outfuncs.h"

char* FILE_MARKINGS = "    a   b   c   d   e   f   g   h  \n";
char* ROW_SEPERATOR = "  +---+---+---+---+---+---+---+---+\n";
char* ROW_START = "| ";
char* ROW_MID = " | ";

void outputH (struct gameState* state, struct charbuffer* out, char* filename) {
	if (out) {
	addstr (out, FILE_MARKINGS);
	addstr (out, ROW_SEPERATOR);
	int row;
	int col;
	for (row = 0; row < 8; row++) {
		add (out, (char)('8' - row));
		add (out, ' ');
		addstr (out, ROW_START);
		for (col = 0; col < 8; col++) {
			struct square* cell = getSquare (state->boardState, col, row);
			add (out, cell->char_rep);
			addstr (out, ROW_MID);
		}
		add (out, (char)('8' - row));
		add (out, '\n');
		addstr (out, ROW_SEPERATOR);
	}
	addstr (out, FILE_MARKINGS);
	add (out, '\n');
	struct charbuffer* r_moves = state->remaining_moves;
	int i;
	int m = 1;
	int p = 0;
	for (i = 0; m < state->num_moves_processed + 1 && i < state->moves->length; i++) {
		if (state->moves->data[i] == ' ') m++;
	}
	for (; i < state->moves->length; i++) {
		if (state->moves->data[i] == ' ') {
			add (out, '\n');
			p = 0;
		} else {
			add (out, state->moves->data[i]);
			p++;
			if (!(p % 2) && i + 1 < state->moves->length && state->moves->data[i + 1] != ' ') {
				addstr (out, "->");
			}
		}
	}
	add (r_moves, 0);
	add (out, 0);
	writeToFile (out, filename);
	}
}

void outputE (struct gameState* state, struct charbuffer* in, struct charbuffer* out, char* filename) {
	if (out) {
		int comment_mode;
		int i;
		int board_index = 0;
		for (i = 0; i < state->turn_text_position; i++) {
			add (out, in->data[i]);
		}
		if (state->original_turn) {
			i += 3;
		} else {
			i += 5;
		}
		if (state->red_turn) {
			addstr (out, "red");
		} else {
			addstr (out, "black");
		}
		for (; i < state->token_positions[2]; i++) {
			add (out, in->data[i]);
		}
		while (i < state->token_positions[3]) {
			if (in->data[i] == '#' && !comment_mode) {
				comment_mode = 1;
			} else if (comment_mode && in->data[i] == '\n') {
				comment_mode = 0;
			}
			if (!comment_mode && (in->data[i] == '"' || in->data[i] == '.' || in->data[i] == 'R' || in->data[i] == 'r' || in->data[i] == 'B' || in->data[i] == 'b')) {
				int col = board_index % 8;
				int row = board_index / 8;
				add (out, getSquare (state->boardState, col, row)->char_rep);
				board_index++;
			} else {
				add (out, in->data[i]);
			}
			i++;
		}
		if (state->move_positions) {
			for (; i < state->move_positions->data; i++) {
				add (out, in->data[i]);
			}
			struct node* working = state->move_positions;
			i = 0;
			while (working->next && i < state->num_moves_processed) {
				working = working->next;
				i++;
			}
			if (!working->next && i == state->num_moves_processed - 1) {
				i = in->length;
			} else {
				i = working->data;
			}
			while (i < in->length) {
				add (out, in->data[i]);
				i++;
			}
			add (out, 0);
			writeToFile (out, filename);
		}
	}
}

void writeToFile (struct charbuffer* out, char* filename) {
	FILE* fp = fopen (filename, "w");
	fprintf (fp, out->data);
	fclose (fp);
}
