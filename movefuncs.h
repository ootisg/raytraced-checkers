#ifndef movefuncs
#define movefuncs

#include "loadfuncs.h"

struct square {
	char rank; //Vertical, 0 at bottom
	char file; //Horizontal, a at left
	int space_color; //Red is 0, Black is 1
	int piece_color; //Red is 0, Black is 1
	int piece_type; //Pawn is 0, King is 1
	int empty; //1 for an empty space, 0 for an occupied space
	char char_rep; //Character representation of the square, for output purposes
};

struct board {
	struct square squares[64];
	int flipped;
};

int doMove (char* move, struct gameState* gstate, struct procst* pstate);
struct board* makeBoard (struct gameState* gstate);
struct square* getSquare (struct board* board, int col, int row);
struct square* copySquare (struct square* src, struct square* dest);
struct board* copyBoard (struct board* src);

#endif
