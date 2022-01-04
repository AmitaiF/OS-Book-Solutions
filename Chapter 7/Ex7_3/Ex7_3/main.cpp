// This is a solution for exercise 7.3 from Barak Gonen's OS book
// The goal is to map a file and write to it using 2 processes


#include<stdio.h>
#include<cstdlib>
#include<cstring>
#include<Windows.h>

using namespace std;

#define EXE_PATH "..\\..\\helpProcess\Debug\\helpProcess.exe"
#define NUM_PROCESSES 2

int main()
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi[NUM_PROCESSES];

	// create the argument string
	int size = strlen(EXE_PATH);
	size += 1; // null termination
	PCHAR param = (PCHAR)malloc(size * sizeof(CHAR));

	for (int i = 0; i < NUM_PROCESSES; i++) {
		sprintf_s(param, size, "%s", EXE_PATH);

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

