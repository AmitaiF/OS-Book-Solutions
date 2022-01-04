#include "pch.h"

int getCharCount(LPSTR data, char charToCount);
int getSystemAlignment();


int main()
{
	// the filename
	LPCWSTR filename = L"C:\\Users\\Amitai Farber\\Desktop\\קבצים\\פרויקטים\\תכנות\\OS Book Solutions\\Chapter 7\\Ex7_2\\gibrish.bin";
	char charToCount = 'A';

	// handle for the file
	HANDLE hFile = CreateFileW(filename,
		GENERIC_READ,
		0,						// other processes can't share
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// get the size of the file
	DWORD file_size = GetFileSize(hFile, NULL);

	HANDLE hMapFile = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, "myFile");

	int mem_buffer_size = getSystemAlignment();
	int buffer_number = 0;
	int count = 0;
	int file_location = buffer_number * mem_buffer_size;
	LPSTR pBuf;

	while (file_location <= (file_size - mem_buffer_size)) {
		pBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, file_location, mem_buffer_size);
		count += getCharCount(pBuf, charToCount);

		buffer_number++;
		file_location = buffer_number * mem_buffer_size;
		UnmapViewOfFile(pBuf);
		// just that we would see the program in the process explorer
		Sleep(100);
	}

	// now we need to take care of the reminder of the file, since we mapped chunks of 'mem_buffer_size'
	int reminder = file_size - file_location;
	pBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, file_location, reminder);
	count += getCharCount(pBuf, charToCount);
	UnmapViewOfFile(pBuf);

	cout << "There is " << count << " times " << charToCount << endl;

	CloseHandle(hFile);
	CloseHandle(hMapFile);

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


int getSystemAlignment() {
	// get system memory alignment granularity (usually 65536)
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	return sys_info.dwAllocationGranularity;
}