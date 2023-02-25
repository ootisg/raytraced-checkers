#ifndef loadfuncs
#define loadfuncs

#include "charbuffer.h"

struct gameState;
struct procst;

#include "movefuncs.h"
#include "outfuncs.h"

struct node {
	int data;
	struct node* next;
};

struct funcmap {
	void (*map[128])(char, struct gameState*, struct procst*);
};

struct basicmaps {
	struct funcmap* ERRMAP_FULL;
	struct funcmap* ERRMAP_WHITESPACE;
	struct funcmap* IGNOREMAP_FULL;
};

struct stringlist {
	char** strings;
	int length;
	int fill;
	int count;
};

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

struct procst {
	struct funcmap* defaultmap;
	struct funcmap* currentmap;
	struct funcmap* commentmap;
	struct funcmap* searchmap;
	struct stringlist* searches;
	struct charbuffer* moves;
	struct charbuffer* currentmove;
	char* foundterm;
	char* errcontext;
	int line;
	int linepos;
	int position;
	int scanpos;
	int numMoveSpaces;
	int boardpos;
	int stage;
	int error;
	int num_moves_processed;
	int num_moves_found;
	int move_processing_done;
};

void startComment (char c, struct gameState* game, struct procst* state);
void addToMoves (char c, struct gameState* game, struct procst* state);
void moveSep (char c, struct gameState* game, struct procst* state);
void endMove (char c, struct gameState* game, struct procst* state);
struct gameState* makeGameState ();
struct procst* makeProcessingState ();
struct gameState* buildGameState (struct charbuffer* data, struct charbuffer* eFile, struct charbuffer* hFile, int num_moves, void(*display_function)(struct charbuffer*, struct gameState*, struct procst*, int num_moves));

#endif
