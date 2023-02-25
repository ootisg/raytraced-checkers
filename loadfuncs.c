#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadfuncs.h"

struct basicmaps* BASIC_MAPS;

void error (char c, struct gameState* game, struct procst* state) {
	fprintf (stderr, "Formatting error while %s: unexpected character %c encountered at line %d, position %d.\n", state->errcontext, c, state->line, state->linepos);
	state->error = 1;
}

void extError (char* error, struct procst* state) {
	fprintf (stderr, error, state->line);
	state->error = 1;
}

void ignore (char c, struct gameState* game, struct procst* state) {

}

struct stringlist* makeStringlist (int size) {
	struct stringlist* ptr;
	ptr = malloc (sizeof (struct stringlist));
	ptr->strings = malloc (sizeof (char*) * size);
	ptr->length = size;
	ptr->fill = 0;
	ptr->count = 0;
	return ptr;
}

int addString (struct stringlist* list, char* item) {
	//Return value of 0 indicates the item was added; 1 indicates the stringlist was already full.
	if (list->fill < list->length) {
		list->strings[list->fill] = item;
		list->fill++;
		list->count++;
		return 0;
	}
	return 1;
}

struct funcmap* makeFunctionMap () {
	struct funcmap* ptr;
	ptr = malloc (sizeof (struct funcmap));
	int i;
	for (i = 0; i < 128; i ++) {
		ptr->map[i] = error;
	}
	return ptr;
}

void fillFunctionMap (struct funcmap* map, void* function) {
	int i;
	for (i = 0; i < 128; i++) {
		map->map[i] = function;
	}
}

struct gameState* makeGameState () {
	struct gameState* ptr;
	ptr = malloc (sizeof (struct gameState));
	ptr->flipped = -1;
	ptr->capture = -1;
	ptr->multiple_jumps = -1;
	ptr->red_turn = -1;
	ptr->num_allowed_moves = -1;
	ptr->remaining_moves = makebuffer ();
	ptr->move_positions = NULL;
	ptr->moves_legal = 1;
	ptr->num_moves_processed = 0;
	return ptr;
}

struct procst* makeProcessingState () {
	struct procst* ptr;
	ptr = malloc (sizeof (struct procst));
	ptr->searches = malloc (sizeof (struct stringlist));
	ptr->defaultmap = makeFunctionMap ();
	ptr->currentmap = makeFunctionMap ();
	ptr->commentmap = makeFunctionMap ();
	ptr->searchmap = makeFunctionMap ();
	ptr->currentmove = makebuffer ();
	ptr->moves = makebuffer ();
	ptr->position = 0;
	ptr->scanpos = 0;
	ptr->line = 0;
	ptr->linepos = 0;
	ptr->numMoveSpaces = 0;
	ptr->boardpos = 0;
	ptr->stage = 0;
	ptr->error = 0;
	ptr->num_moves_processed = 0;
	ptr->num_moves_found = 0;
	ptr->move_processing_done = 0;
	return ptr;
}

void copyFunctionMap (struct funcmap* src, struct funcmap* dest) {
	int i;
	for (i = 0; i < 128; i++) {
		dest->map[i] = src->map[i];
	}
}

void setFuncMapChars (struct funcmap* map, char* chars, void* function) {
	int i = 0;
	for (i = 0; chars[i]; i++) {
		map->map[chars[i]] = function;
	}
}

void setComment (struct funcmap* map) {
	setFuncMapChars (map, "#", startComment);
}

void setWhitespace (struct funcmap* map, void* function) {
	setFuncMapChars (map, " \t\n\r", function);
}

struct basicmaps* makeBasicMaps () {
	struct basicmaps* ptr = malloc (sizeof (struct basicmaps));
	ptr->ERRMAP_FULL = makeFunctionMap ();
	setComment (ptr->ERRMAP_FULL);
	ptr->ERRMAP_WHITESPACE = makeFunctionMap ();
	setWhitespace (ptr->ERRMAP_WHITESPACE, ignore);
	setComment (ptr->ERRMAP_WHITESPACE);
	ptr->IGNOREMAP_FULL = makeFunctionMap ();
	fillFunctionMap (ptr->IGNOREMAP_FULL, ignore);
	setComment (ptr->IGNOREMAP_FULL);
}

/*
 * Functions here are for funcmap
 */

