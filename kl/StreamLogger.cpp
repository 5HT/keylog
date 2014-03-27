
#include <fstream>
#include <string>
#include <windows.h>

#include "KeySet.h"
#include "MindSet.h"

#pragma execution_character_set("utf-8")

using namespace std;

struct packet {
	time_t time;
	char *handword;
	struct mindSet mind;
	int version;
};

HANDLE hTimerQueue = CreateTimerQueue();
HANDLE hTimer = NULL;
int arg = 0;
SYSTEMTIME timestamp;
HINSTANCE instance;
HANDLE doneEvent;
TCHAR semName[] = TEXT("StreamLoggerSemaphore");
HANDLE sem;

VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	GetSystemTime(&timestamp);
	ofstream testFile;
	testFile.open("StreamLogger.txt", ios::out | ios::binary | ios::app);

	union { FILETIME a; long long b; } ts;
	SystemTimeToFileTime(&timestamp, &ts.a);

	// TODO: write this also to synrc.com using ERL binary format

	testFile << "Stream,"  << ((int)layout >> 16) << "," 
		                   << ts.b << ","
						   << keys() << "," 
						   << mindset.attention << ","
						   << mindset.meditation << ","
						   << mindset.al << ","
						   << mindset.ah << ","
						   << mindset.bl << ","
						   << mindset.bh << ","
						   << mindset.gl << ","
						   << mindset.gh << ","
						   << mindset.d << ","
						   << mindset.t << "\r\n";
	testFile.close();

	if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
		(WAITORTIMERCALLBACK)TimerRoutine, &arg, 10, 0, 0));
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{	
	sem = CreateSemaphore(NULL, 1, 1, semName);
	if (WaitForSingleObject(sem, 0) == WAIT_TIMEOUT)
	{
		MessageBox(NULL, L"There are already one instance of this application running",
			             L"Error", MB_OK | MB_ICONSTOP);
		CloseHandle(sem);
		return -1;
	}

	instance = hInstance;
	doneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD dwThreadIdArray[2];

	if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
		(WAITORTIMERCALLBACK)TimerRoutine, &arg, 10, 0, 0))
	{
		printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
		return 3;
	}

	CreateThread(NULL, 0, KeySetTurnOn,  NULL, 0, &dwThreadIdArray[0]);
	CreateThread(NULL, 0, MindSetTurnOn, NULL, 0, &dwThreadIdArray[1]);

	if (WaitForSingleObject(doneEvent, INFINITE) != WAIT_OBJECT_0)
		printf("WaitForSingleObject failed (%d)\n", GetLastError());

}

 