// This is a solution for exercise 4.4 from Barak Gonen's OS book

// The goal of this exercise is to create a process


#include<stdio.h>
#include<cstdlib>
#include<cstring>
#include<Windows.h>

using namespace std;

#define EXE_PATH "C:\\Users\\Amitai Farber\\Desktop\\hello.exe"
#define PROCESS_ARG "34"

int main()
{
	// create the argument string
	int size = strlen(EXE_PATH) + strlen(PROCESS_ARG);
	size += 2; // space + null termination

	PCHAR param = (PCHAR)malloc(size * sizeof(CHAR));
	sprintf_s(param, size, "%s %s", EXE_PATH, PROCESS_ARG);

	// prepare empty si and pi for the process
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
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
		&pi
	);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	free(param);

	return 1;
}