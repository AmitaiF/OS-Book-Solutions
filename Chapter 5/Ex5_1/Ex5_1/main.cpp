// This is a solution for exercise 5.1 from Barak Gonen's OS book

// The goal of this exercise is to solve the Dining Philosofers
// problem using Locks (CRITICAL_SECTION)

#include<Windows.h>
#include<stdio.h>

#define NUM_THREADS 5

// Global lock for preventing race conditions.
// Works only for threads in this process
CRITICAL_SECTION gCriticalSections[NUM_THREADS];

DWORD WINAPI eat_func(LPVOID param) {
	bool finished = false;
	int ID = (int)param;

	int firstLock = ID;
	int secondLock = (ID + 1) % NUM_THREADS;

	while (!finished) {
		EnterCriticalSection(&gCriticalSections[firstLock]);
		// we can use Sleep to ensure context switching
		// Sleep(10);
		if (TryEnterCriticalSection(&gCriticalSections[secondLock])) {
			printf("Philosofer #%d is eating\n", ID + 1);
			finished = true;
			LeaveCriticalSection(&gCriticalSections[secondLock]);
		}
		else {
			LeaveCriticalSection(&gCriticalSections[firstLock]);
		}
	}
	LeaveCriticalSection(&gCriticalSections[firstLock]);

	return 1;
}

int main()
{
	for (int i = 0; i < NUM_THREADS; i++)
		InitializeCriticalSection(&gCriticalSections[i]);

	HANDLE hThread[NUM_THREADS];

	for (INT i = 0; i < NUM_THREADS; i++) {
		hThread[i] = CreateThread(
			NULL,		//default security attributes
			0,			//default stack size
			eat_func,	//thread function
			(LPVOID)i,	//thread param
			0,			//default creation flags
			NULL		//return thread identifier
		);
	}

	WaitForMultipleObjects(NUM_THREADS, hThread, TRUE, INFINITE);

	for (int i = 0; i < NUM_THREADS; i++)
		DeleteCriticalSection(&gCriticalSections[i]);
}