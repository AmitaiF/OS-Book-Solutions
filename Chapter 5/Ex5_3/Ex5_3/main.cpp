// This is a solution for exercise 5.3 from Barak Gonen's OS book


#include<stdio.h>
#include<cstdlib>
#include<cstring>
#include<Windows.h>

using namespace std;

#define EXE_PATH "..\\Philosophers\\Philosopher2.exe"
#define NUM_PROCESSES 5

int main()
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi[NUM_PROCESSES];

	// create the argument string
	int size = strlen(EXE_PATH);
	size += 5; // space + null termination + argument #1 + space + argument #2
	PCHAR param = (PCHAR)malloc(size * sizeof(CHAR));

	for (int i = 0; i < NUM_PROCESSES; i++) {
		sprintf_s(param, size, "%s %d %d", EXE_PATH, i, NUM_PROCESSES);

		// prepare empty si and pi for the process
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		si.cb = sizeof(si);

		// create the process
		CreateProcessA(
			NULL,
			param,
			NULL,
			NULL,
			false,
			0,
			NULL,
			NULL,
			&si,
			&pi[i]
		);
	}

	for (int i = 0; i < NUM_PROCESSES; i++)
		WaitForSingleObject(pi[i].hProcess, INFINITE);

	for (int i = 0; i < NUM_PROCESSES; i++)
	{
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}

	free(param);

	return 1;
}