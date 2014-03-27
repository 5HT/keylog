// ThinkGear MindSet stream driver

#include <string>
#include <map>
#include <windows.h>
#include <strsafe.h>
#include <time.h>
#include "MindSet.h"

#include "thinkgear.h"

using namespace std;

float value; 
struct mindSet mindset = {};
char *comPortName = NULL;
int   dllVersion = 0;
int   connectionId = 0;
int   packetsRead = 0;
int   errCode = 0;

int MindSetConnect()
{

	dllVersion = TG_GetDriverVersion();
	printf("ThinkGear DLL version: %d\n", dllVersion);

	connectionId = TG_GetNewConnectionId();
	if (connectionId < 0) {
		fprintf(stderr, "ERROR: TG_GetNewConnectionId() returned %d.\n", connectionId);
		return -1;
	}

	for (int i = 0; i < 99; i++)
	{

		string comPortName = "\\\\.\\COM" + to_string(i);
		
		errCode = TG_Connect(connectionId,
			comPortName.c_str(),
			TG_BAUD_9600,
			TG_STREAM_PACKETS);

		if (errCode < 0) continue; else break;

	}
}

DWORD WINAPI MindSetTurnOn(LPVOID lpParam)
{
	MindSetConnect();

	packetsRead = 0;

	while (true)
	{

			errCode = TG_ReadPackets(connectionId, 1);

			if (errCode == 1) {

				packetsRead++;

				value = TG_GetValueStatus(connectionId, TG_DATA_ATTENTION);
				if (value != 0) mindset.attention = TG_GetValue(connectionId, TG_DATA_ATTENTION);

				value = TG_GetValueStatus(connectionId, TG_DATA_MEDITATION);
				if (value != 0) mindset.meditation = TG_GetValue(connectionId, TG_DATA_MEDITATION);

				value = TG_GetValueStatus(connectionId, TG_DATA_DELTA);
				if (value != 0) mindset.d = TG_GetValue(connectionId, TG_DATA_DELTA);

				value = TG_GetValueStatus(connectionId, TG_DATA_THETA);
				if (value != 0) mindset.t = TG_GetValue(connectionId, TG_DATA_THETA);

				value = TG_GetValueStatus(connectionId, TG_DATA_ALPHA1);
				if (value != 0) mindset.al = TG_GetValue(connectionId, TG_DATA_ALPHA1);

				value = TG_GetValueStatus(connectionId, TG_DATA_ALPHA2);
				if (value != 0) mindset.ah = TG_GetValue(connectionId, TG_DATA_ALPHA2);

				value = TG_GetValueStatus(connectionId, TG_DATA_BETA1);
				if (value != 0) mindset.bl = TG_GetValue(connectionId, TG_DATA_BETA1);

				value = TG_GetValueStatus(connectionId, TG_DATA_BETA2);
				if (value != 0) mindset.bh = TG_GetValue(connectionId, TG_DATA_BETA2);

				value = TG_GetValueStatus(connectionId, TG_DATA_GAMMA1);
				if (value != 0) mindset.gl = TG_GetValue(connectionId, TG_DATA_GAMMA1);

				value = TG_GetValueStatus(connectionId, TG_DATA_GAMMA2);
				if (value != 0) mindset.gh = TG_GetValue(connectionId, TG_DATA_GAMMA2);

			}

		}

		TG_FreeConnection(connectionId);

		return 0;

}

