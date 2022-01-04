#include<cstdio>
#include<Windows.h>

#define DLL_EXPORT
#include"mydll.h"

extern "C" {
	DECLDIR void SharedFunction() {
		printf("Shared function.\n");
	}
	void UnsharedFunction() {
		printf("Unshared function.\n");
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		SharedFunction();
		UnsharedFunction();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}