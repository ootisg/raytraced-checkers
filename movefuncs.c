#include <stdlib.h>
#include <stdio.h>

#include "loadfuncs.h"
#include "board_constants.h"

struct board* makeBoard (struct gameState* gstate) {
	struct board* usedBoard;
	if (usedBoard = malloc (sizeof (struct board))) {
		usedBoard->flipped = gstate->flipped;
		int i;
		for (i = 0; i < 64; i++) {
			char space = gstate->board[i];
			struct square* csquare = &(usedBoard->squares[i]);
			csquare->rank = 7 - (i / 8) + '1';
			csquare->file = i % 8 + 1 + 'a';	
			switch (space) {
				case '\"':
					csquare->empty = 1;
					csquare->space_color = SQUARE_COLOR_RED;
					csquare->char_rep = '\"';
					break;
				case '.':
					csquare->empty = 1;
					csquare->space_color = SQUARE_COLOR_BLACK;
					csquare->char_rep = '.';
					break;
				case 'b':
					csquare->empty = 0;
					csquare->space_color = SQUARE_COLOR_BLACK;
					csquare->piece_color = SQUARE_COLOR_BLACK;
					csquare->piece_type = PIECE_TYPE_PAWN;
					csquare->char_rep = 'b';
					break;
				case 'B':
					csquare->empty = 0;
					csquare->space_color = SQUARE_COLOR_BLACK;
					csquare->piece_color = SQUARE_COLOR_BLACK;
					csquare->piece_type = PIECE_TYPE_KING;
					csquare->char_rep = 'B';
					break;
				case 'r':
					csquare->empty = 0;
					csquare->space_color = SQUARE_COLOR_BLACK;
					csquare->piece_color = SQUARE_COLOR_RED;
					csquare->piece_type = PIECE_TYPE_PAWN;
					csquare->char_rep = 'r';
					break;
				case 'R':
					csquare->empty = 0;
					csquare->space_color = SQUARE_COLOR_BLACK;
					csquare->piece_color = SQUARE_COLOR_RED;
					csquare->piece_type = PIECE_TYPE_KING;
					csquare->char_rep = 'R';
					break;
				default:
					break;
			}
		}
	}
	return usedBoard;
}

struct square* getSquare (struct board* board, int col, int row) {
	if (col < 0 || row < 0 || col >= 8 || row >= 8) {
		return NULL;
	} else {
		return &(board->squares[row * 8 + col]);
	}
}

int canJump (int col, int row, struct gameState* gstate, struct procst* pstate) {
	int direction;
	struct board* used_board = gstate->boardState;
	struct square* s0 = getSquare (used_board, col, row);
	if (getSquare (gstate->boardState, col, row)->empty) {
		return 0;
	}
	if ((gstate->red_turn && s0->piece_color == SQUARE_COLOR_BLACK) || (!gstate->red_turn && s0->piece_color == SQUARE_COLOR_RED)) {
		return 0;
	}
	for (direction = 0; direction < 4; direction++) {
		int xstep = direction % 2;
		int ystep = direction / 2;
		if (s0->piece_type == PIECE_TYPE_PAWN) {
			if (s0->piece_color == SQUARE_COLOR_RED) {
				ystep = 0;
			} else if (s0->piece_color == SQUARE_COLOR_BLACK) {
				ystep = 1;
			}
		}
		if (!xstep) xstep = -1;
		if (!ystep) ystep = -1;
		struct square* s1 = getSquare (used_board, col + xstep, row + ystep);
		struct square* s2 = getSquare (used_board, col + xstep * 2, row + ystep * 2);
		if (s1 && !s1->empty && s2 && s2->empty) {
			if (s0->piece_color != s1->piece_color) {
				return 1;
			}
		}
	}
	return 0;
}

int canDoJump (struct gameState* gstate, struct procst* pstate) {
	int i = 0;
	for (i = 0; i < 64; i++) {
		if (canJump (i % 8, i / 8, gstate, pstate)) return 1;
	}
	return 0;
}

int validJump (int col1, int row1, int col2, int row2, struct gameState* gstate, struct procst* pstate) {
	//Prereq: col1, row1 and col2, row2 must be 2 apart
	int imx = (col1 + col2) / 2;
	int imy = (row1 + row2) / 2;
	struct board* used_board = gstate->boardState;
	struct square* start = getSquare (used_board, col1, row1);
	struct square* interm = getSquare (used_board, imx, imy);
	struct square* land = getSquare (used_board, col2, row2);
	if (!start->empty && !interm->empty && land->empty && start->piece_color != interm->piece_color) {
		return 1;
	}
	return 0;
}

int abs (int a) {
	if (a < 0) {
		return a * -1;	
	}
	return a;
}

