#include<Windows.h>
#include<cstdio>

#define HIDE L"hideme"
#define HIDEA "hideme"

int hook(PCSTR func_to_hook, DWORD new_func_address, PDWORD old_func_address);
PIMAGE_THUNK_DATA getAdressesThunk(PCSTR func_to_hook);
PIMAGE_IMPORT_DESCRIPTOR getImportDescriptor(DWORD base_address);

DWORD oldFindFirstFileW;
DWORD oldFindFirstFileExW;
DWORD oldFindNextFileW;
DWORD oldFindNextFileA;

HANDLE WINAPI newFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);
HANDLE WINAPI newFindFirstFileExW(LPCTSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags);
BOOL WINAPI newFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);
BOOL WINAPI newFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

int main() {
	hook("FindFirstFileW", (DWORD)&newFindFirstFileW, &oldFindFirstFileW);
	hook("FindFirstFileExW", (DWORD)&newFindFirstFileExW, &oldFindFirstFileExW);
	hook("FindNextFileW", (DWORD)&newFindNextFileW, &oldFindNextFileW);

	WIN32_FIND_DATAW data;
	HANDLE handle = FindFirstFileW(L"C:\\Users\\Amitai Farber\\Desktop\\test\\*", &data);
	do {
		wprintf(L"%s\n", data.cFileName);
	} while (FindNextFileW(handle, &data));

	return 0;
}

int hook(PCSTR func_to_hook, DWORD new_func_address, PDWORD old_func_address) {
	// get the correct IAT
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
	DWORD base_address = (DWORD)GetModuleHandle(NULL);

	// get the imoprt descriptor
	auto import_descriptor = getImportDescriptor(base_address);
	if (!import_descriptor)
		return NULL;

	PIMAGE_THUNK_DATA names_thunk, addresses_thunk;
	PIMAGE_IMPORT_BY_NAME func_name_data;
	int index = 0;
	// iterate all the DLLs
	while (import_descriptor[index].Characteristics) {
		// iterate all the function in the DLL, 
		// in order to find the function to hook
		names_thunk = (PIMAGE_THUNK_DATA)(base_address + import_descriptor[index].OriginalFirstThunk);
		addresses_thunk = (PIMAGE_THUNK_DATA)(base_address + import_descriptor[index].FirstThunk);

		if (!names_thunk || !addresses_thunk)
			return NULL;

		while (*(WORD*)names_thunk != NULL && *(WORD*)addresses_thunk != NULL) {
			func_name_data = (PIMAGE_IMPORT_BY_NAME)(base_address + names_thunk->u1.AddressOfData);
			if (!strcmp(func_name_data->Name, func_to_hook))
				return addresses_thunk;
			names_thunk++;
			addresses_thunk++;
		}
		index++;
	}
	return NULL;
}

PIMAGE_IMPORT_DESCRIPTOR getImportDescriptor(DWORD base_address) {
	auto dos_header = (PIMAGE_DOS_HEADER)base_address;
	// check the "MZ" magic word
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	// find NT Header
	auto nt_header = (PIMAGE_NT_HEADERS)(base_address + dos_header->e_lfanew);
	if (nt_header->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	// find Optional Header
	auto optional_header = nt_header->OptionalHeader;
	// check that the exe is 32 bit
	if (optional_header.Magic != 0x10B)
		return NULL;

	// find the import descriptor
	auto import_directory = optional_header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	auto descriptor_start_RVA = import_directory.VirtualAddress;
	return (PIMAGE_IMPORT_DESCRIPTOR)(base_address + descriptor_start_RVA);
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
