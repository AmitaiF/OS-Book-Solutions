#include<Windows.h>
#include<cstdio>

#define DLL_PATH "MyDll.dll"

typedef void(*PFUNC)(void);


int main() {
	HMODULE hModule = LoadLibraryA(DLL_PATH);
	if (NULL == hModule) {
		printf("Failed to load dll\n");
		return -1;
	}
	PFUNC pFunc = (PFUNC)GetProcAddress(hModule, "SharedFunction");
	if (NULL != pFunc) {
		(*pFunc)();
	}
	else {
		printf("failed to load function\n");
		return -1;
	}
	return 0;
}