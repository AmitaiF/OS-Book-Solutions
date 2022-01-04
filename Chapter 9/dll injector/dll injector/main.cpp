// In this exercise, we inject a dll to a  process.

#include<Windows.h>
#include<cstdio>
#include<tlhelp32.h>

#define DLL_PATH "..\\x64\\Debug\\IAT Hooking.dll"
#define PROCESS L"cmd.exe"

DWORD GetProcessIDbyName(wchar_t* proc);

int main()
{

	// Get full path of DLL to inject
	LPSTR full_path = new CHAR[MAX_PATH];
	DWORD pathLen = GetFullPathNameA(DLL_PATH, MAX_PATH, full_path, NULL);

	// Get LoadLibrary function address (the address doesn't change at remote process)
	PVOID addrLoadLibrary = (PVOID)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");

	// Open remote process
	DWORD id = GetProcessIDbyName((PWCHAR)PROCESS);
	if (id == 0) {
		printf("Process not found!\n");
		return -1;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
	if (hProcess == NULL) {
		printf("Couldn't open the process\n");
		return -1;
	}

	// Get a pointer to memory location in remote process,
	// big enough to store DLL path
	PVOID memAddr = (PVOID)VirtualAllocEx(hProcess,
		NULL,
		pathLen,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE);
	if (NULL == memAddr) {
		printf("Couldn't allocate memory\n");
		return -1;
	}

	// Write DLL name to remote process memory
	BOOL check = WriteProcessMemory(hProcess, memAddr, full_path, pathLen, NULL);
	if (0 == check) {
		printf("Couldn't write memory\n");
		return -1;
	}

	// Open remote thread, while executing LoadLibrary
	// with parameter DLL name, will trigger DLLMain
	HANDLE hRemote = CreateRemoteThread(hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)addrLoadLibrary,
		memAddr,
		0,
		NULL);
	if (NULL == hRemote) {
		printf("%d", GetLastError());
		printf("Couldn't create remote thread\n");
		return -1;
	}

	WaitForSingleObject(hRemote, INFINITE);
	check = CloseHandle(hRemote);
	return 0;
}


DWORD GetProcessIDbyName(wchar_t* proc)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	auto pe = PROCESSENTRY32W{ sizeof(PROCESSENTRY32W) };

	if (Process32First(snapshot, &pe)) {
		do {
			if (!_wcsicmp(proc, pe.szExeFile)) {
				CloseHandle(snapshot);
				return pe.th32ProcessID;
			}
		} while (Process32Next(snapshot, &pe));
	}
	CloseHandle(snapshot);
	return 0;
}