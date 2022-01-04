// This is a solution for exercise 5.2 from Barak Gonen's OS book

// The goal of this exercise is to create 2 processes that uses mutex


#include<stdio.h>
#include<cstdlib>
#include<cstring>
#include<Windows.h>

using namespace std;

//HelloMutex1 is without mutex
//#define EXE_PATH "..\\HelloMutex1.exe"
#define EXE_PATH "..\\HelloMutex2.exe"
#define NUM_PROCESSES 2

int main()
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi[NUM_PROCESSES];

	// create the argument string
	int size = strlen(EXE_PATH);
	size += 3; // space + null termination + argument
	PCHAR param = (PCHAR)malloc(size * sizeof(CHAR));

	for (int i = 0; i < NUM_PROCESSES; i++){
		sprintf_s(param, size, "%s %d", EXE_PATH, i);

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