#include <stdio.h>
#include <stdlib.h>

#include "charbuffer.h"

struct charbuffer* makebuffer () {
	struct charbuffer* buffer;
       	buffer = malloc (sizeof (struct charbuffer));
	buffer->data = malloc (sizeof(char));
	buffer->length = 0;
	buffer->maxLength = sizeof(char);
	return buffer;
}

struct charbuffer* clonebuffer (struct charbuffer* src) {
	struct charbuffer* dest;
	dest = malloc (sizeof (struct charbuffer));
	dest->data = malloc (sizeof (char) * src->maxLength);
	dest->length = src->length;
	dest->maxLength = src->maxLength;
	int i;
	for (i = 0; i < dest->length; i++) {
		dest->data[i] = src->data[i];
	}
	return dest;
}

void extendBuffer (struct charbuffer* buffer) {
	char* oldData = buffer->data;
	buffer->data = malloc (buffer->maxLength * 2);
	if (!buffer->data) {
		printf ("Error: memory allocation failure while extending buffer");
		exit (1);
	}
	int pos;
	for (pos = 0; pos < buffer->length; pos ++) {
		buffer->data[pos] = oldData[pos];
	}
	buffer->maxLength = buffer->maxLength * 2;
	free (oldData);
}

void add (struct charbuffer* buffer, char data) {
	if (buffer->length > buffer->maxLength - 1) {
		extendBuffer (buffer);
	}
	buffer->data[buffer->length] = data;
	buffer->length += sizeof(char);
}

void addstr (struct charbuffer* buffer, char* str) {
	int i = 0;
	while (str[i]) {
		add (buffer, str[i]);
		i++;
	}
}

void add_data (struct charbuffer* buffer, char* data, int length) {
	int i = 0;
	while (i < length) {
		add (buffer, data[i]);
		i++;
	}
}

char* add_block (struct charbuffer* buffer, int length) {
	int currpos = buffer->length;
	int i = 0;
	while (i < length) {
		add (buffer, 0);
		i++;
	}
	return buffer->data + currpos;
}

struct charbuffer* getFile (char* filename) {
	struct charbuffer* filedata = makebuffer ();
	FILE* infile = fopen (filename, "r");
	int c;
	while (((c = fgetc (infile)) != EOF)) {
		add (filedata, c);
	}
	return filedata;
}

struct charbuffer* getStdin () {
	struct charbuffer* data = makebuffer ();
	int c;
	while (((c = fgetc (stdin)) != EOF)) {
		add (data, c);
	}
	return data;
}

void printBuffer (struct charbuffer* buffer) {
	int i;
	for (i = 0; i < buffer->length; i ++) {
		printf ("%c", buffer->data[i]);
	}
	printf ("\n");
}
