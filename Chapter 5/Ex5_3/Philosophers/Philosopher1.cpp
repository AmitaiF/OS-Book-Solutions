#include<stdio.h>
#include<Windows.h>

#define MUTEX_BASE_NAME "Stick #"

int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Wrong number of arguments!\n");
		return -1;
	}

	int ID = atoi(argv[1]);
	int totalPhlosophers = atoi(argv[2]);

	int firstLock = ID;
	int secondLock = (ID + 1) % totalPhlosophers;

	// bool finished = false;

	int size = strlen(MUTEX_BASE_NAME);
	size += 2; // null + argument
	PCHAR firstMutexName = (PCHAR)malloc(size * sizeof(CHAR));
	PCHAR secondMutexName = (PCHAR)malloc(size * sizeof(CHAR));

	sprintf_s(firstMutexName, size, "%s%d", MUTEX_BASE_NAME, firstLock);
	sprintf_s(secondMutexName, size, "%s%d", MUTEX_BASE_NAME, secondLock);

	HANDLE hFirstMutex = CreateMutexA(NULL, FALSE, firstMutexName);
	// we using Sleep to ensure context switching
	Sleep(100);
	HANDLE hSecondMutex = CreateMutexA(NULL, FALSE, secondMutexName);
	
	WaitForSingleObject(hFirstMutex, INFINITE);
	WaitForSingleObject(hSecondMutex, INFINITE);

	printf("Philosopher #%d is eating!\n", ID);

	ReleaseMutex(hFirstMutex);
	ReleaseMutex(hSecondMutex);

	CloseHandle(hFirstMutex);
	CloseHandle(hSecondMutex);

	free(firstMutexName);
	free(secondMutexName);

	return 0;
}