void endComment (char c, struct gameState* game, struct procst* state) {
	copyFunctionMap (state->commentmap, state->currentmap);
}

void startComment (char c, struct gameState* game, struct procst* state) {
	copyFunctionMap (state->currentmap, state->commentmap);
	fillFunctionMap (state->currentmap, ignore);
	setFuncMapChars (state->currentmap, "\n", endComment);
}

void endSearch (struct procst* state) {
	copyFunctionMap (state->searchmap, state->currentmap);
}

void checkTerms (char c, struct gameState* game, struct procst* state) {
	struct stringlist* searches = state->searches;
	if (state->scanpos != 0) {
		int i;
		for (i = 0; i < searches->length; i++) {
			if (searches->strings[i]) {
				state->currentmap->map[searches->strings[i][state->scanpos]] = error;
				if (searches->strings[i][state->scanpos] != c) {
					searches->strings[i] = NULL;
					searches->count--;
				}
			}
		}
	}
	if (searches->count == 0) {
		return;
	}
	state->scanpos++;
	int i;
	for (i = 0; i < searches->length; i++) {
		if (searches->strings[i]) {
			if (searches->strings[i][state->scanpos]) {
				state->currentmap->map[searches->strings[i][state->scanpos]] = checkTerms;
			} else {
				state->foundterm = searches->strings[i];
				endSearch (state);
			}
		}
	}
}

void beginSearch (char c, struct gameState* game, struct procst* state) {
	copyFunctionMap (state->currentmap, state->searchmap);
	fillFunctionMap (state->currentmap, error);
	setFuncMapChars (state->currentmap, "#", startComment);
	checkTerms (c, game, state);
}

void addToBoard (char c, struct gameState* game, struct procst* state) {
	//Gimme mah extra credit boiiiiiii
	if (!state->boardpos) {
		if (c == 'b' || c == 'B' || c == 'r' || c == 'R' || c == '.') {
			game->flipped = 1;
		} else if (c == '\"') {
			game->flipped = 0;
		}
	}
	int polarity = (state->boardpos % 2 + (state->boardpos / 8) % 2) % 2;
	//Oh yeah down here does some extra credit-y things too
	if (game->flipped) {
		polarity = !polarity;
	}
	//And that's all the extra credit took. I hope I get like 200 points for this, but I doubt I'll get more than like 1.
	if (!polarity && (c == 'b' || c == 'B' || c == 'r' || c == 'R')) {
		extError ("Board error at line %d: a piece is on a red square.\n", state);
	} else if (!polarity && (c == '.')) {
		extError ("Board error at line %d: expecting a red square, but found a black square.\n", state);
	} else if (polarity && (c == '\"')) {
		extError ("Board error at line %d: expecting a black square, but found a red square.\n", state);
	} else {
		game->board[state->boardpos] = c;
	}
	state->boardpos++;
}

void addToMoves (char c, struct gameState* game, struct procst* state) {
	struct charbuffer* buffer = state->moves;
	struct charbuffer* cbuffer = state->currentmove;
	if (c >= 'a' && c <= 'h') {
		if (buffer->length == 0 || buffer->data[buffer->length - 1] == ' ') {
			//Dawn of a new day
			struct node* newNode;
			newNode = malloc (sizeof (struct node));
			newNode->data = state->position; 
			if (newNode) {
				if (!game->move_positions) {
					game->move_positions = newNode;
				} else {
					struct node* workingNode = game->move_positions;
					while (workingNode->next) workingNode = workingNode->next;
					workingNode->next = newNode;
				}
			}
		}
		if (buffer->length == 0 || buffer->data[buffer->length - 1] == ' ' || (buffer->data[buffer->length - 1] >= '1' && buffer->data[buffer->length - 1] <= '8')) {
			if (buffer->length != 0 && cbuffer->data[cbuffer->length - 1] == 0) {
				cbuffer->length = 0;
			}
			add (buffer, c);
			add (cbuffer, c);
			setFuncMapChars (state->currentmap, "-", error);
			setWhitespace (state->currentmap, error);
		} else {
			error (c, game, state);
		}
	} else if (c >= '1' && c <= '8') {
		if (buffer->length == 0) {
			error (c, game, state);
		} else {
			if (buffer->data[buffer->length - 1] >= 'a' && buffer->data[buffer->length - 1] <= 'h') {
				add (buffer, c);
				add (cbuffer, c);
				setFuncMapChars (state->currentmap, "-", moveSep);
				setWhitespace (state->currentmap, endMove);
			} else {
				error (c, game, state);
			}
		}
	}
}

