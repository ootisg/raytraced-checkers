#ifndef charbuffer_inc
#define charbuffer_inc

struct charbuffer {
	char* data;
	int length;
	int maxLength;
};

struct charbuffer* makebuffer ();
void extendBuffer (struct charbuffer* buffer);
void add (struct charbuffer* buffer, char data);
void addstr (struct charbuffer* buffer, char* str);
void add_data (struct charbuffer* buffer, char* data, int length); 
char* add_block (struct charbuffer* buffer, int length);
struct charbuffer* getFile (char* filename);
struct charbuffer* getStdin ();
void printBuffer (struct charbuffer* buffer);

#endif
