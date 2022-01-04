// This is a solution for exercise 4.5 from Barak Gonen's OS book

// The goal of this exercise is to create 4 threads
// that will count up to a certain number and will
// print their count along with their ID

#include<Windows.h>
#include<stdio.h>

struct IdAndCount
{
	int ID;
	int count;
};

DWORD WINAPI count_and_print(LPVOID param) {
	int ID = ((IdAndCount*)param)->ID;
	int count = ((IdAndCount*)param)->count;

	for (int i = 0; i < count; i++)
		printf("Thread %d, var = %d\n", ID, i);

	return 1;
}

int main() {
	HANDLE handles[4];
	for (int i = 0; i < 4; i++)
	{
		IdAndCount* arg = (IdAndCount*)malloc(sizeof(IdAndCount));
		arg->count = 1000;
		arg->ID = (i + 1);

		handles[i] = CreateThread(
			NULL,
			0,
			count_and_print,
			(LPVOID)arg,
			0,
			NULL
		);
	}
	WaitForMultipleObjects(4, handles, TRUE, INFINITE);
	return 0;
}