void moveSep (char c, struct gameState* game, struct procst* state) {
	if (c == '-') {
		setFuncMapChars (state->currentmap, "-", error);
		setFuncMapChars (state->currentmap, ">", moveSep);
		setFuncMapChars (state->currentmap, "abcdefgh12345678", error);
		setWhitespace (state->currentmap, error);
	} else if (c == '>') {
		setFuncMapChars (state->currentmap, ">", error);
		setFuncMapChars (state->currentmap, "abcdefgh12345678", addToMoves);
		state->numMoveSpaces++;
	}
}

void endMove (char c, struct gameState* game, struct procst* state) {
	state->num_moves_found++;
	add (state->moves, ' ');
	add (state->currentmove, 0);
	int status = -1;
	int was_legal = game->moves_legal;
	#ifdef COMPILE_CHANGE
	if (game->moves_legal && !state->move_processing_done) status = doMove (state->currentmove->data, game, state);
	#endif
	if (game->moves_legal && state->move_processing_done) status = 1;
	if (status == -1) game->moves_legal = 0;
	int i = 0;
	if (status == 0) {
		state->num_moves_processed++;
	} else if (state->move_processing_done) {
		while (state->currentmove->data[i]) {
			add (game->remaining_moves, state->currentmove->data[i]);
			i++;
		}
		add (game->remaining_moves, ' ');
	}
	if (status == 0) game->red_turn = !game->red_turn;
	setFuncMapChars (state->currentmap, "-", error);
	setWhitespace (state->currentmap, ignore);
	state->numMoveSpaces = 0;
}

/*
 * Top level parsing functions
 */

void initSearch (struct procst* state) {
	struct stringlist* searches = state->searches;
	int i;
	for (i = 0; i < searches->length; i++) {
		if (searches->strings[i]) {
			state->currentmap->map[searches->strings[i][0]] = beginSearch;
		}
	}
}

int foundString (struct procst* state, char* string) {
	if (state->foundterm) {
		int i;
		for (i = 0; state->foundterm[i] == string[i]; i++) {
			if (!state->foundterm[i] && !string[i]) {
				return 1;
			}
		}
	}
	return 0;
}

void runChar (char c, struct gameState* game, struct procst* state) {
	if (c == '\n') {
		state->linepos = 0;
		state->line++;
	} else {
		state->linepos++;
	}
	state->currentmap->map[c] (c, game, state);
}

int doSearches (struct gameState* game, struct procst* state, struct charbuffer* data) {
	state->scanpos = 0;
	state->foundterm = NULL;
	initSearch (state);
	while (state->position < data->length && !state->error && !state->foundterm) {
		runChar (data->data[state->position], game, state);
		state->position++;
		if (state->error) {
			return 1;
		}
	}
	if (state->position >= data->length) {
	extError ("Unexpected EOF on line %d\n", state);
	}
	free (state->searches);
}

void change_output (struct charbuffer* data, struct gameState* gstate, struct procst* pstate, int num_moves) {
	printf ("Input file has %d moves\n", pstate->num_moves_found);
	printf ("Processing %d moves\n", pstate->num_moves_processed + !gstate->moves_legal);
	if (gstate->moves_legal) {
		printf ("All moves are legal\n");
	} else {
		int illegalMove = pstate->num_moves_processed + 1;
		int i = 0;
		int moveCount = 0;
		while (moveCount < pstate->num_moves_processed) {
			if (pstate->moves->data[i] == ' ') {
				moveCount++;
			}
			i++;
		}
		struct charbuffer* badmove = makebuffer ();
		int parity = 0;
		while (i < pstate->moves->length && pstate->moves->data[i] != ' ') {
			add (badmove, pstate->moves->data[i]);
			if (parity && i + 1 < pstate->moves->length && pstate->moves->data[i + 1] != ' ') {
				addstr (badmove, "->");
			}
			i++;
			parity = !parity;
		}
		add (badmove, 0);
		printf ("Move %d is illegal: %s\n", illegalMove, badmove->data);
	}
}

struct gameState* buildGameState (struct charbuffer* data, struct charbuffer* eFile, struct charbuffer* hFile, int num_moves, void(*display_function)(struct charbuffer*, struct gameState*, struct procst*, int num_moves)) {
	//Make processor and game states
	struct procst* pstate = makeProcessingState ();
	struct gameState* gstate = makeGameState ();
	gstate->num_allowed_moves = num_moves;
	gstate->total_allowed_moves = num_moves;

