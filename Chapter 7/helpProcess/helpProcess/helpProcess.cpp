#include <Windows.h>
#include<iostream>

using namespace std;

int getSystemAlignment();


int main()
{
	// the filename
	LPCWSTR filename = L"..\\..\\Ex7_3\\gibrish.bin";
	int mem_buffer_size = getSystemAlignment();
	
	printf("trying to open map file...\n");
	HANDLE hMapFile = OpenFileMappingA(GENERIC_ALL, FALSE, "gibrish");
	if (hMapFile != NULL) {
		// act like process 2
		printf("Opened!\n");
		printf("mapping...\n");
		LPSTR pBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, mem_buffer_size);
		if (!pBuf) {
			printf("Error while reading the file!\n");
			return -1;
		}
		printf("mapped!\n");
		printf("writing...\n");
		pBuf[0] = '*';
		printf("wrote!\n");
		printf("unmapping...\n");
		UnmapViewOfFile(pBuf);
		printf("unmapped!\n");
	}
	else { // act like process 1
		// handle for the file
		printf("creating handle to the file...\n");
		HANDLE hFile = CreateFileW(filename,
			GENERIC_READ,
			1,						// other processes can share
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		printf("mapping...\n");
		HANDLE hMapFile = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, "gibrish");
		
		LPSTR pBuf;
		printf("waiting for second process...\n");
		do{
			pBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, mem_buffer_size);
		}
		while(pBuf[0] != '*');
		
		
		CloseHandle(hFile);
	}

	CloseHandle(hMapFile);

	return 0;
}


int getSystemAlignment() {
	// get system memory alignment granularity (usually 65536)
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	return sys_info.dwAllocationGranularity;
}