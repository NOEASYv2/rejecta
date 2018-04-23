#include <Windows.h>
#include <fstream>
#include "Install.hpp"

#include "SDK.hpp"
#include "helpers/Utils.hpp"

/*

Source code base is from Gladv2, and all other changes are Copyrighted by @onliner and @chambers. Under US copyright law, distribution of this source code is a $100,000 fine.

*/


long __stdcall ExceptionFilter(struct _EXCEPTION_POINTERS*);
bool FileExists(std::string &file);

BOOL __stdcall DllMain(HINSTANCE mod, DWORD dwReason, LPVOID res)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:

		// This always fails from what I've seen
		DisableThreadLibraryCalls(mod);

		if (FileExists(std::string("Rejecta.net.log")))
			std::remove("Rejecta.net.log");

		Global::hmDll = mod;
		SetUnhandledExceptionFilter(ExceptionFilter);

		Installer::installGladiator();

		break;
		
	case DLL_PROCESS_DETACH:

		Installer::uninstallGladiator();

		break;
	}
	return TRUE;
}

bool FileExists(std::string &file)
{
	std::ifstream in(file);
	return in.good();
}

long __stdcall ExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{

	int m_ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	int m_exceptionInfo_0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
	int m_exceptionInfo_1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
	int m_exceptionInfo_2 = ExceptionInfo->ExceptionRecord->ExceptionInformation[2];


	return EXCEPTION_CONTINUE_SEARCH;
}