	//Ignore whitespace and comments
	setWhitespace (pstate->currentmap, ignore);
	setComment (pstate->currentmap);
	
	//Search for RULES token
	pstate->errcontext = "searching for RULES: token";
	pstate->position = 0;
	pstate->searches = makeStringlist (1);
	addString (pstate->searches, "RULES:");
	doSearches (gstate, pstate, data);
	if (!foundString (pstate, "RULES:")) {
		return NULL;
	}
	if (pstate->error) {return NULL;}
	gstate->token_positions[0] = pstate->position;

	//Parse rules
	pstate->errcontext = "parsing rules";
	int i;
	for (i = 0; i < 2; i++) {
		//Search for the tokens capture and no capture
		pstate->searches = makeStringlist (4);
		addString (pstate->searches, "capture");
		addString (pstate->searches, "no ");
		addString (pstate->searches, "single ");
		addString (pstate->searches, "multiple ");
		doSearches (gstate, pstate, data);
		if (foundString (pstate, "no ")) {
			//token no followed by one space has been found, search for the remainder of the token
			pstate->searches = makeStringlist (1);
			addString (pstate->searches, "capture");
			doSearches (gstate, pstate, data);
			if (foundString (pstate, "capture")) {
				if (gstate->capture == -1) {
					gstate->capture = 0;
				} else {
					//A big oof happened
					return NULL;
				}
			}
		} else if (foundString (pstate, "capture")) {
			//token capture has been found
			if (gstate->capture == -1) {
				gstate->capture = 1;
			} else {
				//A big oof happened
				return NULL;
			}
		} else if (foundString (pstate, "single ") || foundString (pstate, "multiple ")) {
			//token single or token multiple has been found
			int single = 0;
			if (foundString (pstate, "single ")) {
				single = 1;
			}
			pstate->searches = makeStringlist (1);
			addString (pstate->searches, "jumps");
			doSearches (gstate, pstate, data);
			if (foundString (pstate, "jumps")) {
				if (gstate->multiple_jumps != -1) {
					//A big oof happened
					return NULL;
				} else {
					if (single) {
						//Single jumps (laaame)
						gstate->multiple_jumps = 0;
					} else {
						//Multiple jumps!
						gstate->multiple_jumps = 1;
					}
				}
			}
		}
	}
	if (pstate->error) {return NULL;}
	
	//Search for TURN: token
	pstate->errcontext = "searching for TURN: token";
	pstate->searches = makeStringlist (1);
	addString (pstate->searches, "TURN:");
	doSearches (gstate, pstate, data);
	if (!foundString (pstate, "TURN:")) {
		return NULL;
	}
	if (pstate->error) {return NULL;}
	gstate->token_positions[1] = pstate->position;

	//Search and set turn
	pstate->errcontext = "parsing turn";
	pstate->searches = makeStringlist (2);
	addString (pstate->searches, "red");
	addString (pstate->searches, "black");
	doSearches (gstate, pstate, data);
	if (foundString (pstate, "red")) {
		gstate->red_turn = 1;
	} else if (foundString (pstate, "black")) {
		gstate->red_turn = 0;
	}
	if (pstate->error) {return NULL;}
	gstate->turn_text_position = pstate->position - 1;
	if (gstate->red_turn) {
		gstate->turn_text_position -= 2;
	} else {
		gstate->turn_text_position -= 4;
	}
	gstate->original_turn = gstate->red_turn;

	//Search for BOARD: token
	pstate->errcontext = "searching for BOARD: token";
	pstate->searches = makeStringlist (1);
	addString (pstate->searches, "BOARD:");
	doSearches (gstate, pstate, data);
	if (!foundString (pstate, "BOARD:")) {
		return NULL;
	}
	if (pstate->error) {return NULL;}
	gstate->token_positions[2] = pstate->position;

