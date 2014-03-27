#ifndef KEYSET_H
#define KEYSET_H

#include <string>
#include <cstring>
#include <windows.h>

extern DWORD WINAPI KeySetTurnOn(LPVOID lpParam);
extern HINSTANCE instance;
extern HKL layout;
extern std::string keys();

#endif