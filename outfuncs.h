#pragma once

#include "charbuffer.h"
#include "loadfuncs.h"
#include "movefuncs.h"

void outputH (struct gameState* state, struct charbuffer* out, char* filename);
void outputE (struct gameState* state, struct charbuffer* in, struct charbuffer* out, char* filename);
void writeToFile (struct charbuffer* out, char* filename);
