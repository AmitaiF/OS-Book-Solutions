#include "pch.h"

int getCharCount(LPSTR data, char charToCount);

int main()
{
	// the filename
	LPCWSTR filename = L"C:\\Users\\Amitai Farber\\Desktop\\קבצים\\פרויקטים\\תכנות\\OS Book Solutions\\Chapter 7\\Ex7\\gibrish.bin";
	char charToCount = 'A';
	
	// handle for the file
	HANDLE hFile = CreateFileW(filename,
						GENERIC_READ,
						0,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	
	// get the size of the file
	DWORD file_size = GetFileSize(hFile, NULL);
	
	// buffer for reading the file
	LPVOID pBuf = malloc(file_size + 1);
	
	// read the file to the buffer
	ReadFile(hFile, pBuf, file_size, NULL, NULL);
	
	// get the data from the buffer
	LPSTR data = (LPSTR)pBuf;

	Sleep(10000);

	int count = getCharCount(data, charToCount);
	cout << "There is " << count << " times " << charToCount << endl;

	free(pBuf);
	CloseHandle(hFile);
	return 0;
}


int getCharCount(LPSTR data, char charToCount)
{
	int count = 0;

	int size = strlen(data);
	for (int i = 0; i < size; i++)
		if (data[i] == charToCount)
			count++;
	return count;
}