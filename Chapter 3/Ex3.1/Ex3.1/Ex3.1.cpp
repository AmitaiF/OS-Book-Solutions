#include <Windows.h>
#include <iostream>

using namespace std;

#define BUFFER_SIZE 20

int main()
{
	char buffer[BUFFER_SIZE] = { 0 };
	DWORD dwBytesRead;
	
	// create file handle
	HANDLE fileHandle = CreateFileA("hello.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		cout << "ERROR: " << GetLastError() << endl;
		return -1;
	}
	
	// read from the file
	BOOL readFileStatus = ReadFile(fileHandle, buffer, BUFFER_SIZE - 1, &dwBytesRead, NULL);
	if (readFileStatus == FALSE)
	{
		cout << "ERROR: " << GetLastError() << endl;
		return -1;
	}
	
	// close the file handle
	CloseHandle(fileHandle);

	// if we read data from the file
	if (dwBytesRead > 0 && dwBytesRead <= BUFFER_SIZE - 1)
	{
		buffer[dwBytesRead] = '\0';
		cout << "Data from file: " << endl;
		cout << buffer << endl;
	}
	// there is no data in the file
	else if (dwBytesRead == 0)
		cout << "The file was empty!" << endl;
	// we read more the the space in buffer (unexpected result)
	else
		cout << "dwBytesRead is too large!" << endl;

	return 0;
}