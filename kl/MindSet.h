#ifndef MINDSET_H
#define MINDSET_H

#include <windows.h>

extern struct mindSet mindset;
extern DWORD WINAPI MindSetTurnOn(LPVOID lpParam);

struct mindSet {
	char *keys;
	float meditation;
	float attention;
	float al, ah, bl, bh, gl, gh, d,  t;
};

#endif
