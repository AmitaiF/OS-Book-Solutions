// This is a solution for exercise 4.3 from Barak Gonen's OS book

// The goal of this exercise is to create a thread
// and pass a struct as a parameter

#include "pch.h"


struct TwoNumbers
{
	INT a;
	INT b;
};


DWORD WINAPI print_stars(LPVOID param) {
	TwoNumbers* nums = (TwoNumbers*)param;
	int a = nums->a;
	int b = nums->b;

	for (int i = 0; i < a; i++) {
		for (int j = 0; j <= i; j++)
			printf("*");
		printf("\n");
	}

	for (int i = 0; i < b; i++) {
		for (int j = 0; j <= i; j++)
			printf("*");
		printf("\n");
	}

	return 0;
}

int main() {
	TwoNumbers nums;
	nums.a = 5;
	nums.b = 3;

	HANDLE hThread = CreateThread(
		NULL,			// default security attributes
		0,				// default stack size
		print_stars,	// thread function
		&nums,			// thread param
		0,				// default creation flag
		NULL			// return thread ID
	);
	
	// we call it in order to wait for the thread to end
	// otherwise the main thread would end before the thread we crearted
	WaitForSingleObject(hThread, INFINITE);
	
	return 0;
}