	//Board mode! Finally!
	//Here I'm adding a search for the token MOVES: which ends the board format
	pstate->errcontext = "parsing the board";
	pstate->searches = makeStringlist (1);
	addString (pstate->searches, "MOVES:");
	pstate->scanpos = 0;
	pstate->foundterm = NULL;
	initSearch (pstate);
	//Add a few characters to ignore, and a few to pay attention to
	setFuncMapChars (pstate->currentmap, "|-+", ignore);
	setFuncMapChars (pstate->currentmap, "\".bBrR", addToBoard);
	//Aaaaaand... time to iterate over the board!
	while (pstate->position < data->length && !foundString (pstate, "MOVES:")) {
		runChar (data->data[pstate->position], gstate, pstate);
		pstate->position++;
		if (pstate->boardpos > 64) {
			extError ("Board error on line %d: more spaces were defined than exist on the board\n", pstate);
			return NULL;
		}
		if (pstate->error) {return NULL;}
	}
	if (pstate->position >= data->length) {extError ("Unexpected EOF on line %d\n", pstate);}
	if (pstate->boardpos < 64) {
		extError ("Board error on line %d: end of board section found, but the entire board was not defined.", pstate);
	}
	if (pstate->error) {return NULL;}
	gstate->boardState = makeBoard (gstate);
	gstate->token_positions[3] = pstate->position;	

	//And now we check to see if we found the MOVES: token
	pstate->errcontext = "parsing moves";
	setFuncMapChars (pstate->currentmap, "|-+", error);
	setFuncMapChars (pstate->currentmap, "\".bBrR", error);
	setFuncMapChars (pstate->currentmap, "abcdefgh12345678", addToMoves);
	while (pstate->position < data->length) {
		runChar (data->data[pstate->position], gstate, pstate);
		pstate->position++;
		if (pstate->error) {return NULL;}
	}

	gstate->num_moves_processed = pstate->num_moves_processed;
	gstate->moves = pstate->moves;

	/*
	//Legacy utility for part 1
	printf ("VALID INPUT\n");
	printf ("Initial configuration:\n");
	if (gstate->red_turn) {
		printf ("Turn: red\n");
	} else {
		printf ("Turn: black\n");
	}
	int redPawns, redKings, blackPawns, blackKings;
	redPawns = redKings = blackPawns = blackKings = 0;
	for (i = 0; i < 64; i++) {
		char c = gstate->board[i];
		if (c == 'r') {
			redPawns++;
		} else if (c == 'R') {
			redKings++;
		} else if (c == 'b') {
			blackPawns++;
		} else if (c == 'B') {
			blackKings++;
		}
	}
	printf ("Red: %d kings %d pawns\n", redKings, redPawns);
	printf ("Black: %d kings %d pawns\n", blackKings, blackPawns);
	*/

	if (display_function) {
		display_function (data, gstate, pstate, num_moves);
	}

	return gstate;
}

int stringToInt (char* str) {
	int base = 1;
	unsigned total = 0;
	int len = strlen (str);
	int i;
	for (i = len - 1; i >= 0; i--) {
		if (str[i] >= '0' && str[i] <= '9') {
			total += base * (str[i] - '0');
		} else {
			return -1;
		}
		base *= 10;
	}
	return total;
}

#ifdef COMPILE_CHANGE
int main (int argc, char *argv[]) {
	int i;
	char* filename = NULL;
	char* efilename = NULL;
	char* hfilename = NULL;
	int num_moves = -1;
	char switchVal = 0;
	for (i = 1; i < argc; i++) {
		char* working = argv[i];
		if (switchVal) {
			switch (switchVal) {
				case 'e':
					efilename = working;
					break;
				case 'h':
					hfilename = working;
					break;
				case 'm':
					num_moves = stringToInt (working);
					break;
				default:
					break;
			}
		}
		if (working[0] == '-') {
			switchVal = working[1];
		} else {
			if (!switchVal && !filename) {
				filename = working;
			}
			switchVal = 0;
		}
	}
	//BASIC_MAPS = makeBasicMaps ();
	struct gameState* gstate;
	struct charbuffer* eFile = NULL;
	struct charbuffer* hFile = NULL;
	if (efilename) eFile = makebuffer ();
	if (hfilename) hFile = makebuffer ();
	struct charbuffer* in;
	if (filename) {
		in = getFile (filename);
		gstate = buildGameState (in, eFile, hFile, num_moves, change_output);
	} else {
		in = getStdin ();
		gstate = buildGameState (in, eFile, hFile, num_moves, change_output);
	}
	if (!gstate) {
		return -1;
	}

	//Output files
	outputH (gstate, hFile, hfilename);
	outputE (gstate, in, eFile, efilename);
}


#endif
