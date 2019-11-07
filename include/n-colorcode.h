#ifndef ncolorcode
#define ncolorcode

#define _POSIX_C_SOURCE 2

#define COLOR_GRAY 8

void printMsg(const char* str, ...);

void printColorBlock(int y, int x, int colorPair);

int quit(int status);

#endif