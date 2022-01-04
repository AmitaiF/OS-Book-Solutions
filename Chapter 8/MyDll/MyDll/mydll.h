#pragma once

#ifdef DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif // DLL_EXPORT

extern "C" {
	DECLDIR void SharedFunction();
	void UnsharedFunction();
}