void empty (struct square* space) {
	space->empty = 1;
	if (space->space_color == SQUARE_COLOR_BLACK) {
		space->char_rep = '.';
	} else {
		space->char_rep = '\"';
	}
}

void capture (struct square* space, struct gameState* gstate) {
	empty (space);
}

void movePiece (struct square* start, struct square* end, struct square* jumped, struct gameState* gstate, struct procst* pstate) {
	//Prereq: move is valid
	int new_type = start->piece_type;
	if (start->piece_type == PIECE_TYPE_PAWN) {
		if (start->piece_color == SQUARE_COLOR_BLACK && end->rank == '1') {
			new_type = PIECE_TYPE_KING;
			start->char_rep = 'B';
		} else if (start->piece_color == SQUARE_COLOR_RED && end->rank == '8') {
			new_type = PIECE_TYPE_KING;
			start->char_rep = 'R';
		}
	}
	if (jumped) {
		capture (jumped, gstate);
	}
	end->empty = 0;
	end->piece_type = new_type;
	end->piece_color = start->piece_color;
	end->char_rep = start->char_rep;
	empty (start);
}

int doMove (char* move, struct gameState* gstate, struct procst* pstate) {
	//RETURN CODES
	//-1 is an error
	//0 is a successfully parsed move
	//1 is an end of move parsing (due to limited number of moves to process)
	#ifdef COMPILE_CHANGE
	if (pstate && (gstate->num_allowed_moves == 0 || pstate->move_processing_done)) {
		pstate->move_processing_done = 1;
		return 1;
	}
	#endif
	int jumpPossible = canDoJump (gstate, pstate);
	struct square* start = getSquare (gstate->boardState, (int)(move[0] - 'a'), (int)(7 - (move[1] - '1')));
	if (start->empty) return -1;
	if (start->piece_color == SQUARE_COLOR_RED && !gstate->red_turn) return -1;
	if (start->piece_color == SQUARE_COLOR_BLACK && gstate->red_turn) return -1;
	int i = 2;
	struct square* last;
	struct square* current;
	while (move[i]) {
		if (!gstate->multiple_jumps && i >= 4) return -1;
		int lastCoords[2] = {(int)(move[i - 2] - 'a'), (int)(7 - (move[i - 1] - '1'))};
		int currentCoords[2] = {(int)(move[i] - 'a'), (int)(7 - (move[i + 1] - '1'))};
		last = getSquare (gstate->boardState, lastCoords[0], lastCoords[1]);
		current = getSquare (gstate->boardState, currentCoords[0], currentCoords[1]);
		int xstep = abs (currentCoords[0] - lastCoords[0]);
		int ystep = abs (currentCoords[1] - lastCoords[1]);
		if (xstep != ystep) return -1;
		if (xstep == 0 || xstep > 2) return -1;
		if (xstep == 1 && i >= 4) return -1;
		if (xstep < 2 && gstate->capture && jumpPossible) return -1;
		if (last->piece_type == PIECE_TYPE_PAWN) {
			if (last->piece_color == SQUARE_COLOR_RED && currentCoords[1] > lastCoords[1]) {
				return -1;
			}
			if (last->piece_color == SQUARE_COLOR_BLACK && currentCoords[1] < lastCoords[1]) {
				return -1;
			}
		}
		if (xstep == 1) {
			if (!current->empty) {
				return -1;
			} else {
				movePiece (last, current, NULL, gstate, pstate);
			}
		}
		if (xstep == 2) {
			if (!validJump (lastCoords[0], lastCoords[1], currentCoords[0], currentCoords[1], gstate, pstate)) return -1;
			movePiece (last, current, getSquare (gstate->boardState, (lastCoords [0] + currentCoords [0]) / 2, (lastCoords [1] + currentCoords [1]) / 2), gstate, pstate);
			jumpPossible = canDoJump (gstate, pstate);
			if (gstate->capture && jumpPossible && !move[i + 2]) return -1;
		}
		i += 2;
	}
	if (gstate->num_allowed_moves != -1) (gstate->num_allowed_moves)--;
	return 0;
}

struct square* copySquare (struct square* src, struct square* dest) {
	dest->rank = src->rank;
	dest->file = src->file;
	dest->space_color = src->space_color;
	dest->piece_color = src->piece_color;
	dest->piece_type = src->piece_type;
	dest->empty = src->empty;
	dest->char_rep = src->char_rep;
	return dest;
}

struct board* copyBoard (struct board* src) {
	struct board* dest = malloc (sizeof (struct board));
	dest->flipped = src->flipped;
	int i;
	for (i = 0; i < 64; i++) {
		copySquare (&(src->squares[i]), &(dest->squares[i]));
	}
	return dest;
}
